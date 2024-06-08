/* Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "iotsdk/nx_driver_mdh.h"

#include "nx_api.h"
#include "nx_secure_tls_api.h"
#include "nxd_dhcp_client.h"
#include "nxd_dns.h"
#include "nxd_sntp_client.h"
#include "sample_config.h"

extern VOID
sample_entry(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr, UINT (*unix_time_callback)(ULONG *unix_time));

static NX_PACKET_POOL gs_packet_pool = {0};
static NX_IP gs_network_ip = {0};
static NX_DNS gs_dns_client = {0};
static NX_DHCP gs_dhcp_client = {0};
static NX_SNTP_CLIENT gs_sntp_client = {0};

static ULONG gs_unix_time_base;

static ULONG gs_network_ip_memory_buffer[STACK_SIZE_NETWORK_IP / sizeof(ULONG)];
static ULONG gs_packet_pool_buffer[SAMPLE_POOL_SIZE / sizeof(ULONG)];
static ULONG gs_arp_cache_memory_buffer[SAMPLE_ARP_CACHE_SIZE / sizeof(ULONG)];
static ULONG gs_sample_helper_thread_memory_buffer[STACK_SIZE_SAMPLE_HELPER / sizeof(ULONG)];

static const CHAR *sntp_servers[] = {
    "0.pool.ntp.org",
    "1.pool.ntp.org",
    "2.pool.ntp.org",
    "3.pool.ntp.org",
};
static UINT sntp_server_index;

static void sample_helper_thread_entry(ULONG context);
static UINT dhcp_wait(void);
static UINT dns_create(ULONG dns_server_address);
static UINT sntp_time_sync(void);
static UINT sntp_time_sync_internal(ULONG sntp_server_address);
static UINT fetch_unix_time_cb(ULONG *unix_time);
static UINT init_nx_ip(void);

int main(void)
{
    tx_kernel_enter();
}

void tx_application_define(void *first_unused_memory)
{
    NX_PARAMETER_NOT_USED(first_unused_memory);

    static TX_THREAD sample_helper_thread;
    UINT status = tx_thread_create(&sample_helper_thread,
                                   "Sample helper",
                                   sample_helper_thread_entry,
                                   0,
                                   gs_sample_helper_thread_memory_buffer,
                                   STACK_SIZE_SAMPLE_HELPER,
                                   THREAD_PRIORITY_SAMPLE_HELPER,
                                   THREAD_PRIORITY_SAMPLE_HELPER,
                                   TX_NO_TIME_SLICE,
                                   TX_AUTO_START);
    if (status != TX_SUCCESS) {
        printf("Error (0x%02x): Sample helper thread could not be created.\r\n", status);
        return;
    }

    status = init_nx_ip();
    if (status != NX_SUCCESS) {
        printf("Error (0x%02x): Failed to initialize Network IP.\r\n", status);
        return;
    }
}

static UINT init_nx_ip(void)
{
    nx_system_initialize();

    UINT status = nx_packet_pool_create(&gs_packet_pool,
                                        "NetX Packet Pool",
                                        PACKET_SIZE,
                                        (UCHAR *)gs_packet_pool_buffer,
                                        sizeof(gs_packet_pool_buffer));
    if (status != NX_SUCCESS) {
        printf("Error (0x%02x): NetX Duo packet pool could not be created.\r\n", status);
        return status;
    }

    status = nx_ip_create(&gs_network_ip,
                          "NetX IP Instance 0",
                          IP_ADDRESS(0, 0, 0, 0),
                          IP_ADDRESS(0, 0, 0, 0),
                          &gs_packet_pool,
                          nx_driver_mdh_ip_link_fsm,
                          (UCHAR *)gs_network_ip_memory_buffer,
                          sizeof(gs_network_ip_memory_buffer),
                          THREAD_PRIORITY_NETWORK_IP);
    if (status != NX_SUCCESS) {
        printf("Error (0x%02x): NetX Duo IP instance could not be created.\r\n", status);
        return status;
    }

    status = nx_ip_fragment_enable(&gs_network_ip);
    if (status != NX_SUCCESS) {
        printf("Error (0x%02x): IP fragment processing could not be enabled.\r\n", status);
        return status;
    }

    status = nx_arp_enable(&gs_network_ip, (VOID *)gs_arp_cache_memory_buffer, sizeof(gs_arp_cache_memory_buffer));
    if (status != NX_SUCCESS) {
        printf("Error (0x%02x): ARP could not be enabled.\r\n", status);
        return status;
    }

    status = nx_tcp_enable(&gs_network_ip);
    if (status != NX_SUCCESS) {
        printf("Error (0x%02x): TCP could not be enabled\r\n", status);
        return status;
    }

    status = nx_udp_enable(&gs_network_ip);
    if (status != NX_SUCCESS) {
        printf("Error (0x%02x): UDP could not be enabled\r\n", status);
        return status;
    }

    nx_secure_tls_initialize();

    return status;
}

static void sample_helper_thread_entry(ULONG context)
{
    NX_PARAMETER_NOT_USED(context);

    if (dhcp_wait()) {
        printf("Failed to get the IP address!\r\n");
        return;
    }

    ULONG ip_address = 0;
    ULONG network_mask = 0;
    nx_ip_address_get(&gs_network_ip, &ip_address, &network_mask);
    ULONG gateway_address = 0;
    nx_ip_gateway_address_get(&gs_network_ip, &gateway_address);

    ULONG dns_server_address[3];
    UINT dns_server_address_size = sizeof(dns_server_address);
    nx_dhcp_interface_user_option_retrieve(
        &gs_dhcp_client, 0, NX_DHCP_OPTION_DNS_SVR, (UCHAR *)(dns_server_address), &dns_server_address_size);

    UINT status = dns_create(dns_server_address[0]);
    if (status != NX_SUCCESS) {
        printf("Error (0x%02x): Failed to create DNS.\r\n", status);
        return;
    }

    status = sntp_time_sync();
    if (status != NX_SUCCESS) {
        printf("Error (0x%02x): Failed to sync SNTP time.\r\n", status);
        printf("Set Time to default value: 0x%02x.", SAMPLE_SYSTEM_TIME);
        gs_unix_time_base = SAMPLE_SYSTEM_TIME;
    } else {
        printf("SNTP Time successfully synchronized.\r\n");
    }

    UINT unix_time;
    fetch_unix_time_cb((ULONG *)&unix_time);
    srand(unix_time);

    sample_entry(&gs_network_ip, &gs_packet_pool, &gs_dns_client, fetch_unix_time_cb);
}

static UINT dhcp_wait(void)
{
    printf("DHCP In Progress...\r\n");

    UINT status = nx_dhcp_create(&gs_dhcp_client, &gs_network_ip, "DHCP Client");
    if (status != NX_SUCCESS) {
        printf("Error (0x%02x): DHCP client could not be created\r\n", status);
        return status;
    }

    status = nx_dhcp_user_option_request(&gs_dhcp_client, NX_DHCP_OPTION_NTP_SVR);
    if (status != NX_SUCCESS) {
        nx_dhcp_delete(&gs_dhcp_client);
        return status;
    }

    status = nx_dhcp_start(&gs_dhcp_client);
    if (status != NX_SUCCESS) {
        nx_dhcp_delete(&gs_dhcp_client);
        return status;
    }

    ULONG actual_status = UINT32_MAX;
    const ULONG needed_status = NX_IP_ADDRESS_RESOLVED;
    status = nx_ip_status_check(&gs_network_ip, needed_status, &actual_status, DHCP_WAIT_TIME);
    if ((status != NX_SUCCESS) && (actual_status != needed_status)) {
        nx_dhcp_delete(&gs_dhcp_client);
        printf("Error (0x%02x): Could not resolve DHCP. Expected `0x%02x` but got `0x%02x`.\r\n",
               status,
               (UINT)needed_status,
               (UINT)actual_status);
        return status;
    }

    return NX_SUCCESS;
}

static UINT dns_create(ULONG dns_server_address)
{
    UINT status;

    // Create a DNS instance for the Client.  Note this function will create
    // the DNS Client packet pool for creating DNS message packets intended
    // for querying its DNS server.
    status = nx_dns_create(&gs_dns_client, &gs_network_ip, (UCHAR *)"DNS Client");
    if (status != NX_SUCCESS) {
        return status;
    }

    // Add an IPv4 server address to the Client list.
    status = nx_dns_server_add(&gs_dns_client, dns_server_address);
    if (status != NX_SUCCESS) {
        nx_dns_delete(&gs_dns_client);
        return status;
    }

    return NX_SUCCESS;
}

static UINT sntp_time_sync_internal(ULONG sntp_server_address)
{
    UINT status = nx_sntp_client_create(&gs_sntp_client, &gs_network_ip, 0, &gs_packet_pool, NX_NULL, NX_NULL, NX_NULL);
    if (status != NX_SUCCESS) {
        return status;
    }

    status = nx_sntp_client_initialize_unicast(&gs_sntp_client, sntp_server_address);
    if (status != NX_SUCCESS) {
        nx_sntp_client_delete(&gs_sntp_client);
        return status;
    }

    status = nx_sntp_client_set_local_time(&gs_sntp_client, 0, 0);
    if (status != NX_SUCCESS) {
        nx_sntp_client_delete(&gs_sntp_client);
        return status;
    }

    status = nx_sntp_client_run_unicast(&gs_sntp_client);
    if (status != NX_SUCCESS) {
        nx_sntp_client_stop(&gs_sntp_client);
        nx_sntp_client_delete(&gs_sntp_client);
        return status;
    }

    // Wait till updates are received or timeout
    for (UINT i = 0; i < SNTP_MAX_UPDATE_CHECK; i++) {

        // Verify we have a valid SNTP service running.
        UINT server_status;
        status = nx_sntp_client_receiving_updates(&gs_sntp_client, &server_status);
        if ((status == NX_SUCCESS) && (server_status == NX_TRUE)) {
            ULONG sntp_seconds;
            ULONG sntp_fraction;
            status = nx_sntp_client_get_local_time(&gs_sntp_client, &sntp_seconds, &sntp_fraction, NX_NULL);
            if (status != NX_SUCCESS) {
                continue;
            }

            ULONG system_time_in_second = tx_time_get() / TX_TIMER_TICKS_PER_SECOND;

            // Convert to Unix epoch and minus the current system time.
            gs_unix_time_base = (sntp_seconds - (system_time_in_second + SAMPLE_UNIX_TO_NTP_EPOCH_SECOND));

            // Time sync successfully.

            nx_sntp_client_stop(&gs_sntp_client);
            nx_sntp_client_delete(&gs_sntp_client);

            return NX_SUCCESS;
        }

        tx_thread_sleep(SNTP_UPDATE_SLEEP_INTERVAL);
    }

    // Time sync failed.

    nx_sntp_client_stop(&gs_sntp_client);
    nx_sntp_client_delete(&gs_sntp_client);

    return NX_NOT_SUCCESSFUL;
}

static UINT sntp_time_sync(void)
{
    ULONG sntp_server_address[3];
    UINT sntp_server_address_size = sizeof(sntp_server_address);
    UINT status = nx_dhcp_interface_user_option_retrieve(
        &gs_dhcp_client, 0, NX_DHCP_OPTION_NTP_SVR, (UCHAR *)(sntp_server_address), &sntp_server_address_size);
    if (status == NX_SUCCESS) {
        for (UINT i = 0; (i * 4) < sntp_server_address_size; i++) {
            // Start SNTP to sync the local time.
            status = sntp_time_sync_internal(sntp_server_address[i]);
            if (status == NX_SUCCESS) {
                return status;
            }
        }
    }

    // Sync time by NTP server array.
    for (UINT i = 0; i < SNTP_MAX_SYNC_CHECK; i++) {
        // Make sure the network is still valid.
        ULONG gateway_address;
        while (nx_ip_gateway_address_get(&gs_network_ip, &gateway_address)) {
            tx_thread_sleep(NX_IP_PERIODIC_RATE);
        }

        // Look up SNTP Server address.
        status = nx_dns_host_by_name_get(
            &gs_dns_client, (UCHAR *)sntp_servers[sntp_server_index], &sntp_server_address[0], 5 * NX_IP_PERIODIC_RATE);
        if (status == NX_SUCCESS) {

            status = sntp_time_sync_internal(sntp_server_address[0]);
            if (status == NX_SUCCESS) {
                return status;
            }
        }

        // Switch SNTP server every time.
        sntp_server_index = (sntp_server_index + 1) % (sizeof(sntp_servers) / sizeof(sntp_servers[0]));
    }

    return NX_NOT_SUCCESSFUL;
}

static UINT fetch_unix_time_cb(ULONG *unix_time)
{
    *unix_time = gs_unix_time_base + (tx_time_get() / TX_TIMER_TICKS_PER_SECOND);
    return NX_SUCCESS;
}
