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
#include "libdcnode/dronecan.h"
#include "libdcnode/serialization_internal.h"

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


static inline int8_t dronecan_equipment_temperature_serialize(
    const Temperature_t* const obj,
    uint8_t* const buffer,
    size_t* const inout_buffer_size_bytes)
{
    if ((obj == NULL) || (buffer == NULL) || (inout_buffer_size_bytes == NULL)) {
        return -2;
    }

    const size_t capacity_bytes = *inout_buffer_size_bytes;
    if (capacity_bytes < UAVCAN_EQUIPMENT_DEVICE_TEMPERATURE_MESSAGE_SIZE) {
        return -3;
    }

    canardEncodeScalar(buffer, 0, 16, &obj->device_id);
    canardEncodeFloat16(buffer, 16,  obj->temperature);
    canardEncodeScalar(buffer, 32, 8, &obj->error_flags);

    return 0;
}

static inline int8_t dronecan_equipment_temperature_publish(
    const Temperature_t* const obj,
    uint8_t* inout_transfer_id)
{
    uint8_t buffer[UAVCAN_EQUIPMENT_DEVICE_TEMPERATURE_MESSAGE_SIZE];
    size_t inout_buffer_size = UAVCAN_EQUIPMENT_DEVICE_TEMPERATURE_MESSAGE_SIZE;
    dronecan_equipment_temperature_serialize(obj, buffer, &inout_buffer_size);
    uavcanPublish(UAVCAN_EQUIPMENT_DEVICE_TEMPERATURE_SIGNATURE,
                  UAVCAN_EQUIPMENT_DEVICE_TEMPERATURE_ID,
                  inout_transfer_id,
                  CANARD_TRANSFER_PRIORITY_MEDIUM,
                  buffer,
                  UAVCAN_EQUIPMENT_DEVICE_TEMPERATURE_MESSAGE_SIZE);

    return 0;
}

#ifdef __cplusplus
}
#endif


#endif  // UAVCAN_EQUIPMENT_DEVICE_TEMPERATURE_H_
