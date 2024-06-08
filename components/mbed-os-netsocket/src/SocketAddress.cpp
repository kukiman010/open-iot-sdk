/* Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "netsocket/SocketAddress.h"

#include "ip4string.h"
#include "ip6string.h"

#include <stdio.h>
#include <string.h>

SocketAddress::SocketAddress(const nsapi_addr_t &addr, uint16_t port) : _addr(addr), _port(port)
{
}

SocketAddress::SocketAddress(const char *addr, uint16_t port) : _port(port)
{
    set_ip_address(addr);
}

SocketAddress::SocketAddress(const void *bytes, nsapi_version_t version, uint16_t port) : _port(port)
{
    set_ip_bytes(bytes, version);
}

SocketAddress::SocketAddress(const SocketAddress &addr) : _addr(addr._addr), _port(addr._port)
{
}

bool SocketAddress::set_ip_address(const char *addr)
{
    _ip_address = nullptr;

    if (addr && stoip4(addr, strlen(addr), _addr.bytes)) {
        _addr.version = NSAPI_IPv4;
        return true;
    } else if (addr && stoip6(addr, strlen(addr), _addr.bytes)) {
        _addr.version = NSAPI_IPv6;
        return true;
    } else {
        _addr = nsapi_addr_t();
        return false;
    }
}

void SocketAddress::set_ip_bytes(const void *bytes, nsapi_version_t version)
{
    nsapi_addr_t addr{};

    addr.version = version;
    if (version == NSAPI_IPv6) {
        memcpy(addr.bytes, bytes, NSAPI_IPv6_BYTES);
    } else if (version == NSAPI_IPv4) {
        memcpy(addr.bytes, bytes, NSAPI_IPv4_BYTES);
    }
    set_addr(addr);
}

void SocketAddress::set_addr(const nsapi_addr_t &addr)
{
    _ip_address = nullptr;
    _addr = addr;
}

const char *SocketAddress::get_ip_address() const
{
    if (_addr.version == NSAPI_UNSPEC) {
        return nullptr;
    }

    if (!_ip_address) {
        _ip_address.reset(new char[NSAPI_IP_SIZE]);
        if (_addr.version == NSAPI_IPv4) {
            ip4tos(_addr.bytes, _ip_address.get());
        } else if (_addr.version == NSAPI_IPv6) {
            ip6tos(_addr.bytes, _ip_address.get());
        }
    }

    return _ip_address.get();
}

SocketAddress::operator bool() const
{
    if (_addr.version == NSAPI_IPv4) {
        for (int i = 0; i < NSAPI_IPv4_BYTES; i++) {
            if (_addr.bytes[i]) {
                return true;
            }
        }

        return false;
    } else if (_addr.version == NSAPI_IPv6) {
        for (int i = 0; i < NSAPI_IPv6_BYTES; i++) {
            if (_addr.bytes[i]) {
                return true;
            }
        }

        return false;
    } else {
        return false;
    }
}

SocketAddress &SocketAddress::operator=(const SocketAddress &addr)
{
    set_addr(addr._addr);
    set_port(addr._port);
    return *this;
}

bool operator==(const SocketAddress &a, const SocketAddress &b)
{
    if (!a && !b) {
        return true;
    } else if (a.get_addr().version != b.get_addr().version) {
        return false;
    } else if (a.get_port() != b.get_port()) {
        return false;
    } else if (a.get_addr().version == NSAPI_IPv4) {
        return memcmp(a.get_addr().bytes, b.get_addr().bytes, NSAPI_IPv4_BYTES) == 0;
    } else { // NSAPI_IPv6
        return memcmp(a.get_addr().bytes, b.get_addr().bytes, NSAPI_IPv6_BYTES) == 0;
    }
}

bool operator!=(const SocketAddress &a, const SocketAddress &b)
{
    return !(a == b);
}
