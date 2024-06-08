/*
 * Copyright (c) 2014-2018, 2020, Pelion and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2022-2023, Arm Limited and affiliates
 * SPDX-License-Identifier: Apache-2.0
 *
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
#ifndef COMMON_FUNCTIONS_H_
#define COMMON_FUNCTIONS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
 * Copy a bitstring
 *
 * Copy a bitstring of specified length. The bit string is in big-endian
 * (network) bit order. Bits beyond the bitlength at the destination are not
 * modified.
 *
 * For example, copying 4 bits sets the first 4 bits of dst[0] from src[0],
 * the lower 4 bits of dst[0] are unmodified.
 *
 * \param dst destination pointer
 * \param src source pointer
 * \param bits number of bits to copy
 *
 * \return the value of dst
 */
uint8_t *bitcopy(uint8_t *dst, const uint8_t *src, uint_fast8_t bits);

#ifdef __cplusplus
}
#endif
#endif /*__COMMON_FUNCTIONS_H_*/
