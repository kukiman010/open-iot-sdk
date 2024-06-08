// Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
#define PW_LOG_MODULE_NAME "server"

#include <string.h>

#include "pw_rpc/server.h"

#include "iotsdk-example-pigweed-rpc.pwpb.h"
#include "iotsdk-example-pigweed-rpc.rpc.pwpb.h"
#include "pw_hdlc/rpc_channel.h"
#include "pw_hdlc/rpc_packets.h"
#include "pw_log/log.h"
#include "pw_rpc/channel.h"
#include "pw_stream/sys_io_stream.h"
#include "pw_string/util.h"

#include <algorithm>
#include <array>

namespace {
using EchoRequest = iotsdk::example::pw_rpc::EchoRequest::Message;
using EchoResponse = iotsdk::example::pw_rpc::EchoResponse::Message;

template <class T> using EchoServiceBase = iotsdk::example::pw_rpc::pw_rpc::pwpb::EchoService::Service<T>;

constexpr size_t kBufferSize = 150;

pw::stream::SysIoWriter writer;

pw::hdlc::RpcChannelOutput hdlc_channel_output(writer, pw::hdlc::kDefaultRpcAddress, "HDLC output");

std::array<pw::rpc::Channel, 1> channels{pw::rpc::Channel::Create<1>(&hdlc_channel_output)};

pw::rpc::Server server(channels);

struct EchoService : EchoServiceBase<EchoService> {
    pw::Status Echo(const EchoRequest &request, EchoResponse &response)
    {
        PW_LOG_DEBUG("Got echo request: \"%.*s\"", request.text.size(), request.text.data());

        response.text = request.text;

        PW_LOG_DEBUG("Responding \"%.*s\"", response.text.size(), response.text.data());

        return pw::OkStatus();
    }
} service;
} // namespace

void start_rpc_app()
{
    static std::array<std::byte, kBufferSize> buffer;

    PW_LOG_INFO("Starting server");

    server.RegisterService(service);

    pw::hdlc::ReadAndProcessPackets(server, hdlc_channel_output, buffer);
}
