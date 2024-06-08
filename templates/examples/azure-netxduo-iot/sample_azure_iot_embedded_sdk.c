/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/

/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 */

#include "sample_azure_iot_credentials.h"
#include "sample_config.h"

#include "nx_api.h"
#include "nx_azure_iot_cert.h"
#include "nx_azure_iot_ciphersuites.h"
#include "nx_azure_iot_hub_client.h"

#include <stdio.h>

static VOID printf_packet(NX_PACKET *packet_ptr);
static VOID on_connection_status_update(NX_AZURE_IOT_HUB_CLIENT *hub_client_ptr, UINT status);
static UINT initialize_iothub(NX_AZURE_IOT_HUB_CLIENT *iothub_client_ptr);
static void telemetry_thread_entry(ULONG parameter);
static void cloud_to_device_thread_entry(ULONG parameter);
static void on_azure_iot_log(az_log_classification classification, UCHAR *msg, UINT msg_len);
extern VOID sample_connection_monitor(NX_IP *ip_ptr,
                                      NX_AZURE_IOT_HUB_CLIENT *iothub_client_ptr,
                                      UINT connection_status,
                                      UINT (*iothub_init)(NX_AZURE_IOT_HUB_CLIENT *hub_client_ptr));

static NX_SECURE_X509_CERT root_ca_cert = {0};
static NX_SECURE_X509_CERT root_ca_cert_2 = {0};
static NX_SECURE_X509_CERT root_ca_cert_3 = {0};
static UCHAR nx_azure_iot_tls_metadata_buffer[NX_AZURE_IOT_TLS_METADATA_BUFFER_SIZE] = {0};
static ULONG nx_azure_iot_thread_stack[STACK_SIZE_NX_AZURE_IOT / sizeof(ULONG)] = {0};
static NX_AZURE_IOT nx_azure_iot = {0};
static NX_AZURE_IOT_HUB_CLIENT iothub_client = {0};
static volatile UINT sample_connection_status = NX_AZURE_IOT_NOT_INITIALIZED;
static volatile UINT sample_properties_request_sent = NX_FALSE;
static TX_THREAD sample_telemetry_thread = {0};
static ULONG sample_telemetry_thread_stack[STACK_SIZE_SAMPLE / sizeof(ULONG)] = {0};
static TX_THREAD sample_c2d_thread = {0};
static ULONG sample_c2d_thread_stack[STACK_SIZE_SAMPLE / sizeof(ULONG)] = {0};
static const CHAR *sample_properties[MAX_PROPERTY_COUNT][2] = {{"propertyA", "valueA"}, {"propertyB", "valueB"}};

