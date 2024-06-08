/* Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "netsocket/InternetSocket.h"

#include "IotSocketToMbed.h"
#include "pw_log/log.h"

InternetSocket::~InternetSocket()
{
    // this is to retain behaviour of the mbed version and is safe
    // because delete will only happen once, gated by _factory_allocated
    InternetSocket::close();
}

nsapi_error_t InternetSocket::open(nsapi_version_t nsapi_socket_type)
{
    nsapi_error_t err = NSAPI_ERROR_OK;

    osMutexAcquire(_socket_lock, osWaitForever);

    if (_socket_opened) {
        err = NSAPI_ERROR_UNKNOWN;
    } else if (nsapi_socket_type == NSAPI_IPv4) {
        if (_iot_socket_af_type && (_iot_socket_af_type != IOT_SOCKET_AF_INET)) {
            err = NSAPI_ERROR_PARAMETER;
        } else {
            _iot_socket_af_type = IOT_SOCKET_AF_INET;
        }
    } else if (nsapi_socket_type == NSAPI_IPv6) {
        if (_iot_socket_af_type && (_iot_socket_af_type != IOT_SOCKET_AF_INET6)) {
            err = NSAPI_ERROR_PARAMETER;
        } else {
            _iot_socket_af_type = IOT_SOCKET_AF_INET6;
        }
    } else {
        err = NSAPI_ERROR_PARAMETER;
    }

    if (err == NSAPI_ERROR_OK) {
        _socket_opened = true;
    }

    osMutexRelease(_socket_lock);

    return err;
}

nsapi_error_t InternetSocket::close()
{
    if (_factory_allocated) {
        _factory_allocated = false;
        // delete gated by _factory_allocated but clang static analyser still warns about possible double delete
#ifndef __clang_analyzer__
        delete this;
#endif
    } else {
        osMutexAcquire(_socket_lock, osWaitForever);
        int32_t ret = iotSocketClose(_socket_id);
        if (ret == 0) {
            _socket_opened = false;
            _socket_id = IOT_SOCKET_ID_INVALID;
            _iot_socket_af_type = 0;
        }
        osMutexRelease(_socket_lock);
        return iotSocketErrorToMbed(ret);
    }

    return NSAPI_ERROR_OK;
}

int InternetSocket::join_multicast_group(const SocketAddress &address)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

int InternetSocket::leave_multicast_group(const SocketAddress &address)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

nsapi_error_t InternetSocket::bind(uint16_t port)
{
    if (!_socket_opened) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    uint8_t ip[NSAPI_IPv6_BYTES];

    if (!verify_socket_type()) {
        return NSAPI_ERROR_PARAMETER;
    }

    int32_t rc = iotSocketBind(_socket_id, ip, get_iot_address_size(), port);
    return iotSocketErrorToMbed(rc);
}

nsapi_error_t InternetSocket::bind(const SocketAddress &address)
{
    if (!_socket_opened) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    if (!verify_socket_type(address.get_ip_version())) {
        return NSAPI_ERROR_PARAMETER;
    }

    const uint8_t *iot_address = reinterpret_cast<const uint8_t *>(address.get_ip_address());

    int32_t rc = iotSocketBind(_socket_id, iot_address, get_iot_address_size(), address.get_port());

    return iotSocketErrorToMbed(rc);
}

void InternetSocket::set_blocking(bool blocking)
{
    set_timeout(blocking ? -1 : 0);
}

void InternetSocket::set_timeout(int timeout)
{
    osMutexAcquire(_socket_lock, osWaitForever);

    // to maintain mbed os behaviour we are simulating the open state and here we need
    // to temporarily set it to true since setting timeout is allowed on non open socket
    bool old_value = _socket_opened;
    _socket_opened = true;
    if (!verify_socket_type()) {
        _socket_opened = old_value;
        osMutexRelease(_socket_lock);
        // no place to report error
        PW_LOG_ERROR("InternetSocket::set_timeout() failed because socket could not be created");
        return;
    }
    _socket_opened = old_value;

    uint32_t is_blocking = timeout ? 1 : 0;

    if (timeout < 0) {
        _timeout = osWaitForever;
    } else {
        _timeout = (uint32_t)timeout;
    }

    if (is_blocking) {
        iotSocketSetOpt(_socket_id, IOT_SOCKET_SO_RCVTIMEO, static_cast<const void *>(&_timeout), sizeof(uint32_t));
        iotSocketSetOpt(_socket_id, IOT_SOCKET_SO_SNDTIMEO, static_cast<const void *>(&_timeout), sizeof(uint32_t));
    }

    iotSocketSetOpt(_socket_id, IOT_SOCKET_IO_FIONBIO, static_cast<const void *>(&is_blocking), sizeof(uint32_t));

    osMutexRelease(_socket_lock);
}

nsapi_error_t InternetSocket::setsockopt(int level, int optname, const void *optval, unsigned optlen)
{
    if (!verify_socket_type()) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    int32_t rc = IOT_SOCKET_ENOTSUP;

    if (level == NSAPI_SOCKET) {
        switch (optname) {
            case NSAPI_KEEPALIVE:
                rc = iotSocketSetOpt(_socket_id, IOT_SOCKET_SO_KEEPALIVE, optval, optlen);
                break;
            default:
                break;
        }
    }

    return iotSocketErrorToMbed(rc);
}

nsapi_error_t InternetSocket::getsockopt(int level, int optname, void *optval, unsigned *optlen)
{
    if (!verify_socket_type()) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    int32_t rc = IOT_SOCKET_ENOTSUP;

    if (level == NSAPI_SOCKET) {
        switch (optname) {
            case NSAPI_KEEPALIVE:
                static_assert(
                    sizeof(unsigned) == sizeof(uint32_t),
                    "mbed-os-netsockets does not work on architectures where unsigned int is different from uint32_t");
                rc = iotSocketGetOpt(_socket_id, IOT_SOCKET_SO_KEEPALIVE, optval, reinterpret_cast<uint32_t *>(optlen));
                break;
            default:
                break;
        }
    }

    return iotSocketErrorToMbed(rc);
}

nsapi_error_t InternetSocket::getpeername(SocketAddress *address)
{
    if (!address) {
        return NSAPI_ERROR_PARAMETER;
    }

    if (_socket_id == IOT_SOCKET_ID_INVALID) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    uint8_t iot_address[NSAPI_IPv6_BYTES];
    uint32_t address_size = get_iot_address_size();
    uint16_t port;

    int32_t rc = iotSocketGetPeerName(_socket_id, iot_address, &address_size, &port);

    if (rc == IOT_SOCKET_ENOTCONN) {
        return NSAPI_ERROR_NO_CONNECTION;
    } else {
        address->set_ip_bytes(iot_address, get_ip_nsapi_version());
        address->set_port(port);
    }

    return NSAPI_ERROR_OK;
}
