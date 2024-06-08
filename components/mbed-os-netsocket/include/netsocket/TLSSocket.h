/** @file TLSSocket.h TLSSocket */
/*
 * Copyright (c) 2018-2022 ARM Limited
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
/** @addtogroup netsocket
 * @{
 */

// This file was copied and adapted from the mbed-os source code. the return
// values of the API should use the mbed-os error codes.

#ifndef _MBED_HTTPS_TLS_TCP_SOCKET_H_
#define _MBED_HTTPS_TLS_TCP_SOCKET_H_

#include "netsocket/TCPSocket.h"

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/error.h"
#include "mbedtls/platform.h"
#include "mbedtls/ssl.h"

#if defined(MBED_CONF_NSAPI_OFFLOAD_TLSSOCKET) && MBED_CONF_NSAPI_OFFLOAD_TLSSOCKET
#error "MBED_CONF_NSAPI_OFFLOAD_TLSSOCKET not supported"
#endif // MBED_CONF_NSAPI_OFFLOAD_TLSSOCKET

// This class requires Mbed TLS SSL/TLS client code

#include "TLSSocketWrapper.h"

/**
 * \brief TLSSocket is a wrapper around TCPSocket for interacting with TLS servers.
 *
 * TLSSocket uses the TLSSocketWrapper with internal TCP socket.
 * This is a helper for creating commonly used TLS connections over TCP.
 *
 */
class TLSSocket : public TLSSocketWrapper {
public:
    /** Create an uninitialized socket.
     *
     *  Must call open to initialize the socket on a network stack.
     */
    TLSSocket() : TLSSocketWrapper(&tcp_socket)
    {
    }

    /** Destroy the TLSSocket and closes the transport.
     */
    ~TLSSocket() override;

    /** Opens a socket.
     *
     *  Creates a network socket on the network stack of the given
     *  network interface.
     *
     *  @note TLSSocket cannot be reopened after closing. It should be destructed to
     *        clear internal TLS memory structures.
     *
     *  @param stack    Network stack as target for socket.
     *  @return         NSAPI_ERROR_OK on success. See @ref TCPSocket::open
     */
    nsapi_error_t open()
    {
        return tcp_socket.open();
    }

    using TLSSocketWrapper::connect;

private:
    TCPSocket tcp_socket;
};

#endif // _MBED_HTTPS_TLS_TCP_SOCKET_H_

/** @} */
