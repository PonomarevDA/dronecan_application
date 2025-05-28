/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_DEVICE_TEMPERATURE_H_
#define UAVCAN_EQUIPMENT_DEVICE_TEMPERATURE_H_

#include <stdint.h>
#include <stdbool.h>
#include "dcnode/dcnode.h"
#include "serialization_internal.h"

#define UAVCAN_EQUIPMENT_DEVICE_TEMPERATURE_ID                  1110
#define UAVCAN_EQUIPMENT_DEVICE_TEMPERATURE_SIGNATURE           0x70261c28a94144c6
#define UAVCAN_EQUIPMENT_DEVICE_TEMPERATURE_MESSAGE_SIZE        5  // 40 bits
#define UAVCAN_EQUIPMENT_DEVICE_TEMPERATURE                     UAVCAN_EXPAND(UAVCAN_EQUIPMENT_DEVICE_TEMPERATURE)

typedef enum{
    ERROR_FLAG_OVERHEATING = 1,
    ERROR_FLAG_OVERCOOLING = 2,
} TemperatureErrorFlags_t;

typedef struct {
    uint16_t device_id;                   // uint16
    float temperature;                    // float16
    TemperatureErrorFlags_t error_flags;  // uint8
} Temperature_t;

#ifdef __cplusplus
extern "C" {
#endif


static inline uint32_t dronecan_equipment_temperature_serialize(
    const Temperature_t* const obj,
    uint8_t* const buffer)
{
    if (obj == NULL || buffer == NULL) {
        return 0;
    }

    canardEncodeScalar(buffer, 0, 16, &obj->device_id);
    canardEncodeFloat16(buffer, 16,  obj->temperature);
    canardEncodeScalar(buffer, 32, 8, &obj->error_flags);

    return UAVCAN_EQUIPMENT_DEVICE_TEMPERATURE_MESSAGE_SIZE;
}

#ifdef __cplusplus
}
#endif


#endif  // UAVCAN_EQUIPMENT_DEVICE_TEMPERATURE_H_
