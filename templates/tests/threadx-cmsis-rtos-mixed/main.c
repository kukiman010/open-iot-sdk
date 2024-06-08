/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cmsis_os2.h"
#include "tx_api.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

static osMutexId_t gMutex;
static TX_MUTEX *tx_mutex;

static void thread_cmsis_rtos(void *argument)
{
    (void)argument;
    for (int i = 0; i < 5; ++i) {
        if (osMutexAcquire(gMutex, 10) != osOK) {
            printf("Failed to acquire mutex in CMSIS-RTOS thread!\r\n");
            return;
        }

        printf("Message from CMSIS-RTOS thread\r\n");

        if (osMutexRelease(gMutex) != osOK) {
            printf("Failed to release mutex in CMSIS-RTOS thread!\r\n");
        }
        osDelay(500);
    }
}

static VOID thread_native_threadx(ULONG argument)
{
    (void)argument;
    for (int i = 0; i < 5; ++i) {
        if (tx_mutex_get(tx_mutex, 10) != TX_SUCCESS) {
            printf("Failed to get mutex in native ThreadX thread!\r\n");
            return;
        }

        printf("Message from native ThreadX thread\r\n");

        if (tx_mutex_put(tx_mutex) != TX_SUCCESS) {
            printf("Failed to put mutex in native ThreadX thread!\r\n");
        }
        tx_thread_sleep(500);
    }
}

int main()
{
    printf("Inside main()\r\n");

    osStatus_t ret = osKernelInitialize();
    if (ret != osOK) {
        printf("osKernelInitialize failed: %d\r\n", ret);
        return EXIT_FAILURE;
    }

    static const osMutexAttr_t mutex_attr = {
        "myThreadMutex", // human readable mutex name
        0U,              // attr_bits
        NULL,            // memory for control block (if NULL use dynamic allocation)
        0U               // size for control block (if 0 use dynamic allocation)
    };
    gMutex = osMutexNew(&mutex_attr);
    if (gMutex == NULL) {
        printf("Failed to create mutex\r\n");
        return EXIT_FAILURE;
    }
    tx_mutex = (TX_MUTEX *)gMutex;

    static const osThreadAttr_t thread_attr = {
        .stack_size = 1024 // Allocate our threads with enough stack for printf
    };
    osThreadId_t tid_cmsis_rtos = osThreadNew(thread_cmsis_rtos, NULL, &thread_attr);
    if (tid_cmsis_rtos != NULL) {
        printf("CMSIS-RTOS thread: ID = 0x%" PRIXPTR "\r\n", (uintptr_t)tid_cmsis_rtos);
    } else {
        printf("Failed to create CMSIS-RTOS thread\r\n");
        return EXIT_FAILURE;
    }

    TX_THREAD tid_native_threadx;
    // In ThreadX, 0 means highest priority, so this thread will run first.
    UINT threadx_priority = 0;
    static ULONG thread_stack[1024 / sizeof(ULONG)];
    UINT tx_ret = tx_thread_create(&tid_native_threadx,
                                   "Native ThreadX thread",
                                   thread_native_threadx,
                                   0,
                                   thread_stack,
                                   sizeof(thread_stack),
                                   threadx_priority,
                                   threadx_priority,
                                   TX_NO_TIME_SLICE,
                                   TX_AUTO_START);
    if (tx_ret == TX_SUCCESS) {
        printf("Native ThreadX thread: ID = 0x%" PRIXPTR "\r\n", (uintptr_t)&tid_native_threadx);
    } else {
        printf("Failed to create native ThreadX thread\r\n");
    }

    osKernelState_t state = osKernelGetState();
    if (state == osKernelReady) {
        printf("Starting kernel\r\n");
        ret = osKernelStart();
        if (ret != osOK) {
            printf("Failed to start kernel: %d\r\n", ret);
            return EXIT_FAILURE;
        }
    } else {
        printf("Kernel not ready: %d\r\n", state);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
