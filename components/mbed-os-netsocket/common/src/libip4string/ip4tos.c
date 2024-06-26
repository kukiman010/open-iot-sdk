/*
 * Copyright (c) 2014-2018, Pelion and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2022, Arm Limited and affiliates
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
#include "common_functions.h"
#include "ip4string.h"

#include <stdio.h>
#include <string.h>

static uint8_t ipv4_tostring(char *string, uint8_t byte)
{
    char *baseString = string;
    uint8_t digits_written = 0;

    // Write the digits to the buffer from the least significant to the most
    //   This is the incorrect order but we will swap later
    do {
        *string++ = '0' + byte % 10;
        byte /= 10;
        digits_written++;
    } while (byte);

    // We put the final \0, then go back one step on the last digit for the swap
    *string-- = '\0';

    // We now swap the digits
    while (baseString < string) {
        char tmp = *string;
        *string-- = *baseString;
        *baseString++ = tmp;
    }

    return digits_written;
}

/**
 * Print binary IPv4 address to a string.
 * String must contain enough room for full address, 16 bytes exact.
 * \param addr IPv4 address.
 * \p buffer to write string to.
 */
uint_fast8_t ip4tos(const void *ip4addr, char *p)
{
    uint_fast8_t outputPos = 0;
    const uint8_t *byteArray = ip4addr;

    for (uint_fast8_t component = 0; component < 4; ++component) {
        uint8_t digits_written;
        // Convert the byte to string
        digits_written = ipv4_tostring(&p[outputPos], byteArray[component]);

        // Move outputPos to the end of the string
        outputPos += digits_written;

        // Append a dot if this is not the last digit
        if (component < 3) {
            p[outputPos++] = '.';
        }
    }

    // Return length of generated string, excluding the terminating null character
    return outputPos;
}
