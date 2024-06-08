This application demonstrates how to use the Azure RTOS NetX Duo network stack with Open IoT SDK.

NetX Duo services are used to do the following:
* Create a client UDP socket and a server UDP socket on the loopback interface
* Send data from the client UDP socket and check that the server UDP socket receives it

## Walkthrough

* `tx_application_define`: Azure RTOS ThreadX function to create threads and initialise Azure RTOS NetX Duo
* `udp_socket_thread_entry`: Azure RTOS ThreadX thread to send a UDP packet on the loopback interface and check it is also received back.

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
FVP_Corstone_SSE-300_Ethos-U55 -C mps3_board.smsc_91c111.enabled=1 -C mps3_board.hostbridge.userNetworking=1 -a __build/examples/azure-netxduo/iotsdk-example-azure-netxduo.elf
```
