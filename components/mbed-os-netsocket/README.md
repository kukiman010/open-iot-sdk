# Mbed OS Netsocket

Adds a c++ wrapper around the IotSocket API to make it compatible with mbed-os netsocket 6.16.0.

# Limitations

## IP version selection

The stack supports using both IPv4 and IPv6 at the same time.
Socket type is determined by the address used in any operation that accepts an address.
Once the type is set it cannot be changed and using an incompatible address in subsequent calls will return an error.

Some calls do not accept an adress but require the socket to be opened:
    - bind
    - setsockopt
    - getsockopt
    - set_timeout
    - set_blocking

A new API call `InternetSocket::open(nsapi_version_t nsapi_socket_type)` has been added that allows you to set the IP version.

This is overloaded on the already mandatory call to `InternetSocket::open()`.
If the old call is used (which doesn't include a parameter) then the default IP version will be used (IPv4).
To use IPv6 as the default the compile time definition `DEFAULT_IPv6` must be defined in the mbed-os-netsocket component.

## Missing functions

The sockets are currently missing the following functions:
    - sigio
    - sendto_control
    - recvfrom_control

The DNS handling is missing from the TLS socket.

MBED_CONF_NSAPI_OFFLOAD_TLSSOCKET option is not supported.

# Tests

This components uses the Mbed OS unit tests. To do so we are using a mock of the CMSIS functions as well as the IotSocket API. The mocking framework used is Fake Function Framework (fff) and the testing framework is gtest.
