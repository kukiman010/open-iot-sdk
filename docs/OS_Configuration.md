# OS config

The below Real Time Operating Systems (RTOSes) are available in the Open IoT SDK:
- [RTX](https://www2.keil.com/mdk5/cmsis/rtx)
- [FreeRTOS](https://www.freertos.org/)
- [ThreadX](https://azure.microsoft.com/en-us/services/rtos/#overview)

All supported RTOSes can be used via the [CMSIS-RTOS v2 API](https://www.keil.com/pack/doc/CMSIS/RTOS2/html/index.html).

FreeRTOS and ThreadX can be used via their own native APIs.

## RTX (via CMSIS-RTOS v2 API)

In your application's `CMakeLists.txt`, do

```cmake
list(APPEND IOTSDK_FETCH_LIST cmsis-5)
```

before adding the Open IoT SDK to make available both the CMSIS-RTOS v2 API and the required RTX components from
CMSIS_5.

Once the SDK has been added, these libraries are available under the names `cmsis-rtos-api` and `cmsis-rtx`. Your
application can link `cmsis-rtos-api` against `cmsis-rtx` as the concrete implementation:

```cmake
target_link_libraries(cmsis-rtos-api PUBLIC cmsis-rtx)
```

This ensures all code that uses `cmsis-rtos-api` gets access to the implementation at the linking stage.

Full example applications can be found [here](https://git.gitlab.arm.com/iot/open-iot-sdk/examples/sdk-examples/-/tree/main/examples/cmsis-rtos-api/cmsis-rtx).

## FreeRTOS (via CMSIS-RTOS v2 API)

Your application's `CMakeLists.txt` should contain

```cmake
list(APPEND IOTSDK_FETCH_LIST cmsis-freertos cmsis-5)
```

before adding the Open IoT SDK.

Once the SDK has been added, your application can link `cmsis-rtos-api` against `freertos-cmsis-rtos` as the concrete implementation:

```cmake
target_link_libraries(cmsis-rtos-api PUBLIC freertos-cmsis-rtos)
```

This ensures all code that uses `cmsis-rtos-api` gets access to the implementation at the linking stage.

Full example applications can be found [here](https://git.gitlab.arm.com/iot/open-iot-sdk/examples/sdk-examples/-/tree/main/examples/cmsis-rtos-api/cmsis-freertos).

## FreeRTOS (via stand-alone kernel)

For vanilla FreeRTOS API support, do

```cmake
list(APPEND IOTSDK_FETCH_LIST cmsis-freertos)
```

before adding the Open IoT SDK.

Once the SDK has been added, `freertos-kernel` can then be linked in your application:

```cmake
target_link_libraries(my_app freertos-kernel)
```

You may also select a FreeRTOS heap implementation by linking with `freertos-kernel-heap-*` (\* is 1-5, [see for details](https://freertos.org/a00111.html)).

Full example applications can be found [here](https://git.gitlab.arm.com/iot/open-iot-sdk/examples/sdk-examples/-/tree/main/examples/freertos-api/freertos).

## ThreadX (via CMSIS-RTOS v2 API)

To enable use of ThreadX with the CMSIS-RTOSv2 API, fetch both ThreadX and the ThreadX CDI Port by doing

```cmake
list(APPEND IOTSDK_FETCH_LIST threadx threadx-cdi-port)
```

before adding the Open IoT SDK.

Once the SDK has been added, your application can link `cmsis-rtos-api` against `threadx-cdi-port` as the concrete implementation:

```cmake
target_link_libraries(cmsis-rtos-api PUBLIC threadx-cdi-port)
```

This ensures all code that uses `cmsis-rtos-api` gets access to the implementation at the linking stage.

Full example applications can be found [here](https://git.gitlab.arm.com/iot/open-iot-sdk/examples/sdk-examples/-/tree/main/examples/cmsis-rtos-api/cmsis-threadx).

## ThreadX (via stand-alone kernel)

Fetch ThreadX by doing

```cmake
list(APPEND IOTSDK_FETCH_LIST threadx)
```

before adding the Open IoT SDK.

Once the SDK has been added, the `threadx` cmake target can then be linked in your application:

```cmake
target_link_libraries(my_app threadx)
```

Full example applications can be found [here](https://git.gitlab.arm.com/iot/open-iot-sdk/examples/sdk-examples/-/tree/main/examples/threadx-api).
