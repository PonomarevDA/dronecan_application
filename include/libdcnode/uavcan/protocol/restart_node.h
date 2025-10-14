/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_PROTOCOL_RESTART_NODE_H_
#define UAVCAN_PROTOCOL_RESTART_NODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "libdcnode/internal.h"

#define UAVCAN_PROTOCOL_RESTART_NODE_ID             5
#define UAVCAN_PROTOCOL_RESTART_NODE_SIGNATURE      0x569e05394a3017f0
#define UAVCAN_PROTOCOL_RESTART_NODE_RESPONSE_SIZE  1
#define UAVCAN_PROTOCOL_RESTART_NODE                UAVCAN_EXPAND(UAVCAN_PROTOCOL_RESTART_NODE)

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_PROTOCOL_RESTART_NODE_H_
