/* Sockets Example
 * Copyright (c) 2016-2023 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hal/serial_api.h"
#include "iotsdk/ip_network_api.h"

#include "cmsis_os2.h"
#include "ip4string.h"
#include "ip6string.h"
#include "netsocket/TCPSocket.h"
#include "netsocket/TLSSocket.h"
#include "netsocket/nsapi_types.h"
#include "pw_log/log.h"

#include <cstdio>

extern "C" {
#include "iot_socket.h"
}

#ifdef MBED_CONF_APP_USE_TLS_SOCKET
#include "root_ca_cert.h"

#endif // MBED_CONF_APP_USE_TLS_SOCKET

mdh_serial_t *serial = NULL;
extern "C" mdh_serial_t *get_example_serial();
static osMessageQueueId_t net_msg_queue = NULL;
typedef enum net_event_t { NET_EVENT_NETWORK_UP, NET_EVENT_NETWORK_DOWN, NET_EVENT_NONE } net_event_t;

typedef struct {
    net_event_t event;
    int32_t return_code;
} net_msg_t;

static int mbedtls_platform_example_nv_seed_read(unsigned char *buf, size_t buf_len)
{
    if (buf == NULL) {
        return (-1);
    }
    memset(buf, 0xA5, buf_len);
    return 0;
}

static int mbedtls_platform_example_nv_seed_write(unsigned char *buf, size_t buf_len)
{
    (void)buf;
    (void)buf_len;

    return 0;
}
class SocketDemo {
    static constexpr size_t MAX_NUMBER_OF_ACCESS_POINTS = 10;
    static constexpr size_t MAX_MESSAGE_RECEIVED_LENGTH = 100;

    static constexpr char DEMO_HOSTNAME[] = "ifconfig.io"; // http://ifconfig.io/

#ifdef MBED_CONF_APP_USE_TLS_SOCKET
    static constexpr size_t REMOTE_PORT = 443; // tls port
#else
    static constexpr size_t REMOTE_PORT = 80; // standard HTTP port
#endif // MBED_CONF_APP_USE_TLS_SOCKET

public:
    SocketDemo()
    {
    }

    ~SocketDemo()
    {
    }

    void run()
    {
        /* opening the socket only allocates resources */
#ifdef EXAMPLE_IPV6
        nsapi_size_or_error_t result = _socket.open(NSAPI_IPv6);
#else
        nsapi_size_or_error_t result = _socket.open(NSAPI_IPv4);
#endif
        if (result < 0) {
            PW_LOG_ERROR("_socket.open() returned: %d", result);
            return;
        }

#ifdef MBED_CONF_APP_USE_TLS_SOCKET
        result = _socket.set_root_ca_cert(root_ca_cert);
        if (result != NSAPI_ERROR_OK) {
            PW_LOG_ERROR("_socket.set_root_ca_cert() returned %d", result);
            return;
        }
        _socket.set_hostname(DEMO_HOSTNAME);
#endif // MBED_CONF_APP_USE_TLS_SOCKET

        /* now we have to find where to connect */

        SocketAddress address;
        uint32_t socket_type;
        uint32_t ip_length;
        const char *hostname = DEMO_HOSTNAME;

        // call iotsockethostname resolution to get ip address.

#ifdef EXAMPLE_IPV6
        socket_type = IOT_SOCKET_AF_INET6;
        ip_length = 16;
        uint8_t ip[16];
#else
        socket_type = IOT_SOCKET_AF_INET;
        ip_length = 4;
        uint8_t ip[4];
#endif

        result = iotSocketGetHostByName(hostname, socket_type, ip, &ip_length);
        if (result != 0) {
            PW_LOG_ERROR("Error! hostname resolution returned: %d", result);
            return;
        }
#ifdef EXAMPLE_IPV6
        char buf[40]; // max IPv6 length
        ip6tos(ip, buf);
#else
        char buf[16];
        ip4tos(ip, buf);
#endif

        address.set_ip_address(buf);
        address.set_port(REMOTE_PORT);

        /* we are connected to the network but since we're using a connection oriented
         * protocol we still need to open a connection on the socket */

        PW_LOG_INFO("Opening connection to remote port %d", (int)REMOTE_PORT);
        result = _socket.connect(address);
        if (result != 0) {
            PW_LOG_ERROR("Error! _socket.connect() returned: %d", result);
            return;
        }

        /* exchange an HTTP request and response */

        if (!send_http_request()) {
            PW_LOG_ERROR("fail send http");
            return;
        }

        if (!receive_http_response()) {
            PW_LOG_ERROR("fail recv http");
            return;
        }

        PW_LOG_INFO("Demo concluded successfully");
    }

