/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MBED_ERROR_H
#define MBED_ERROR_H

typedef int mbed_error_status_t;

// Module type/id definitions
/** mbed_module_type_t definition
 * @note
 * This enumeration defines the module types. The value of these enum values will be encoded into mbed_error_status_t
 MODULE field.\n\n
 *      See mbed_error_status_t description for more info.\n
 *          MBED_MODULE_UNKNOWN - This module type can be used if caller of the mbed_error/mbed_warning doesn't know who
 is the actual originator of the error.\n
 *                       Other module values can be used to provide more info on who/where the error originated
 from.\n\n
 *                       For example, if I2C driver is the component originating the error you can use
 MBED_MODULE_DRIVER_I2C to provide more info.\n
 *                       Its used in call to MBED_MAKE_ERROR/MBED_MAKE_SYSTEM_ERROR/MBED_MAKE_CUSTOM_ERROR macros.\n
 *
 * @code
 *      Example: mbed_error_status_t i2c_driver_error = MBED_MAKE_ERROR( MBED_MODULE_DRIVER_I2C,
 MBED_ERROR_CONFIG_UNSUPPORTED );
 * @endcode
 *
 * @note
 *  \n Below are the module code mappings:\n
    \verbatim
    MBED_MODULE_APPLICATION                     0       Application
    MBED_MODULE_PLATFORM                        1       Platform
    MBED_MODULE_KERNEL                          2       RTX Kernel
    MBED_MODULE_NETWORK_STACK                   3       Network stack
    MBED_MODULE_HAL                             4       HAL - Hardware Abstraction Layer
    MBED_MODULE_MEMORY_SUBSYSTEM                5       Memory Subsystem
    MBED_MODULE_FILESYSTEM                      6       Filesystem
    MBED_MODULE_BLOCK_DEVICE                    7       Block device
    MBED_MODULE_DRIVER                          8       Driver
    MBED_MODULE_DRIVER_SERIAL                   9       Serial Driver
    MBED_MODULE_DRIVER_RTC                      10      RTC Driver
    MBED_MODULE_DRIVER_I2C                      11      I2C Driver
    MBED_MODULE_DRIVER_SPI                      12      SPI Driver
    MBED_MODULE_DRIVER_GPIO                     13      GPIO Driver
    MBED_MODULE_DRIVER_ANALOG                   14      Analog Driver
    MBED_MODULE_DRIVER_DIGITAL                  15      DigitalIO Driver
    MBED_MODULE_DRIVER_CAN                      16      CAN Driver
    MBED_MODULE_DRIVER_ETHERNET                 17      Ethernet Driver
    MBED_MODULE_DRIVER_CRC                      18      CRC Module
    MBED_MODULE_DRIVER_PWM                      19      PWM Driver
    MBED_MODULE_DRIVER_QSPI                     20      QSPI Driver
    MBED_MODULE_DRIVER_USB                      21      USB Driver
    MBED_MODULE_TARGET_SDK                      22      SDK
    MBED_MODULE_BLE                             23      BLE
    MBED_MODULE_NETWORK_STATS                   24      Network Statistics
    MBED_MODULE_UNKNOWN                         255     Unknown module
    \endverbatim
 */
typedef enum _mbed_module_type {
    MBED_MODULE_APPLICATION = 0,
    MBED_MODULE_PLATFORM,
    MBED_MODULE_KERNEL,
    MBED_MODULE_NETWORK_STACK,
    MBED_MODULE_HAL,
    MBED_MODULE_MEMORY_SUBSYSTEM,
    MBED_MODULE_FILESYSTEM,
    MBED_MODULE_BLOCK_DEVICE,
    MBED_MODULE_DRIVER,
    MBED_MODULE_DRIVER_SERIAL,
    MBED_MODULE_DRIVER_RTC,
    MBED_MODULE_DRIVER_I2C,
    MBED_MODULE_DRIVER_SPI,
    MBED_MODULE_DRIVER_GPIO,
    MBED_MODULE_DRIVER_ANALOG,
    MBED_MODULE_DRIVER_DIGITAL,
    MBED_MODULE_DRIVER_CAN,
    MBED_MODULE_DRIVER_ETHERNET,
    MBED_MODULE_DRIVER_CRC,
    MBED_MODULE_DRIVER_PWM,
    MBED_MODULE_DRIVER_QSPI,
    MBED_MODULE_DRIVER_USB,
    MBED_MODULE_DRIVER_WATCHDOG,
    MBED_MODULE_TARGET_SDK,
    MBED_MODULE_BLE,
    MBED_MODULE_NETWORK_STATS,
    /* Add More entities here as required */

    MBED_MODULE_UNKNOWN = 255,
    MBED_MODULE_MAX = MBED_MODULE_UNKNOWN
} mbed_module_type_t;

#endif // MBED_ERROR_H
