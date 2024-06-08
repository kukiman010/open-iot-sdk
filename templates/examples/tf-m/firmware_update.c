/*
 * Copyright (c) 2021-2023, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "psa/fwu_config.h"
#include "psa/update.h"
#include "tfm_ns_interface.h"

#include <inttypes.h>
#include <stdio.h>

// RTOS-specific initialization that is not declared in any header file
uint32_t tfm_ns_interface_init(void);

// PSA Firmware Update example - query the current firmware versions
void psa_thread(void *argument)
{
    (void)argument;

    printf("In psa_thread()\r\n");

    uint32_t ret = tfm_ns_interface_init();
    if (ret != 0) {
        printf("tfm_ns_interface_init() failed: %" PRIu32 "\r\n", ret);
        return;
    }

    psa_fwu_component_info_t info_s;
    psa_status_t status = psa_fwu_query(FWU_COMPONENT_ID_SECURE, &info_s);
    if (status != PSA_SUCCESS) {
        printf("Failed to query secure firmware information. Error %" PRId32 "\r\n", status);
        return;
    }

    printf("Secure firmware version: %" PRIu8 ".%" PRIu8 ".%" PRIu16 "-%" PRIu32 "\r\n",
           info_s.version.major,
           info_s.version.minor,
           info_s.version.patch,
           info_s.version.build);

    psa_fwu_component_info_t info_ns;
    status = psa_fwu_query(FWU_COMPONENT_ID_NONSECURE, &info_ns);
    if (status != PSA_SUCCESS) {
        printf("Failed to query non-secure firmware information. Error %" PRId32 "\r\n", status);
        return;
    }

    printf("Non-secure firmware version: %" PRIu8 ".%" PRIu8 ".%" PRIu16 "-%" PRIu32 "\r\n",
           info_ns.version.major,
           info_ns.version.minor,
           info_ns.version.patch,
           info_ns.version.build);
}
