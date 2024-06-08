/* Socket
 * Copyright (c) 2015-2022 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
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

#include "netsocket/InternetDatagramSocket.h"
#include "IotSocketToMbed.h"
#include "cmsis_os2.h"

nsapi_error_t InternetDatagramSocket::connect(const SocketAddress &address)
{
    _connected_udp_address = address;
    return NSAPI_ERROR_OK;
}

nsapi_size_or_error_t InternetDatagramSocket::sendto(const SocketAddress &address, const void *data, nsapi_size_t size)
{
    if (!verify_socket_type(address.get_ip_version())) {
        return NSAPI_ERROR_PARAMETER;
    }

    int32_t rc = iotSocketSendTo(_socket_id,
                                 data,
                                 static_cast<uint32_t>(size),
                                 reinterpret_cast<const uint8_t *>(address.get_ip_address()),
                                 get_iot_address_size(),
                                 address.get_port());

    return iotSocketErrorToMbed(rc);
}

nsapi_size_or_error_t InternetDatagramSocket::send(const void *data, nsapi_size_t size)
{
    if (!_connected_udp_address) {
        return NSAPI_ERROR_NO_ADDRESS;
    }
    return sendto(_connected_udp_address, data, size);
}

nsapi_size_or_error_t InternetDatagramSocket::recvfrom(SocketAddress *address, void *buffer, nsapi_size_t size)
{
    if (address) {
        if (!verify_socket_type(address->get_ip_version())) {
            return NSAPI_ERROR_PARAMETER;
        }
    }

    if (_socket_id == IOT_SOCKET_ID_INVALID) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    uint8_t iot_address[NSAPI_IPv6_BYTES];
    uint32_t address_size = get_iot_address_size();
    uint16_t port;

    int32_t rc = iotSocketRecvFrom(_socket_id, buffer, size, iot_address, &address_size, &port);

    if (rc >= 0) {
        bool filtered = false;
        // Filter incomming packets using connected peer address
        if (_connected_udp_address) {
            SocketAddress recv_address;
            recv_address.set_ip_bytes(iot_address, get_ip_nsapi_version());
            recv_address.set_port(port);

            if (_connected_udp_address != recv_address) {
                rc = 0;
                filtered = true;
            }
        }

        if (address && !filtered) {
            address->set_ip_bytes(iot_address, get_ip_nsapi_version());
            address->set_port(port);
        }
    }

    return iotSocketErrorToMbed(rc);
}

nsapi_size_or_error_t InternetDatagramSocket::recv(void *buffer, nsapi_size_t size)
{
    return recvfrom(NULL, buffer, size);
}

Socket *InternetDatagramSocket::accept(nsapi_error_t *error)
{
    if (error) {
        *error = NSAPI_ERROR_UNSUPPORTED;
    }
    return NULL;
}

nsapi_error_t InternetDatagramSocket::listen(int)
{
    return NSAPI_ERROR_UNSUPPORTED;
}
