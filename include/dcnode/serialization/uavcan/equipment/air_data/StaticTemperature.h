/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_AIR_DATA_STATIC_TEMPERATURE_H_
#define UAVCAN_EQUIPMENT_AIR_DATA_STATIC_TEMPERATURE_H_

#include "dcnode/dcnode.h"
#include "serialization_internal.h"

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

static inline uint32_t dronecan_equipment_air_data_static_temperature_serialize(
    const StaticTemperature* const obj,
    uint8_t* const buffer)
{
    if (obj == NULL || buffer == NULL) {
        return 0;
    }

    canardEncodeFloat16(buffer, 0,  obj->static_temperature);
    canardEncodeFloat16(buffer, 16, obj->static_temperature_variance);

    return UAVCAN_EQUIPMENT_AIR_DATA_STATIC_TEMPERATURE_MESSAGE_SIZE;
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_AIR_DATA_STATIC_TEMPERATURE_H_
