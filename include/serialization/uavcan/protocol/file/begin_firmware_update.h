/*
 * Copyright (C) 2025 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_PROTOCOL_FILE_BEGIN_FIRMWARE_UPDATE_H_
#define UAVCAN_PROTOCOL_FILE_BEGIN_FIRMWARE_UPDATE_H_

#include "dronecan_application_internal.h"

#define UAVCAN_PROTOCOL_FILE_BEGIN_FIRMWARE_UPDATE_ID               40
#define UAVCAN_PROTOCOL_FILE_BEGIN_FIRMWARE_UPDATE_SIGNATURE        0xB7D725DF72724126ULL
#define UAVCAN_PROTOCOL_FILE_BEGIN_FIRMWARE_UPDATE_RESPONSE_SIZE    1
#define UAVCAN_PROTOCOL_FILE_BEGIN_FIRMWARE_UPDATE                  UAVCAN_EXPAND(UAVCAN_PROTOCOL_FILE_BEGIN_FIRMWARE_UPDATE)

#endif  // UAVCAN_PROTOCOL_FILE_BEGIN_FIRMWARE_UPDATE_H_