void sample_entry(NX_IP *ip_ptr,
                  NX_PACKET_POOL *pool_ptr,
                  NX_DNS *dns_ptr,
                  UINT (*unix_time_callback)(ULONG *unix_time))
{
    nx_azure_iot_log_init(on_azure_iot_log);

    UINT status = nx_azure_iot_create(&nx_azure_iot,
                                      (UCHAR *)"Azure IoT",
                                      ip_ptr,
                                      pool_ptr,
                                      dns_ptr,
                                      nx_azure_iot_thread_stack,
                                      sizeof(nx_azure_iot_thread_stack),
                                      THREAD_PRIORITY_NX_AZURE_IOT,
                                      unix_time_callback);
    if (status != NX_AZURE_IOT_SUCCESS) {
        printf("Failed on nx_azure_iot_create!: error code = 0x%08x\r\n", status);
        return;
    }

    status = nx_secure_x509_certificate_initialize(&root_ca_cert,
                                                   (UCHAR *)_nx_azure_iot_root_cert,
                                                   (USHORT)_nx_azure_iot_root_cert_size,
                                                   NX_NULL,
                                                   0,
                                                   NULL,
                                                   0,
                                                   NX_SECURE_X509_KEY_TYPE_NONE);
    if (status != NX_SECURE_X509_SUCCESS) {
        printf("Failed to initialize ROOT CA certificate!: error code = 0x%08x\r\n", status);
        nx_azure_iot_delete(&nx_azure_iot);
        return;
    }

    status = nx_secure_x509_certificate_initialize(&root_ca_cert_2,
                                                   (UCHAR *)_nx_azure_iot_root_cert_2,
                                                   (USHORT)_nx_azure_iot_root_cert_size_2,
                                                   NX_NULL,
                                                   0,
                                                   NULL,
                                                   0,
                                                   NX_SECURE_X509_KEY_TYPE_NONE);
    if (status != NX_SECURE_X509_SUCCESS) {
        printf("Failed to initialize ROOT CA certificate!: error code = 0x%08x\r\n", status);
        nx_azure_iot_delete(&nx_azure_iot);
        return;
    }

    status = nx_secure_x509_certificate_initialize(&root_ca_cert_3,
                                                   (UCHAR *)_nx_azure_iot_root_cert_3,
                                                   (USHORT)_nx_azure_iot_root_cert_size_3,
                                                   NX_NULL,
                                                   0,
                                                   NULL,
                                                   0,
                                                   NX_SECURE_X509_KEY_TYPE_NONE);
    if (status != NX_SECURE_X509_SUCCESS) {
        printf("Failed to initialize ROOT CA certificate!: error code = 0x%08x\r\n", status);
        nx_azure_iot_delete(&nx_azure_iot);
        return;
    }

    status = initialize_iothub(&iothub_client);
    if (status != NX_AZURE_IOT_SUCCESS) {
        printf("Failed to initialize iothub client: error code = 0x%08x\r\n", status);
        sample_connection_status = NX_AZURE_IOT_NOT_INITIALIZED;
    }

    sample_connection_status = nx_azure_iot_hub_client_connect(&iothub_client, NX_TRUE, NX_WAIT_FOREVER);
    if (sample_connection_status != NX_AZURE_IOT_SUCCESS) {
        printf("Failed on nx_azure_iot_hub_client_connect!\r\n");
    }

    status = tx_thread_create(&sample_telemetry_thread,
                              "Sample Telemetry Thread",
                              telemetry_thread_entry,
                              0,
                              (UCHAR *)sample_telemetry_thread_stack,
                              STACK_SIZE_SAMPLE,
                              THREAD_PRIORITY_SAMPLE,
                              THREAD_PRIORITY_SAMPLE,
                              1,
                              TX_AUTO_START);
    if (status != TX_SUCCESS) {
        printf("Failed to create telemetry sample thread!: error code = 0x%08x\r\n", status);
    }

    status = tx_thread_create(&sample_c2d_thread,
                              "Sample C2D Thread",
                              cloud_to_device_thread_entry,
                              0,
                              (UCHAR *)sample_c2d_thread_stack,
                              STACK_SIZE_SAMPLE,
                              THREAD_PRIORITY_SAMPLE,
                              THREAD_PRIORITY_SAMPLE,
                              1,
                              TX_AUTO_START);
    if (status != TX_SUCCESS) {
        printf("Failed to create c2d sample thread!: error code = 0x%08x\r\n", status);
    }

    while (NX_TRUE) {
        sample_connection_monitor(ip_ptr, &iothub_client, sample_connection_status, initialize_iothub);

        tx_thread_sleep(NX_IP_PERIODIC_RATE);
    }

    nx_azure_iot_hub_client_deinitialize(&iothub_client);
    nx_azure_iot_delete(&nx_azure_iot);
}

