/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_H_
#define UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_H_

#include "libdcnode/internal.h"

#define UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_ID              10
#define UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_SIGNATURE       0xa7b622f939d1a466
#define UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_REQUEST_SIZE    7
#define UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_RESPONSE_SIZE   7
#define UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE      UAVCAN_EXPAND(UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE)

#ifdef __cplusplus
extern "C" {
#endif

static inline uint8_t uavcanProtocolParamExecuteOpcodeDecode(CanardRxTransfer* transfer)
{
    uint8_t opcode = 0;
    canardDecodeScalar(transfer, 0,  8, false, &opcode);
    return opcode;
}

static inline void uavcanProtocolParamExecuteOpcodeEncode(uint8_t* buffer, int8_t ok)
{
    int64_t val = 0;
    canardEncodeScalar(buffer, 0, 48, &val);
    canardEncodeScalar(buffer, 48, 1, &ok);
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_H_
