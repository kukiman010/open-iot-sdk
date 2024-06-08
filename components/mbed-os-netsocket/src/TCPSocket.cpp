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

#include "netsocket/TCPSocket.h"

#include "IotSocketToMbed.h"

#include <assert.h>
#include <cstdio>
#include <new>

TCPSocket::TCPSocket()
{
    _iot_socket_ipproto = IOT_SOCKET_IPPROTO_TCP;
    _iot_socket_sock_type = IOT_SOCKET_SOCK_STREAM;
}

TCPSocket::TCPSocket(int32_t id, uint8_t iot_socket_type) : TCPSocket()
{
    _socket_id = id;
    _iot_socket_af_type = iot_socket_type;
    _factory_allocated = true;
}
nsapi_error_t TCPSocket::connect(const SocketAddress &address)
{
    if (!_socket_opened) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    if (!verify_socket_type(address.get_ip_version())) {
        return NSAPI_ERROR_PARAMETER;
    }

    const uint8_t *ip = static_cast<const uint8_t *>(address.get_ip_bytes());
    int32_t rc = iotSocketConnect(_socket_id, ip, get_iot_address_size(), address.get_port());

    return iotSocketErrorToMbed(rc);
}

nsapi_size_or_error_t TCPSocket::send(const void *data, nsapi_size_t size)
{
    const uint8_t *data_ptr = static_cast<const uint8_t *>(data);
    nsapi_size_or_error_t ret;
    nsapi_size_t written = 0;

    if (_socket_id == IOT_SOCKET_ID_INVALID) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    // Unlike recv, we should write the whole thing if blocking. POSIX only
    // allows partial as a side-effect of signal handling; it normally tries to
    // write everything if blocking. Without signals we can always write all.
    do {
        int32_t rc = iotSocketSend(_socket_id, data_ptr + written, size - written);
        ret = iotSocketErrorToMbed(rc);
        if (ret >= 0) {
            written += ret;
            if (written >= size) {
                break;
            }
        } else {
            break;
        }
    } while (_timeout == 0);

    if (ret < 0) {
        return ret;
    } else {
        return written;
    }
}

nsapi_size_or_error_t TCPSocket::sendto(const SocketAddress &address, const void *data, nsapi_size_t size)
{
    (void)address;
    return send(data, size);
}

nsapi_size_or_error_t TCPSocket::recv(void *data, nsapi_size_t size)
{
    if (_socket_id == IOT_SOCKET_ID_INVALID) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    int32_t rc = iotSocketRecv(_socket_id, data, size);

    return iotSocketErrorToMbed(rc);
}

nsapi_size_or_error_t TCPSocket::recvfrom(SocketAddress *address, void *data, nsapi_size_t size)
{
    if (_socket_id == IOT_SOCKET_ID_INVALID) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    int32_t rc;

    if (address) {
        uint8_t iot_address[NSAPI_IPv6_BYTES];
        uint32_t address_size = get_iot_address_size();
        uint16_t port;

        rc = iotSocketRecvFrom(_socket_id, data, size, iot_address, &address_size, &port);

        address->set_ip_bytes(iot_address, get_ip_nsapi_version());
        address->set_port(port);
    } else {
        rc = iotSocketRecvFrom(_socket_id, data, size, NULL, NULL, NULL);
    }

    return iotSocketErrorToMbed(rc);
}

nsapi_error_t TCPSocket::listen(int backlog)
{
    if (_socket_id == IOT_SOCKET_ID_INVALID) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    int32_t rc = iotSocketListen(_socket_id, backlog);

    return iotSocketErrorToMbed(rc);
}

TCPSocket *TCPSocket::accept(nsapi_error_t *error)
{
    if (_socket_id == IOT_SOCKET_ID_INVALID) {
        if (error) {
            *error = NSAPI_ERROR_NO_SOCKET;
        }
        return NULL;
    }

    TCPSocket *connection = NULL;

    uint8_t iot_address[NSAPI_IPv6_BYTES];
    uint32_t address_size = get_iot_address_size();
    uint16_t port;

    int32_t rc = iotSocketAccept(_socket_id, iot_address, &address_size, &port);

    if (rc >= 0) {
        SocketAddress address;
        address.set_ip_bytes(iot_address, get_ip_nsapi_version());
        address.set_port(port);

        connection = new (std::nothrow) TCPSocket(rc, _iot_socket_af_type);
        if (!connection) {
            if (error) {
                *error = NSAPI_ERROR_NO_MEMORY;
            }
            iotSocketClose(rc);
            return NULL;
        }
    }

    if (error) {
        *error = iotSocketErrorToMbed(rc);
    }

    return connection;
}
