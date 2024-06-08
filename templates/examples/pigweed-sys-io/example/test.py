#!/usr/bin/env python3

# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

import pytest
import sys

from pyedmgr import (  # noqa: F401
    fixture_test_case,
    logger,
    TestCaseContext,
    TestDevice,
)


@pytest.mark.asyncio
@pytest.mark.parametrize(
    "fixture_test_case,message",
    [("@test.json", b"Hello, world")],
    indirect=["fixture_test_case"],
)
async def test_sys_io(fixture_test_case, message, logger):  # noqa: F811
    async with fixture_test_case as context:
        context: TestCaseContext
        device: TestDevice = context.allocated_devices[0]

        expectations = [
            b"Kernel initialised",
            b"Starting kernel",
            b"Type something at the prompt",
        ]

        index = 0
        while index < len(expectations):
            line = await device.channel.readline_async()
            line = line.strip()

            prefix = "  "
            if expectations[index] in line:
                prefix = "✔ "
                index += 1

            logger.info(prefix + line.decode("utf-8"))

        logger.info("ready to send bytes...")

        for i in range(150):
            message = f"Hello World {i}"
            for c in str.encode(message + "\n"):
                await device.channel.write_async(c.to_bytes(1, sys.byteorder))

            line = await device.channel.readline_async()
            line = line.strip()
            logger.info(line.decode("utf-8"))

        assert index == len(expectations)
