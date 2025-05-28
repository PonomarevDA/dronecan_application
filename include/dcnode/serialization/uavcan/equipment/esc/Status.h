/*
 * Copyright (C) 2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_ESC_STATUS_H_
#define UAVCAN_EQUIPMENT_ESC_STATUS_H_

#include <stdbool.h>
#include "serialization_internal.h"

#define UAVCAN_EQUIPMENT_ESC_STATUS_ID                              1034
#define UAVCAN_EQUIPMENT_ESC_STATUS_SIGNATURE                       0xa9af28aea2fbb254
#define UAVCAN_EQUIPMENT_ESC_STATUS_MESSAGE_SIZE                    14  // 110 / 8

#define ESC_STATUS_MAX_IDX 31
typedef struct {
    uint32_t error_count;           // uint32
    float voltage;                  // float16  Volt
    float current;                  // float16  Ampere
    float temperature;              // float16  Kelvin
    int32_t rpm;                    // int18
    uint8_t power_rating_pct;       // uint7 (range 0% to 127%)
    uint8_t esc_index;              // uint5
} EscStatus_t;

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t dronecan_equipment_esc_status_serialize(
    const EscStatus_t* const obj,
    uint8_t* const buffer)
{
    if (obj == NULL || buffer == NULL) {
        return 0;
    }

    uint16_t voltage = canardConvertNativeFloatToFloat16(obj->voltage);
    uint16_t current = canardConvertNativeFloatToFloat16(obj->current);
    uint16_t temperature = canardConvertNativeFloatToFloat16(obj->temperature);

    canardEncodeScalar(buffer, 0,   32, &obj->error_count);
    canardEncodeScalar(buffer, 32,  16, &voltage);
    canardEncodeScalar(buffer, 48,  16, &current);
    canardEncodeScalar(buffer, 64,  16, &temperature);
    canardEncodeScalar(buffer, 80,  18, &obj->rpm);
    canardEncodeScalar(buffer, 98,  7,  &obj->power_rating_pct);
    canardEncodeScalar(buffer, 105, 5,  &obj->esc_index);

    return UAVCAN_EQUIPMENT_ESC_STATUS_MESSAGE_SIZE;
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_ESC_STATUS_H_
