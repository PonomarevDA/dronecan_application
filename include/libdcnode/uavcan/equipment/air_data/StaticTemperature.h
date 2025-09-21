/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_AIR_DATA_STATIC_TEMPERATURE_H_
#define UAVCAN_EQUIPMENT_AIR_DATA_STATIC_TEMPERATURE_H_

#include "libdcnode/dronecan.h"
#include "libdcnode/serialization_internal.h"

#define UAVCAN_EQUIPMENT_AIR_DATA_STATIC_TEMPERATURE_ID             1029
#define UAVCAN_EQUIPMENT_AIR_DATA_STATIC_TEMPERATURE_SIGNATURE      0x49272a6477d96271
#define UAVCAN_EQUIPMENT_AIR_DATA_STATIC_TEMPERATURE_MESSAGE_SIZE   4
#define UAVCAN_EQUIPMENT_AIR_DATA_STATIC_TEMPERATURE UAVCAN_EXPAND(UAVCAN_EQUIPMENT_AIR_DATA_STATIC_TEMPERATURE)

typedef struct {
    float static_temperature;                           // float16, Kelvin
    float static_temperature_variance;                  // float16, Kelvin^2
} StaticTemperature;

#ifdef __cplusplus
extern "C" {
#endif

static inline int8_t dronecan_equipment_air_data_static_temperature_serialize(
    const StaticTemperature* const obj,
    uint8_t* const buffer,
    size_t* const inout_buffer_size_bytes)
{
    if ((obj == NULL) || (buffer == NULL) || (inout_buffer_size_bytes == NULL)) {
        return -2;
    }

    const size_t capacity_bytes = *inout_buffer_size_bytes;
    if (capacity_bytes < UAVCAN_EQUIPMENT_AIR_DATA_STATIC_TEMPERATURE_MESSAGE_SIZE) {
        return -3;
    }

    canardEncodeFloat16(buffer, 0,  obj->static_temperature);
    canardEncodeFloat16(buffer, 16, obj->static_temperature_variance);

    return 0;
}

static inline int8_t dronecan_equipment_air_data_static_temperature_publish(
    const StaticTemperature* const obj,
    uint8_t* inout_transfer_id)
{
    uint8_t buffer[UAVCAN_EQUIPMENT_AIR_DATA_STATIC_TEMPERATURE_MESSAGE_SIZE];
    size_t inout_buffer_size = UAVCAN_EQUIPMENT_AIR_DATA_STATIC_TEMPERATURE_MESSAGE_SIZE;
    dronecan_equipment_air_data_static_temperature_serialize(obj, buffer, &inout_buffer_size);
    uavcanPublish(UAVCAN_EQUIPMENT_AIR_DATA_STATIC_TEMPERATURE_SIGNATURE,
                  UAVCAN_EQUIPMENT_AIR_DATA_STATIC_TEMPERATURE_ID,
                  inout_transfer_id,
                  CANARD_TRANSFER_PRIORITY_MEDIUM,
                  buffer,
                  UAVCAN_EQUIPMENT_AIR_DATA_STATIC_TEMPERATURE_MESSAGE_SIZE);

    return 0;
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_AIR_DATA_STATIC_TEMPERATURE_H_
