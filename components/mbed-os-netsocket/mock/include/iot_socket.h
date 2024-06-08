/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IOT_SOCKET_MOCK_H
#define IOT_SOCKET_MOCK_H

#include "fff.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**** Address Family definitions ****/
#define IOT_SOCKET_AF_INET  1 ///< IPv4
#define IOT_SOCKET_AF_INET6 2 ///< IPv6

/**** Socket Type definitions ****/
#define IOT_SOCKET_SOCK_STREAM 1 ///< Stream socket
#define IOT_SOCKET_SOCK_DGRAM  2 ///< Datagram socket

/**** Socket Protocol definitions ****/
#define IOT_SOCKET_IPPROTO_TCP 1 ///< TCP
#define IOT_SOCKET_IPPROTO_UDP 2 ///< UDP

/**** Socket Option definitions ****/
#define IOT_SOCKET_IO_FIONBIO \
    1 ///< Non-blocking I/O (Set only, default = 0); opt_val = &nbio, opt_len = sizeof(nbio), nbio (integer):
      ///< 0=blocking, non-blocking otherwise
#define IOT_SOCKET_SO_RCVTIMEO 2 ///< Receive timeout in ms (default = 0); opt_val = &timeout, opt_len = sizeof(timeout)
#define IOT_SOCKET_SO_SNDTIMEO 3 ///< Send timeout in ms (default = 0); opt_val = &timeout, opt_len = sizeof(timeout)
#define IOT_SOCKET_SO_KEEPALIVE \
    4 ///< Keep-alive messages (default = 0); opt_val = &keepalive, opt_len = sizeof(keepalive), keepalive (integer):
      ///< 0=disabled, enabled otherwise
#define IOT_SOCKET_SO_TYPE \
    5 ///< Socket Type (Get only); opt_val = &socket_type, opt_len = sizeof(socket_type), socket_type (integer):
      ///< IOT_SOCKET_SOCK_xxx

/**** Socket Return Codes ****/
#define IOT_SOCKET_ERROR         (-1)  ///< Unspecified error
#define IOT_SOCKET_ESOCK         (-2)  ///< Invalid socket
#define IOT_SOCKET_EINVAL        (-3)  ///< Invalid argument
#define IOT_SOCKET_ENOTSUP       (-4)  ///< Operation not supported
#define IOT_SOCKET_ENOMEM        (-5)  ///< Not enough memory
#define IOT_SOCKET_EAGAIN        (-6)  ///< Operation would block or timed out
#define IOT_SOCKET_EINPROGRESS   (-7)  ///< Operation in progress
#define IOT_SOCKET_ETIMEDOUT     (-8)  ///< Operation timed out
#define IOT_SOCKET_EISCONN       (-9)  ///< Socket is connected
#define IOT_SOCKET_ENOTCONN      (-10) ///< Socket is not connected
#define IOT_SOCKET_ECONNREFUSED  (-11) ///< Connection rejected by the peer
#define IOT_SOCKET_ECONNRESET    (-12) ///< Connection reset by the peer
#define IOT_SOCKET_ECONNABORTED  (-13) ///< Connection aborted locally
#define IOT_SOCKET_EALREADY      (-14) ///< Connection already in progress
#define IOT_SOCKET_EADDRINUSE    (-15) ///< Address in use
#define IOT_SOCKET_EHOSTNOTFOUND (-16) ///< Host not found

DECLARE_FAKE_VALUE_FUNC(int32_t, iotSocketCreate, int32_t, int32_t, int32_t);
DECLARE_FAKE_VALUE_FUNC(int32_t, iotSocketBind, int32_t, const uint8_t *, uint32_t, uint16_t);
DECLARE_FAKE_VALUE_FUNC(int32_t, iotSocketListen, int32_t, int32_t);
DECLARE_FAKE_VALUE_FUNC(int32_t, iotSocketAccept, int32_t, uint8_t *, uint32_t *, uint16_t *);
DECLARE_FAKE_VALUE_FUNC(int32_t, iotSocketConnect, int32_t, const uint8_t *, uint32_t, uint16_t);
DECLARE_FAKE_VALUE_FUNC(int32_t, iotSocketRecv, int32_t, void *, uint32_t);
DECLARE_FAKE_VALUE_FUNC(int32_t, iotSocketRecvFrom, int32_t, void *, uint32_t, uint8_t *, uint32_t *, uint16_t *);
DECLARE_FAKE_VALUE_FUNC(int32_t, iotSocketSend, int32_t, const void *, uint32_t);
DECLARE_FAKE_VALUE_FUNC(int32_t, iotSocketSendTo, int32_t, const void *, uint32_t, const uint8_t *, uint32_t, uint16_t);
DECLARE_FAKE_VALUE_FUNC(int32_t, iotSocketGetSockName, int32_t, uint8_t *, uint32_t *, uint16_t *);
DECLARE_FAKE_VALUE_FUNC(int32_t, iotSocketGetPeerName, int32_t, uint8_t *, uint32_t *, uint16_t *);
DECLARE_FAKE_VALUE_FUNC(int32_t, iotSocketGetOpt, int32_t, int32_t, void *, uint32_t *);
DECLARE_FAKE_VALUE_FUNC(int32_t, iotSocketSetOpt, int32_t, int32_t, const void *, uint32_t);
DECLARE_FAKE_VALUE_FUNC(int32_t, iotSocketClose, int32_t);
DECLARE_FAKE_VALUE_FUNC(int32_t, iotSocketGetHostByName, const char *, int32_t, uint8_t *, uint32_t *);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // IOT_SOCKET_MOCK_H
