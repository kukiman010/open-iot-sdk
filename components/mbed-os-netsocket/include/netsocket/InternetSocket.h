
/** \addtogroup netsocket */
/** @{*/
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

// This file was copied and adapted from the mbed-os source code. the return
// values of the API should use the mbed-os error codes.

#ifndef INTERNETSOCKET_H
#define INTERNETSOCKET_H

extern "C" {
#include "iot_socket.h"
}

#include "cmsis_os2.h"
#include "netsocket/Socket.h"

#include <atomic>
#include <cstring>

#define IOT_SOCKET_ID_INVALID (-1)

/** Socket implementation that uses IP network stack.
 * Not to be directly used by applications. Cannot be directly instantiated.
 */
class InternetSocket : public Socket {
public:
    /** Create an uninitialized socket
     *
     *  Must call open to initialize the socket on a network stack.
     */
    InternetSocket() = default;

    /** Destroy the socket.
     *
     *  @note Closes socket if it's still open.
     */
    ~InternetSocket() override;

    /** Open a network socket on the network stack of the given
     *  network interface.
     *
     *  @param nsapi_socket_type Which version of IP to use, NSAPI_IPv4 or NSAPI_IPv6.
     *  @retval         NSAPI_ERROR_OK on success.
     *  @retval         NSAPI_ERROR_PARAMETER if param out of range
     */
#if DEFAULT_IP_VERSION == NSAPI_IPv4
    nsapi_error_t open(nsapi_version_t nsapi_socket_type = NSAPI_IPv4);
#else
    nsapi_error_t open(nsapi_version_t nsapi_socket_type = NSAPI_IPv6));
#endif

    /** Close any open connection, and deallocate any memory associated
     *  with the socket. Called from destructor if socket is not closed.
     *
     *  @retval         NSAPI_ERROR_OK on success.
     *  @retval         NSAPI_ERROR_NO_SOCKET if socket is not open.
     *  @retval         int negative error codes for stack-related failures.
     */
    nsapi_error_t close() override;

    /** Subscribe to an IP multicast group.
     *
     * @param address   Multicast group IP address.
     *  @return         NSAPI_ERROR_OK on success, negative error code on failure (@see InternetSocket::setsockopt).
     */
    int join_multicast_group(const SocketAddress &address);

    /** Leave an IP multicast group.
     *
     * @param address   Multicast group IP address.
     *  @return         NSAPI_ERROR_OK on success, negative error code on failure (@see InternetSocket::setsockopt).
     */
    int leave_multicast_group(const SocketAddress &address);

    /** Bind the socket to a port on which to receive data.
     *
     *  @param port     Local port to bind.
     *  @retval         NSAPI_ERROR_OK on success.
     *  @retval         NSAPI_ERROR_NO_SOCKET if socket is not open.
     *  @retval         NSAPI_ERROR_PARAMETER if socket type is different from address type.
     *  @retval         int negative error codes for stack-related failures.
     */
    nsapi_error_t bind(uint16_t port);

    /** @copydoc Socket::bind
     */
    nsapi_error_t bind(const SocketAddress &address) override;

    /** @copydoc Socket::set_blocking
     */
    void set_blocking(bool blocking) override;

    /** @copydoc Socket::set_timeout
     */
    void set_timeout(int timeout) override;

    /** @copydoc Socket::setsockopt
     */
    nsapi_error_t setsockopt(int level, int optname, const void *optval, unsigned optlen) override;

    /** @copydoc Socket::getsockopt
     */
    nsapi_error_t getsockopt(int level, int optname, void *optval, unsigned *optlen) override;

    /** @copydoc Socket::getpeername
     */
    nsapi_error_t getpeername(SocketAddress *address) override;

    int32_t get_socket_id()
    {
        return _socket_id;
    };

#if !defined(DOXYGEN_ONLY)

protected:
    /** Check if a socket is set to the type given. If socket is not set yet, it sets it to the type given.
     *
     *  @param nsapi_socket_type ip type of socket.
     *  @retval         true if the types matched or if the socket was created according to the given type.
     *  @retval         false if the types don't match or if the creation failed.
     */
    bool verify_socket_type(nsapi_version_t nsapi_socket_type = NSAPI_UNSPEC)
    {
        bool status = false;
        uint32_t iot_socket_type;

        osMutexAcquire(_socket_lock, osWaitForever);

        // this block allows us to postpone socket creation and allow the new open(nsapi_version_t)
        // to be optionally called, if called it specifies the default type,
        // otherwise the compile time selection is used
        if (nsapi_socket_type == NSAPI_UNSPEC) {
            if (_socket_opened) {
                iot_socket_type = _iot_socket_af_type;
            } else {
#if DEFAULT_IP_VERSION == NSAPI_IPv4
                iot_socket_type = IOT_SOCKET_AF_INET;
#else
                iot_socket_type = IOT_SOCKET_AF_INET6;
#endif
            }
        } else {
            iot_socket_type = (nsapi_socket_type == NSAPI_IPv4) ? IOT_SOCKET_AF_INET : IOT_SOCKET_AF_INET6;
        }

        if (_socket_id != IOT_SOCKET_ID_INVALID) { // Socket already openned
            if (iot_socket_type == _iot_socket_af_type) {
                status = true;
            }
        } else if (!_socket_opened) {
            // to maintain mbed os behaviour we require a call to open()
            status = false;
        } else {
            _iot_socket_af_type = iot_socket_type;
            int32_t rc = iotSocketCreate(_iot_socket_af_type, _iot_socket_sock_type, _iot_socket_ipproto);

            if (rc >= 0) {
                _socket_id = rc;
                status = true;
            }
        }

        osMutexRelease(_socket_lock);

        return status;
    };

    /** Returns number of bytes in address based on the IoT socket type
     */
    uint32_t get_iot_address_size()
    {
        return (_iot_socket_af_type == IOT_SOCKET_AF_INET6) ? NSAPI_IPv6_BYTES : NSAPI_IPv4_BYTES;
    }

    /** Returns nsapi_version_t of IP address basd on IoT socket type
     */
    nsapi_version_t get_ip_nsapi_version()
    {
        return (_iot_socket_af_type == IOT_SOCKET_AF_INET) ? NSAPI_IPv4 : NSAPI_IPv6;
    }

    osMutexId_t _socket_lock = osMutexNew(NULL);

    std::atomic<int32_t> _socket_id{IOT_SOCKET_ID_INVALID};
    uint32_t _timeout = osWaitForever;

    uint8_t _iot_socket_ipproto = 0;
    uint8_t _iot_socket_sock_type = 0;
    std::atomic<uint8_t> _iot_socket_af_type{0};

    bool _factory_allocated = false;
    std::atomic<bool> _socket_opened{false};

    friend class DTLSSocket; // Allow DTLSSocket::connect() to do name resolution on the _stack

#endif //! defined(DOXYGEN_ONLY)
};

#endif // INTERNETSOCKET_H

/** @}*/
