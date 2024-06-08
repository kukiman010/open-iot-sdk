# pw_iot_sdk_config

## Overview

Global configuration for IoT SDK Pigweed backends.

Please see [pw_iot_sdk_config_defaults.h](include/pw_iot_sdk_config/config_defaults.h) for available settings and their default values.

To override, define `pw_iot_sdk_config_FILE` to the name of the header file which contains the chosen values.

This value can be provided by adding a compile definition to the `pw_iot_sdk_config` target:

```cmake
target_compile_definitions(pw_iot_sdk_config
    INTERFACE
        pw_iot_sdk_config_FILE="pw_cmsis_rtos_user_config.h"
)
```

### Note about RTOS

When the IoT SDK detects that CMSIS RTX or FreeRTOS is in use, some configuration values can be set automatically, but if a different RTOS is used they must be overridden. These cases are documented in [pw_iot_sdk_config_defaults.h](include/pw_iot_sdk_config_defaults.h)

## Dependencies

### FreeRTOS

#### `FreeRTOSConfig.h`

If a target called `freertos-config` exists it will be linked to `pw_iot_sdk_config`. This target should provide the header. Alternatively the header can be attached directly or via a different target using Cmake:

```cmake
target_include_directories(pw_iot_sdk_config
    INTERFACE
        path/to/freertos-config-h/
)
```

Or

```cmake
target_link_libraries(pw_iot_sdk_config
    INTERFACE
        target_providing_freertos_config_h
)
```

#### `CMSIS_DeviceHeader`

Must be defined to an includable header. This should be accessible via the `cmsis-core` target which is set up separately by the user.

### CMSIS RTX

#### `RTE_Components.h`

Expected to be accessible via the `cmsis-core` target.
