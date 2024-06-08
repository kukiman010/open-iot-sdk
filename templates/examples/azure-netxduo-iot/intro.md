This application shows the usability of Azure RTOS API to connect to Azure IoT and start interacting with Azure IoT IoTHub with Telemetry data.

## Prerequisites
To run this sample, user should create an IoTHub instance in Azure ([Doc](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-create-through-portal#create-an-iot-hub)). If sample is configured to use Device Provisioning service, use [doc](https://docs.microsoft.com/en-us/azure/iot-dps/quick-setup-auto-provision#create-a-new-iot-hub-device-provisioning-service) to create Device Provisioning instance in Azure and link it to the IoTHub ([Link](https://docs.microsoft.com/en-us/azure/iot-dps/quick-setup-auto-provision#link-the-iot-hub-and-your-device-provisioning-service)).

## IoTHub connect using Symmetric key

Sample connects to the device in the IoTHub using Symmetric key. To register new device in IoTHub use the [doc](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-create-through-portal#register-a-new-device-in-the-iot-hub) to create device with Symmetric key authentication. After device's registration is complete, copy the connection string for the device with following format `HostName=<>;DeviceId=<>;SharedAccessKey=<>`. Edit the following macros in `sample_azure_iot_credentials.h`:

```
#define HOST_NAME                                   "<Hostname from connection string>"
#define DEVICE_ID                                   "<DeviceId from connection string>"
#define DEVICE_SYMMETRIC_KEY                        "<SharedAccessKey from connection string>"
```

## Expected behaviours
The application should print out information similar to the content of [`test.log`](test.log) when successful.

## Additional FVP parameters

This example requires a network interface. To enable it on an MPS3-based FVP such as Corstone-300 AN552,
pass the following additional arguments to the FVP:

```
-C mps3_board.smsc_91c111.enabled=1 -C mps3_board.hostbridge.userNetworking=1
```

E.g.
```
FVP_Corstone_SSE-300_Ethos-U55 -C mps3_board.smsc_91c111.enabled=1 -C mps3_board.hostbridge.userNetworking=1 -a __build/examples/azure-netxduo-iot/iotsdk-example-azure-netxduo-iot.elf
```
