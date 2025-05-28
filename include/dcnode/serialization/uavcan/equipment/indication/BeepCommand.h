/*
 * Copyright (C) 2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_INDICATION_BEEPCOMMAND_H_
#define UAVCAN_EQUIPMENT_INDICATION_BEEPCOMMAND_H_

#include <stdbool.h>
#include <string.h>
#include "dcnode/dcnode.h"
#include "serialization_internal.h"

#define UAVCAN_EQUIPMENT_INDICATION_BEEPCOMMAND_ID                          1080
#define UAVCAN_EQUIPMENT_INDICATION_BEEPCOMMAND_SIGNATURE                   0xBE9EA9FEC2B15D52ULL
#define UAVCAN_EQUIPMENT_INDICATION_BEEPCOMMAND_MAX_VALUE                   8192
#define UAVCAN_EQUIPMENT_INDICATION_BEEPCOMMAND_MESSAGE_SIZE                4
#define UAVCAN_EQUIPMENT_INDICATION_BEEPCOMMAND UAVCAN_EXPAND(UAVCAN_EQUIPMENT_INDICATION_BEEPCOMMAND)

typedef struct {
    float frequency;
    float duration;
} BeepCommand_t;

#ifdef __cplusplus
extern "C" {
#endif

static inline int8_t dronecan_equipment_indication_beep_command_deserialize(
    const CanardRxTransfer* transfer, BeepCommand_t* obj)
{
    if ((transfer == NULL) || (obj == NULL)) {
        return -2;
    }

    uint16_t f16_frequency_dummy;
    uint16_t f16_duration_dummy;

    canardDecodeScalar(transfer, 0,     16, true, &f16_frequency_dummy);
    canardDecodeScalar(transfer, 16,    16, true, &f16_duration_dummy);
    obj->duration = canardConvertFloat16ToNativeFloat(f16_duration_dummy);
    obj->frequency = canardConvertFloat16ToNativeFloat(f16_frequency_dummy);

    return 0;
}

static inline uint32_t dronecan_equipment_indication_beep_command_serialize(
    const BeepCommand_t* const obj,
    uint8_t* const buffer)
{
    if (obj == NULL || buffer == NULL) {
        return 0;
    }

    uint16_t frequency = canardConvertNativeFloatToFloat16(obj->frequency);
    uint16_t duration = canardConvertNativeFloatToFloat16(obj->duration);

    canardEncodeScalar(buffer, 0,   16,   &frequency);
    canardEncodeScalar(buffer, 16,  16,   &duration);

    return UAVCAN_EQUIPMENT_INDICATION_BEEPCOMMAND_MESSAGE_SIZE;
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_ESC_RAW_COMMAND_H_
