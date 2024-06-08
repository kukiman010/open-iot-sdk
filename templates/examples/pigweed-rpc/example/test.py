#!/usr/bin/env python3

# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

import logging  # noqa: F401
from functools import partial
import os

import pytest

from pw_hdlc.rpc import HdlcRpcClient, default_channels

from pyedmgr import (  # noqa: F401
    AbstractChannel,
    SynchronousSocketChannel,
    TestCaseContext,
    TestDevice,
    fixture_test_case,
    logger,
)

PROTO = os.environ["PROTO"]
RPC_READ_SIZE = 1
LOG_TERMINAL = 0
RPC_TERMINAL = 1


@pytest.mark.asyncio
@pytest.mark.parametrize(
    "fixture_test_case", ["@test.json"], indirect=["fixture_test_case"]
)
async def test_rpc(fixture_test_case, logger):  # noqa: F811
    async with fixture_test_case as context:
        context: TestCaseContext
        device = context.allocated_devices[0]
        device.channel.close()

        logger.debug("Open log channel")
        log_channel: SynchronousSocketChannel = device.controller.get_channel(
            terminal=LOG_TERMINAL, sync=True
        )
        log_channel.open()

        logger.debug("Open RPC channel")
        device.channel.close()
        rpc_channel = device.controller.get_channel(terminal=RPC_TERMINAL, sync=True)
        rpc_channel.open()

        logger.info("Create HdlcRpcClient")

        client = HdlcRpcClient(
            partial(rpc_channel.read, RPC_READ_SIZE),
            [PROTO],
            default_channels(rpc_channel.write),
        )

        logger.info("Create EchoService")
        echo_service = client.rpcs().iotsdk.example.pw_rpc.EchoService

        logger.debug(
            log_channel.readline().strip().decode("ascii")
        )  # Kernel initialised
        logger.debug(
            log_channel.readline().strip().decode("ascii")
        )  # INF - main - Starting kernel
        logger.debug(
            log_channel.readline().strip().decode("ascii")
        )  # INF - server - Starting server

        for i in range(150):
            message = f"Hello, world {i}"
            logger.info(f"> {message}")
            status, payload = echo_service.Echo(text=message)
            logger.info(f"< {payload.text}")

            logger.debug(
                log_channel.readline().strip().decode("ascii")
            )  # Got echo request
            logger.debug(
                log_channel.readline().strip().decode("ascii")
            )  # Sending echo response

        assert status.ok()
        assert payload.text == message