static void telemetry_thread_entry(ULONG parameter)
{
    NX_PARAMETER_NOT_USED(parameter);

    UINT i = 0;
    while (i < 6) {
        if (sample_connection_status != NX_AZURE_IOT_SUCCESS) {
            tx_thread_sleep(NX_IP_PERIODIC_RATE);
            continue;
        }

        NX_PACKET *packet_ptr = NULL;
        UINT status = nx_azure_iot_hub_client_telemetry_message_create(&iothub_client, &packet_ptr, NX_WAIT_FOREVER);
        if (status != NX_AZURE_IOT_SUCCESS) {
            printf("Telemetry message create failed!: error code = 0x%08x\r\n", status);
            continue;
        }

        for (size_t index = 0; index < MAX_PROPERTY_COUNT; index++) {
            status = nx_azure_iot_hub_client_telemetry_property_add(packet_ptr,
                                                                    (UCHAR *)sample_properties[index][0],
                                                                    (USHORT)strlen(sample_properties[index][0]),
                                                                    (UCHAR *)sample_properties[index][1],
                                                                    (USHORT)strlen(sample_properties[index][1]),
                                                                    NX_WAIT_FOREVER);
            if (status != NX_AZURE_IOT_SUCCESS) {
                printf("Telemetry property add failed!: error code = 0x%08x\r\n", status);
                break;
            }
        }

        if (status != NX_AZURE_IOT_SUCCESS) {
            nx_azure_iot_hub_client_telemetry_message_delete(packet_ptr);
            continue;
        }

        CHAR buffer[30];
        UINT buffer_length = (UINT)snprintf(buffer, sizeof(buffer), "{\"Message ID\":%u}", i++);
        status = nx_azure_iot_hub_client_telemetry_send(
            &iothub_client, packet_ptr, (UCHAR *)buffer, buffer_length, NX_WAIT_FOREVER);
        if (status != NX_AZURE_IOT_SUCCESS) {
            printf("Telemetry message send failed!: error code = 0x%08x\r\n", status);
            nx_azure_iot_hub_client_telemetry_message_delete(packet_ptr);
            continue;
        }

        printf("Telemetry message sent: %s.\r\n", buffer);

        tx_thread_sleep(5 * NX_IP_PERIODIC_RATE);
    }
}

static void cloud_to_device_thread_entry(ULONG parameter)
{
    NX_PARAMETER_NOT_USED(parameter);

    while (NX_TRUE) {
        if (sample_connection_status != NX_AZURE_IOT_SUCCESS) {
            tx_thread_sleep(NX_IP_PERIODIC_RATE);
            continue;
        }

        NX_PACKET *packet_ptr = NULL;
        UINT status = nx_azure_iot_hub_client_cloud_message_receive(&iothub_client, &packet_ptr, NX_WAIT_FOREVER);
        if (NX_AZURE_IOT_SUCCESS != status) {
            printf("C2D receive failed!: error code = 0x%08x\r\n", status);
            continue;
        }

        const UCHAR *property_buf = NULL;
        USHORT property_buf_size = 0;
        status = nx_azure_iot_hub_client_cloud_message_property_get(&iothub_client,
                                                                    packet_ptr,
                                                                    (UCHAR *)sample_properties[0][0],
                                                                    (USHORT)strlen(sample_properties[0][0]),
                                                                    &property_buf,
                                                                    &property_buf_size);

        if (NX_AZURE_IOT_SUCCESS == status) {
            printf("Receive property: %s = %.*s\r\n", sample_properties[0][0], (INT)property_buf_size, property_buf);
        }

        printf("Received message: ");
        printf_packet(packet_ptr);
        printf("\r\n");

        nx_packet_release(packet_ptr);
    }
}

static VOID printf_packet(NX_PACKET *packet_ptr)
{
    while (packet_ptr != NX_NULL) {
        printf("%.*s",
               (INT)(packet_ptr->nx_packet_append_ptr - packet_ptr->nx_packet_prepend_ptr),
               (CHAR *)packet_ptr->nx_packet_prepend_ptr);
        packet_ptr = packet_ptr->nx_packet_next;
    }
}

static VOID on_connection_status_update(NX_AZURE_IOT_HUB_CLIENT *hub_client_ptr, UINT status)
{
    NX_PARAMETER_NOT_USED(hub_client_ptr);
    if (status) {
        printf("Disconnected from IoTHub!: error code = 0x%08x\r\n", status);
    } else {
        printf("Connected to IoTHub.\r\n");
    }

    sample_connection_status = status;
}

