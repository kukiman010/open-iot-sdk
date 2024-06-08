# Azure RTOS
[Microsoft Azure RTOS](https://github.com/azure-rtos/) is a collection of
repositories for connecting deeply embedded IoT devices.

The Open IoT SDK currently adds following Microsoft Azure RTOS repositories:
* `threadx` - Azure RTOS ThreadX real-time operating system for deeply embedded
              devices.
* `netxduo` - Azure RTOS NetX Duo TCP/IP network stack for IoT applications
              running on Azure RTOS ThreadX.

It is not required to use all Azure RTOS components at the same time. However,
each components have required and optional (depending on the enabled feature)
dependencies.

See the table below for dependencies of the added Azure RTOS components:

| Component | Required dependency| Optional dependency|
|-----------|--------------------|--------------------|
| `threadx` | none               | none               |
| `netxduo` | `threadx`          | `filex` (to be added)|

The Open IoT SDK provides the following CMake options to fetch Azure RTOS
components via CMake FetchContent:
* `IOTSDK_THREADX` - fetch Azure RTOS ThreadX
* `IOTSDK_NETXDUO` - fetch Azure RTOS NetX Duo
It is necessary to enable fetching all required dependencies for Azure RTOS
components for example `IOTSDK_NETXDUO=ON` also requires `IOTSDK_THREADX=ON`.

Azure RTOS projects require knowledge of the toolchain and MCU core
architecture in order to build subdirectory paths to corresponding files that
provide ports support. The Open IoT SDK sets the CMake variables
`THREADX_TOOLCHAIN` and `THREADX_ARCH` to that effect in
[in the CMakeLists.txt for all Azure RTOS components](./CMakeLists.txt) and
requires no further application developer actions.

## [ThreadX](https://github.com/azure-rtos/threadx)
Azure RTOS ThreadX is a real-time operating system (RTOS) highly optimised for
deeply embedded systems.

## [NetX Duo](https://github.com/azure-rtos/netxduo)

Azure RTOS NetX Duo is a real time implementation of the TCP/IP standards that
works exclusively with Azure RTOS ThreadX as its Real-Time Operating System.

In its default configuration, it comes with Azure RTOS FileX enabled to support
server protocol handlers. However, this has been turned off as Azure RTOS FileX
is not yet an Azure RTOS component provided by the Open IoT SDK.

Additionally we have have enabled Azure IoT Middleware for Azure RTOS to
provide platform specific binding code between the Azure RTOS and the Azure SDK
for Embedded C which is made available through NetX Duo.


## License
Azure RTOS components are licensed under the Microsoft Software License Terms
for Microsoft Azure RTOS.
The [full license text is available at https://aka.ms/AzureRTOS_EULA](https://aka.ms/AzureRTOS_EULA).
