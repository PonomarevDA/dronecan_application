/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_AIR_DATA_STATIC_PRESSURE_H_
#define UAVCAN_EQUIPMENT_AIR_DATA_STATIC_PRESSURE_H_

#include "libdcnode/dronecan.h"
#include "libdcnode/serialization_internal.h"

#define UAVCAN_EQUIPMENT_AIR_DATA_STATIC_PRESSURE_ID                1028
#define UAVCAN_EQUIPMENT_AIR_DATA_STATIC_PRESSURE_SIGNATURE         0xcdc7c43412bdc89a
#define UAVCAN_EQUIPMENT_AIR_DATA_STATIC_PRESSURE_MESSAGE_SIZE      6
#define UAVCAN_EQUIPMENT_AIR_DATA_STATIC_PRESSURE UAVCAN_EXPAND(UAVCAN_EQUIPMENT_AIR_DATA_STATIC_PRESSURE)

typedef struct {
    float static_pressure;                              // float32, Pascal
    float static_pressure_variance;                     // float16, Pascal^2
} StaticPressure;

#ifdef __cplusplus
extern "C" {
#endif

static inline int8_t dronecan_equipment_air_data_static_pressure_serialize(
    const StaticPressure* const obj,
    uint8_t* const buffer,
    size_t* const inout_buffer_size_bytes)
{
    if ((obj == NULL) || (buffer == NULL) || (inout_buffer_size_bytes == NULL)) {
        return -2;
    }

    const size_t capacity_bytes = *inout_buffer_size_bytes;
    if (capacity_bytes < UAVCAN_EQUIPMENT_AIR_DATA_STATIC_PRESSURE_MESSAGE_SIZE) {
        return -3;
    }

    canardEncodeFloat32(buffer, 0,  obj->static_pressure);
    canardEncodeFloat16(buffer, 32, obj->static_pressure_variance);

    return 0;
}

static inline int8_t dronecan_equipment_air_data_static_pressure_publish(
    const StaticPressure* const obj,
    uint8_t* inout_transfer_id)
{
    uint8_t buffer[UAVCAN_EQUIPMENT_AIR_DATA_STATIC_PRESSURE_MESSAGE_SIZE];
    size_t inout_buffer_size = UAVCAN_EQUIPMENT_AIR_DATA_STATIC_PRESSURE_MESSAGE_SIZE;
    dronecan_equipment_air_data_static_pressure_serialize(obj, buffer, &inout_buffer_size);
    uavcanPublish(UAVCAN_EQUIPMENT_AIR_DATA_STATIC_PRESSURE_SIGNATURE,
                  UAVCAN_EQUIPMENT_AIR_DATA_STATIC_PRESSURE_ID,
                  inout_transfer_id,
                  CANARD_TRANSFER_PRIORITY_MEDIUM,
                  buffer,
                  UAVCAN_EQUIPMENT_AIR_DATA_STATIC_PRESSURE_MESSAGE_SIZE);

    return 0;
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_AIR_DATA_STATIC_PRESSURE_H_
