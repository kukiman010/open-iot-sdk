#!/bin/bash

# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Tip: set the ENV_ALREADY_SET environment variable to any non-empty string to skip installing packages if they are
# already installed in the active virtual environment.

HERE=$(realpath "$(dirname "$BASH_SOURCE")")

# Enter virtual environment if not already set. Create venv if one doesn't exist.
if [[ -z "$VIRTUAL_ENV" ]]; then
    if [[ -d venv ]]; then
        . venv/bin/activate
    elif [[ -d $HERE/venv ]]; then
        . $HERE/venv/bin/activate
    else
        if ! command -v virtualenv &>/dev/null; then
            pip install virtualenv
        fi
        virtualenv venv
        . venv/bin/activate
    fi
fi

# Download and install protoc
if [[ -z "$ENV_ALREADY_SET" ]]; then
    if ! command -v protoc &>/dev/null; then
        if [[ -d deps/proto/bin/protoc ]]; then
            export PATH=$PWD/deps/proto/bin:$PATH
        elif [[ -d $HERE/deps/proto/bin/protoc ]]; then
            export PATH=$HERE/deps/proto/bin:$PATH
        else
            if [ $(uname -m) = "aarch64" ]; then PACKAGE_ARCH="linux-aarch_64"; else PACKAGE_ARCH="linux-x86_64"; fi
            mkdir -p deps/proto
            PROTOC_URL=https://github.com/protocolbuffers/protobuf/releases/download/v3.19.6/protoc-3.19.6-${PACKAGE_ARCH}.zip

            if command -v curl &>/dev/null; then
                curl -L ${PROTOC_URL} >proto.zip
            elif command -v wget &>/dev/null; then
                wget -O proto.zip ${PROTOC_URL}
            else
                echo "curl or wget must be available to download protoc" >&2
                exit 1
            fi

            unzip -q -o -d deps/proto proto.zip
            rm proto.zip
            export PATH=$PWD/deps/proto/bin:$PATH
            ls -la $PWD/deps/proto/bin
        fi
    fi
    pip3 install -r $HERE/requirements.txt
fi

export PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION="python"
