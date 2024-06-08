/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_LOG_BACKEND_CONFIG_H_
#define PW_LOG_BACKEND_CONFIG_H_

#include "pw_iot_sdk_config/config.h"

#ifndef PW_LOG_MDH_SHOW_FILENAME
#define PW_LOG_MDH_SHOW_FILENAME PW_IOTSDK_LOG_SHOW_FILENAME
#endif /* ! PW_LOG_MDH_SHOW_FILENAME */

#ifndef PW_LOG_MDH_LINE_LENGTH
#define PW_LOG_MDH_LINE_LENGTH PW_IOTSDK_LOG_LINE_LENGTH
#endif /* ! PW_LOG_MDH_LINE_LENGTH */

#ifndef PW_LOG_MDH_MODULE_LENGTH
#define PW_LOG_MDH_MODULE_LENGTH PW_IOTSDK_LOG_MODULE_LENGTH
#endif /* ! PW_LOG_MDH_MODULE_LENGTH */

#ifndef PW_LOG_MDH_TIMESTAMP
#define PW_LOG_MDH_TIMESTAMP PW_IOTSDK_LOG_TIMESTAMP
#endif /* ! PW_LOG_MDH_TIMESTAMP */

#endif /* PW_LOG_BACKEND_CONFIG_H_ */
