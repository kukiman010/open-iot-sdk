# Mbed-os-netsocket example

This an mbed example imported from https://github.com/ARMmbed/mbed-os-example-sockets.
The Open IoT SDK version of the example follows the original one appart from a few differences :
    - Hostname resolution is done from the example instead of from the component itself
    - Addition of a fake RNG for MbedTLS
    - Usage of CMSIS in the example
    - Usage of Pigweed log instead of mbed trace

## Additional FVP parameters

This example requires a network interface. To enable it on an MPS3-based FVP such as Corstone-300 AN552,
pass the following additional arguments to the FVP:

```
-C mps3_board.smsc_91c111.enabled=1 -C mps3_board.hostbridge.userNetworking=1
```
## Socket type selection and configuration

This example can use a TLS socket or a TCP socket (default), this can be changed by setting `MBED_CONF_APP_USE_TLS_SOCKET=ON` in project Cmake configuration.

The default socket type used is NSAPIv4. To use IPv6 by default the flag `DEFAULT_IPv6` should be defined.
