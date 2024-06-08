/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cmsis_os2.h"

namespace pw::this_thread {
void yield()
{
    osThreadYield();
}

} // namespace pw::this_thread
