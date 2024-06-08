/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "fff.h"
#include "iot_socket.h"

DEFINE_FAKE_VALUE_FUNC(int32_t, iotSocketCreate, int32_t, int32_t, int32_t);
DEFINE_FAKE_VALUE_FUNC(int32_t, iotSocketBind, int32_t, const uint8_t *, uint32_t, uint16_t);
DEFINE_FAKE_VALUE_FUNC(int32_t, iotSocketListen, int32_t, int32_t);
DEFINE_FAKE_VALUE_FUNC(int32_t, iotSocketAccept, int32_t, uint8_t *, uint32_t *, uint16_t *);
DEFINE_FAKE_VALUE_FUNC(int32_t, iotSocketConnect, int32_t, const uint8_t *, uint32_t, uint16_t);
DEFINE_FAKE_VALUE_FUNC(int32_t, iotSocketRecv, int32_t, void *, uint32_t);
DEFINE_FAKE_VALUE_FUNC(int32_t, iotSocketRecvFrom, int32_t, void *, uint32_t, uint8_t *, uint32_t *, uint16_t *);
DEFINE_FAKE_VALUE_FUNC(int32_t, iotSocketSend, int32_t, const void *, uint32_t);
DEFINE_FAKE_VALUE_FUNC(int32_t, iotSocketSendTo, int32_t, const void *, uint32_t, const uint8_t *, uint32_t, uint16_t);
DEFINE_FAKE_VALUE_FUNC(int32_t, iotSocketGetSockName, int32_t, uint8_t *, uint32_t *, uint16_t *);
DEFINE_FAKE_VALUE_FUNC(int32_t, iotSocketGetPeerName, int32_t, uint8_t *, uint32_t *, uint16_t *);
DEFINE_FAKE_VALUE_FUNC(int32_t, iotSocketGetOpt, int32_t, int32_t, void *, uint32_t *);
DEFINE_FAKE_VALUE_FUNC(int32_t, iotSocketSetOpt, int32_t, int32_t, const void *, uint32_t);
DEFINE_FAKE_VALUE_FUNC(int32_t, iotSocketClose, int32_t);
DEFINE_FAKE_VALUE_FUNC(int32_t, iotSocketGetHostByName, const char *, int32_t, uint8_t *, uint32_t *);
