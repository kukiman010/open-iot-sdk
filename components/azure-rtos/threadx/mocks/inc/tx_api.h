/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TX_API_H
#define TX_API_H

#include "fff.h"

#ifdef __cplusplus
extern "C" {
#endif

/* API return values.  */
#define TX_SUCCESS ((UINT)0x00)

/* API input parameters and general constants.  */

#define TX_NO_WAIT        ((ULONG)0)
#define TX_WAIT_FOREVER   ((ULONG)0xFFFFFFFFUL)
#define TX_AND            ((UINT)2)
#define TX_AND_CLEAR      ((UINT)3)
#define TX_OR             ((UINT)0)
#define TX_OR_CLEAR       ((UINT)1)
#define TX_1_ULONG        ((UINT)1)
#define TX_2_ULONG        ((UINT)2)
#define TX_4_ULONG        ((UINT)4)
#define TX_8_ULONG        ((UINT)8)
#define TX_16_ULONG       ((UINT)16)
#define TX_NO_TIME_SLICE  ((ULONG)0)
#define TX_AUTO_START     ((UINT)1)
#define TX_DONT_START     ((UINT)0)
#define TX_AUTO_ACTIVATE  ((UINT)1)
#define TX_NO_ACTIVATE    ((UINT)0)
#define TX_TRUE           ((UINT)1)
#define TX_FALSE          ((UINT)0)
#define TX_NULL           ((void *)0)
#define TX_INHERIT        ((UINT)1)
#define TX_NO_INHERIT     ((UINT)0)
#define TX_THREAD_ENTRY   ((UINT)0)
#define TX_THREAD_EXIT    ((UINT)1)
#define TX_NO_SUSPENSIONS ((UINT)0)
#define TX_NO_MESSAGES    ((UINT)0)
#define TX_EMPTY          ((ULONG)0)
#define TX_CLEAR_ID       ((ULONG)0)
#define TX_STACK_FILL     ((ULONG)0xEFEFEFEFUL)

#define VOID void
typedef char CHAR;
typedef unsigned char UCHAR;
typedef int INT;
typedef unsigned int UINT;
typedef long LONG;
typedef unsigned long ULONG;
typedef short SHORT;
typedef unsigned short USHORT;

typedef struct TX_THREAD_STRUCT {
    unsigned int member;
} TX_THREAD;

/* Define the semaphore structure utilized by the application.  */

typedef struct TX_SEMAPHORE_STRUCT {
    /* Define the semaphore ID used for error checking.  */
    ULONG tx_semaphore_id;

    /* Define the semaphore's name.  */
    CHAR *tx_semaphore_name;

    /* Define the actual semaphore count.  A zero means that no semaphore
       instance is available.  */
    ULONG tx_semaphore_count;
} TX_SEMAPHORE;

typedef struct TX_MUTEX_STRUCT {
    /* Define the mutex ID used for error checking.  */
    ULONG tx_mutex_id;
} TX_MUTEX;

DECLARE_FAKE_VALUE_FUNC(UINT, tx_thread_preemption_change, TX_THREAD *, UINT, UINT *);
DECLARE_FAKE_VALUE_FUNC(TX_THREAD *, tx_thread_identify);
DECLARE_FAKE_VALUE_FUNC(UINT, _tx_thread_interrupt_disable);
DECLARE_FAKE_VOID_FUNC(_tx_thread_interrupt_restore, UINT);

DECLARE_FAKE_VALUE_FUNC(UINT, tx_mutex_create, TX_MUTEX *, CHAR *, UINT);
DECLARE_FAKE_VALUE_FUNC(UINT, tx_mutex_delete, TX_MUTEX *);
DECLARE_FAKE_VALUE_FUNC(UINT, tx_mutex_get, TX_MUTEX *, ULONG);
DECLARE_FAKE_VALUE_FUNC(UINT, tx_mutex_put, TX_MUTEX *);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TX_API_H
