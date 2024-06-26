/** \addtogroup netsocket */
/** @{*/
/* InternetDatagramSocket
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

#ifndef INTERNETDATAGRAMSOCKET_H
#define INTERNETDATAGRAMSOCKET_H

#include "cmsis_os2.h"
#include "netsocket/InternetSocket.h"
#include "nsapi_types.h"

/** InternetDatagramSocket socket implementation.
 */
class InternetDatagramSocket : public InternetSocket {
public:
    /** Send data to the specified address.
     *
     *  By default, sendto blocks until data is sent. If socket is set to
     *  nonblocking or times out, NSAPI_ERROR_WOULD_BLOCK is returned
     *  immediately.
     *
     *  @param address  The SocketAddress of the remote host.
     *  @param data     Buffer of data to send to the host.
     *  @param size     Size of the buffer in bytes.
     *  @retval         NSAPI_ERROR_NO_SOCKET in case socket was not created correctly.
     *  @retval         NSAPI_ERROR_WOULD_BLOCK in case non-blocking mode is enabled
     *                  and send cannot be performed immediately.
     *  @retval         int Other negative error codes for stack-related failures.
     *                  See \ref NetworkStack::socket_send.
     */
    nsapi_size_or_error_t sendto(const SocketAddress &address, const void *data, nsapi_size_t size) override;

    /** Receive a datagram and store the source address in address if it's not NULL.
     *
     *  By default, recvfrom blocks until a datagram is received. If socket is set to
     *  nonblocking or times out with no datagram, NSAPI_ERROR_WOULD_BLOCK
     *  is returned.
     *
     *  @note If the datagram is larger than the buffer, the excess data is silently discarded.
     *
     *  @note If socket is connected, only packets coming from connected peer address
     *  are accepted.
     *
     *  @note recvfrom() is allowed write to address and data buffers even if error occurs.
     *  @param address  Destination for the source address or NULL.
     *  @param data     Destination buffer for RAW data to be received from the host.
     *  @param size     Size of the buffer in bytes.
     *  @retval         int Number of received bytes on success.
     *  @retval         NSAPI_ERROR_NO_SOCKET in case socket was not created correctly.
     *  @retval         NSAPI_ERROR_WOULD_BLOCK in case non-blocking mode is enabled
     *                  and send cannot be performed immediately.
     *  @retval         int Other negative error codes for stack-related failures.
     *                  See \ref NetworkStack::socket_recv.
     */
    nsapi_size_or_error_t recvfrom(SocketAddress *address, void *data, nsapi_size_t size) override;

    /** Set the remote address for next send() call and filtering
     *  of incoming packets. To reset the address, zero initialized
     *  SocketAddress must be in the address parameter.
     *
     *  @param address  The SocketAddress of the remote host.
     *  @return         NSAPI_ERROR_OK on success.
     */
    nsapi_error_t connect(const SocketAddress &address) override;

    /** Send a raw data to connected remote address.
     *
     *  By default, send blocks until all data is sent. If socket is set to
     *  nonblocking or times out, a partial amount can be written.
     *  NSAPI_ERROR_WOULD_BLOCK is returned if no data was written.
     *
     *  @note The socket must be connected to a remote host before send() call.
     *
     *  @param data     Buffer of data to send to the host.
     *  @param size     Size of the buffer in bytes.
     *  @retval         int Number of sent bytes on success.
     *  @retval         NSAPI_ERROR_NO_SOCKET in case socket was not created correctly.
     *  @retval         NSAPI_ERROR_WOULD_BLOCK in case non-blocking mode is enabled
     *                  and send cannot be performed immediately.
     *  #retval         NSAPI_ERROR_NO_ADDRESS if the address was not set with connect().
     *  @retval         int Other negative error codes for stack-related failures.
     *                  See \ref NetworkStack::socket_send.
     */
    nsapi_size_or_error_t send(const void *data, nsapi_size_t size) override;

    /** Receive data from a socket.
     *
     *  This is equivalent to calling recvfrom(NULL, data, size).
     *
     *  By default, recv blocks until some data is received. If socket is set to
     *  nonblocking or times out, NSAPI_ERROR_WOULD_BLOCK can be returned to
     *  indicate no data.
     *
     *  @note recv() is allowed write to data buffer even if error occurs.
     *
     *  @param data     Pointer to buffer for data received from the host.
     *  @param size     Size of the buffer in bytes.
     *  @retval         int Number of received bytes on success.
     *  @retval         NSAPI_ERROR_NO_SOCKET in case socket was not created correctly.
     *  @retval         NSAPI_ERROR_WOULD_BLOCK in case non-blocking mode is enabled
     *                  and send cannot be performed immediately.
     *  @retval         int Other negative error codes for stack-related failures.
     *                  See \ref NetworkStack::socket_recv.
     */
    nsapi_size_or_error_t recv(void *data, nsapi_size_t size) override;

    /** Not implemented for InternetDatagramSocket.
     *
     *  @param error      Not used.
     *  @return           NSAPI_ERROR_UNSUPPORTED
     */
    Socket *accept(nsapi_error_t *error = nullptr) override;

    /** Not implemented for InternetDatagramSocket.
     *
     *  @param backlog    Not used.
     *  @return           NSAPI_ERROR_UNSUPPORTED
     */
    nsapi_error_t listen(int backlog = 1) override;
#if !defined(DOXYGEN_ONLY)

protected:
    /** Create an uninitialized socket.
     *
     *  @note Must call open to initialize the socket on a network stack.
     */
    InternetDatagramSocket() = default;

    SocketAddress _connected_udp_address;

#endif //! defined(DOXYGEN_ONLY)
};

#endif

/** @}*/