static UINT initialize_iothub(NX_AZURE_IOT_HUB_CLIENT *iothub_client_ptr)
{
    UINT status;
    UCHAR *iothub_hostname = (UCHAR *)HOST_NAME;
    UCHAR *iothub_device_id = (UCHAR *)DEVICE_ID;
    UINT iothub_hostname_length = sizeof(HOST_NAME) - 1;
    UINT iothub_device_id_length = sizeof(DEVICE_ID) - 1;

    printf("IoTHub Host Name: %.*s; Device ID: %.*s.\r\n",
           iothub_hostname_length,
           iothub_hostname,
           iothub_device_id_length,
           iothub_device_id);

    status = nx_azure_iot_hub_client_initialize(iothub_client_ptr,
                                                &nx_azure_iot,
                                                iothub_hostname,
                                                iothub_hostname_length,
                                                iothub_device_id,
                                                iothub_device_id_length,
                                                (UCHAR *)MODULE_ID,
                                                sizeof(MODULE_ID) - 1,
                                                _nx_azure_iot_tls_supported_crypto,
                                                _nx_azure_iot_tls_supported_crypto_size,
                                                _nx_azure_iot_tls_ciphersuite_map,
                                                _nx_azure_iot_tls_ciphersuite_map_size,
                                                nx_azure_iot_tls_metadata_buffer,
                                                sizeof(nx_azure_iot_tls_metadata_buffer),
                                                &root_ca_cert);
    if (status != NX_AZURE_IOT_SUCCESS) {
        printf("Failed on nx_azure_iot_hub_client_initialize!: error code = 0x%08x\r\n", status);
        return (status);
    }

    status = nx_azure_iot_hub_client_trusted_cert_add(iothub_client_ptr, &root_ca_cert_2);
    if (status != NX_AZURE_IOT_SUCCESS) {
        printf("Failed on nx_azure_iot_hub_client_trusted_cert_add!: error code = 0x%08x\r\n", status);
        goto end;
    }

    status = nx_azure_iot_hub_client_trusted_cert_add(iothub_client_ptr, &root_ca_cert_3);
    if (status != NX_AZURE_IOT_SUCCESS) {
        printf("Failed on nx_azure_iot_hub_client_trusted_cert_add!: error code = 0x%08x\r\n", status);
        goto end;
    }

    status = nx_azure_iot_hub_client_symmetric_key_set(
        iothub_client_ptr, (UCHAR *)DEVICE_SYMMETRIC_KEY, sizeof(DEVICE_SYMMETRIC_KEY) - 1);
    if (status != NX_AZURE_IOT_SUCCESS) {
        printf("Failed on nx_azure_iot_hub_client_symmetric_key_set!\r\n");
        goto end;
    }

    status = nx_azure_iot_hub_client_connection_status_callback_set(iothub_client_ptr, on_connection_status_update);
    if (status != NX_AZURE_IOT_SUCCESS) {
        printf("Failed to set connection status callback!\r\n");
        goto end;
    }

    status = nx_azure_iot_hub_client_cloud_message_enable(iothub_client_ptr);
    if (status != NX_AZURE_IOT_SUCCESS) {
        printf("C2D receive enable failed!: error code = 0x%08x\r\n", status);
        goto end;
    }

end:
    if (status == NX_AZURE_IOT_SUCCESS) {
        printf("Waiting to connect to IoTHub...\r\n");
    } else {
        nx_azure_iot_hub_client_deinitialize(iothub_client_ptr);
    }

    return (status);
}

static void on_azure_iot_log(az_log_classification classification, UCHAR *msg, UINT msg_len)
{
    if (classification == AZ_LOG_IOT_AZURERTOS) {
        printf("%.*s", msg_len, (CHAR *)msg);
    }
}
