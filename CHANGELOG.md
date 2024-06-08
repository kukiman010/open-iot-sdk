# v2023.01 (2023-01-19)

## Changes

* ci: Run cppcheck using CMake-generated `compile_commands.json` which contains a list of files and compiler flags. This provides cppcheck with more complete information of which files belong to one build and how each file should be processed, so that cppcheck can provide better code analysis and catch issues that have been missed before.
* ci: Tidy up to enable in public GitLab instance
* autobot: Fix automatic merge by self approving the MRs
* components: Add CMSIS-DSP as a component that can be fetched by having `cmsis-dsp` in the `IOTSDK_FETCH_LIST` variable.
* components: Add Arm-2D as a component that can be fetched by having `arm-2d` in the `IOTSDK_FETCH_LIST` variable.
* tests: Create a test application to verify usability of Arm-2D and CMSIS-DSP with the Open IoT SDK.
* contributing: License file requires to list all components
* cmsis-dsp: Update CMSIS-DSP to the latest and remove the workarounds for CMake entry point and vector conversions.
* ci: Add public sync from public Gitlab to internal GitLab
* cmake: Fix the issue that `lwip-cmsis-sys` always links against `cmsis-rtx` and ensure linking order is correct for callers and implementations of `cmsis-rtos-api`.
* freertos: Convert FreeRTOS-related libraries from interface to static, to ensure the code is built only once rather than inherited as source files by every library or executable that uses FreeRTOS.
* license: Add components information

  LICENSE.md does not only list name of components and their
  licenses but also more detailed information like version,
  url-origin.
* components: Add NetX Duo as a component that can be fetched by having `netxduo` in the `IOTSDK_FETCH_LIST` variable.
* netxduo: Add IP link driver callback for NetX Duo, using EMAC and Network Stack Memory Manager APIs from MCU-Driver-HAL.
* netxduo: Add an example application that demonstrates usage of NetX Duo with the IP link driver callback based on MCU-Driver-HAL.
* netxduo: Add NetX Duo example that talks to an instance of Azure IoT Hub.
* ci: Test the NetX Duo Azure IoT example.
* clang-format: Fix issues with sorting includes

  Sorting is now enabled with the recent developer-tools version.
* netxduo-iot: Fix example Azure IoTHub init error handling.
* Pigweed: Update version.

  - A patch is applied to pass Cpp check and deal with other specificities of the Open IoT SDK.
  - Linkage between backends and modules has been improved.
* CI: Filter out files originating from dependencies when cppcheck is run.
* Testing: Add pyedmgr as an Open IoT SDK dependency. Pyedmgr is a python package that can discover, flash and communicate with multiple boards/FVP. Paired with pytest it can be used to write test which run on many boards.
* Pigweed: Add MDH based backend for pw_sys_io. This change makes other Pigweed modules based on pw_sys_io available to Open IoT SDK users.
* Pigweed: Add CMSIS RTOSv2 based backend for pw_sync and pw_thread modules. This allows users of the Open IoT SDK to get access to these modules and other modules depending on them.
* Pigweed: Add chrono backend.
* Testing: Enable Pigweed unit test suite. This test suite is used to validate the multiple backends implemented in Open IoT SDK.
* Example: Add pigweed RPC example. It demonstrates how to use pigweed RPC with pytest and pyedmgr to build tests that interact with the device through RPCs.
* ci: Create IoT devices in cloud dynamically
* cmsis-freertos: Update v10.5.1
* tf-m: Update to version 1.7.0
* netxduo: Remove fallback to THREADX_TOOLCHAIN=gnu and THREADX_ARCH=cortex_m0, because the latest NetX Duo now comes with `gnu` and `ac6` ports for Cortex-M55 and Cortex-M85 with CMake support.
* connectivity: Update iot socket repository to use the public gitlab repo which is used to extend the iot socket API.
* tls: Update Mbed TLS git URL. Point to Mbed TLS main repository on Github instead of its old mirror.
* changelog: Add towncrier news fragments and configuration


This changelog should be read in conjunction with any release notes provided
for a specific release version.