private:
    bool send_http_request()
    {
        /* loop until whole request sent */
        const char buffer[] = "GET / HTTP/1.1\r\n"
                              "Host: ifconfig.io\r\n"
                              "Connection: close\r\n"
                              "\r\n";

        nsapi_size_t bytes_to_send = strlen(buffer);
        nsapi_size_or_error_t bytes_sent = 0;

        PW_LOG_INFO("Sending message: \r\n%s", buffer);

        while (bytes_to_send) {
            bytes_sent = _socket.send(buffer + bytes_sent, bytes_to_send);
            if (bytes_sent < 0) {
                PW_LOG_ERROR("Error! _socket.send() returned: %d", bytes_sent);
                return false;
            } else {
                PW_LOG_INFO("sent %d bytes", bytes_sent);
            }

            bytes_to_send -= bytes_sent;
        }

        PW_LOG_INFO("Complete message sent");

        return true;
    }

    bool receive_http_response()
    {
        char buffer[MAX_MESSAGE_RECEIVED_LENGTH];
        int remaining_bytes = MAX_MESSAGE_RECEIVED_LENGTH;
        int received_bytes = 0;

        /* loop until there is nothing received or we've ran out of buffer space */
        nsapi_size_or_error_t result = remaining_bytes;
        while (result > 0 && remaining_bytes > 0) {
            result = _socket.recv(buffer + received_bytes, remaining_bytes);
            if (result < 0) {
                PW_LOG_ERROR("Error! _socket.recv() returned: %d", result);
                return false;
            }

            received_bytes += result;
            remaining_bytes -= result;
        }

        /* the message is likely larger but we only want the HTTP response code */

        PW_LOG_INFO("received %d bytes:\r\n%.*s\r\n", received_bytes, strstr(buffer, "\n") - buffer, buffer);

        return true;
    }

private:
#ifdef MBED_CONF_APP_USE_TLS_SOCKET
    TLSSocket _socket;
#else
    TCPSocket _socket;
#endif // MBED_CONF_APP_USE_TLS_SOCKET
};

static void network_event_callback(network_state_callback_event_t event);

void app_task(void *arg)
{
    (void)arg;

    // Configure Mbed TLS
    mbedtls_threading_set_cmsis_rtos();
    int err =
        mbedtls_platform_set_nv_seed(mbedtls_platform_example_nv_seed_read, mbedtls_platform_example_nv_seed_write);
    if (err) {
        PW_LOG_ERROR("Failed to initialize NV seed functions");
    }

    while (1) {
        // Wait for the connection to be established
        PW_LOG_INFO("Awaiting network connection");
        net_msg_t msg;
        if (osMessageQueueGet(net_msg_queue, &msg, NULL, 1000) != osOK) {
            msg.event = NET_EVENT_NONE;
        }

        if (msg.event == NET_EVENT_NETWORK_UP) {
            PW_LOG_INFO("Network connection enabled");
            PW_LOG_INFO("Starting socket demo");
            SocketDemo *example = new SocketDemo();
            example->run();
            break;
        } else if (msg.event == NET_EVENT_NETWORK_DOWN) {
            PW_LOG_INFO("Network is not enabled");
        }
        osDelay(1000);
    }
}

int main(void)
{
    serial = get_example_serial();
    mdh_serial_set_baud(serial, 115200);

    osStatus_t ret = osKernelInitialize();
    if (ret != osOK) {
        PW_LOG_ERROR("osKernelInitialize failed: %d", ret);
        return -1;
    }

    // Initialization of PW log is done after the kernel initialization because
    // it requires a lock
    pw_log_mdh_init(serial);

    net_msg_queue = osMessageQueueNew(10, sizeof(net_msg_t), NULL);

    if (!net_msg_queue) {
        PW_LOG_ERROR("Cannot create a queue, exiting");
        return -1;
    }
    const osThreadAttr_t thread1_attr = {
        .stack_size = 8192 // Create the thread stack with a size of 8192 bytes
    };
    osThreadId_t app_thread = osThreadNew(app_task, NULL, &thread1_attr);

    if (!app_thread) {
        PW_LOG_ERROR("Failed to create thread");
        return -1;
    }

    PW_LOG_INFO("Initialising network");

    ret = start_network_task(network_event_callback, 0);

    if (ret != osOK) {
        PW_LOG_ERROR("Failed to start lwip");
        return -1;
    }

    osKernelState_t state = osKernelGetState();

    if (state != osKernelReady) {
        PW_LOG_ERROR("Kernel not ready: %d", state);
        return -1;
    }

    PW_LOG_INFO("Starting kernel");

    ret = osKernelStart();

    if (ret != osOK) {
        PW_LOG_ERROR("Failed to start kernel: %d", ret);
        return -1;
    }

    return 0;
}

/** This callback is called by the ip network task. It translates from a network event code
 * to our app message queue code and sends the event to the main app task.
 *
 * @param event network up or down event.
 */
static void network_event_callback(network_state_callback_event_t event)
{
    const net_msg_t msg = {.event = (event == NETWORK_UP) ? NET_EVENT_NETWORK_UP : NET_EVENT_NETWORK_DOWN};
    if (osMessageQueuePut(net_msg_queue, &msg, 0, 0) != osOK) {
        PW_LOG_ERROR("Failed to send message to net_msg_queue");
    }
}
