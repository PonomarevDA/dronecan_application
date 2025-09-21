/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_AIR_DATA_INDICATED_AIRSPEED_H_
#define UAVCAN_EQUIPMENT_AIR_DATA_INDICATED_AIRSPEED_H_

#include "libdcnode/serialization_internal.h"
#include "libdcnode/dronecan.h"

#define UAVCAN_EQUIPMENT_AIR_DATA_INDICATED_AIRSPEED_ID             1021
#define UAVCAN_EQUIPMENT_AIR_DATA_INDICATED_AIRSPEED_SIGNATURE      0xA1892D72AB8945FULL
#define UAVCAN_EQUIPMENT_AIR_DATA_INDICATED_AIRSPEED_MESSAGE_SIZE   4
#define UAVCAN_EQUIPMENT_AIR_DATA_INDICATED_AIRSPEED UAVCAN_EXPAND(UAVCAN_EQUIPMENT_AIR_DATA_INDICATED_AIRSPEED)

typedef struct {
    float indicated_airspeed;               // float16
    float indicated_airspeed_variance;      // float16
} IndicatedAirspeed;

#ifdef __cplusplus
extern "C" {
#endif

static inline int8_t dronecan_equipment_air_data_indicated_airspeed_serialize(
    const IndicatedAirspeed* const obj,
    uint8_t* const buffer,
    size_t* const inout_buffer_size_bytes)
{
    if ((obj == NULL) || (buffer == NULL) || (inout_buffer_size_bytes == NULL)) {
        return -2;
    }

    const size_t capacity_bytes = *inout_buffer_size_bytes;
    if (capacity_bytes < UAVCAN_EQUIPMENT_AIR_DATA_INDICATED_AIRSPEED_MESSAGE_SIZE) {
        return -3;
    }

    canardEncodeFloat16(buffer, 0,  obj->indicated_airspeed);
    canardEncodeFloat16(buffer, 16, obj->indicated_airspeed_variance);

    return 0;
}

static inline int8_t dronecan_equipment_air_data_indicated_airspeed_publish(
    const IndicatedAirspeed* const obj,
    uint8_t* inout_transfer_id)
{
    uint8_t buffer[UAVCAN_EQUIPMENT_AIR_DATA_INDICATED_AIRSPEED_MESSAGE_SIZE];
    size_t inout_buffer_size = UAVCAN_EQUIPMENT_AIR_DATA_INDICATED_AIRSPEED_MESSAGE_SIZE;
    dronecan_equipment_air_data_indicated_airspeed_serialize(obj, buffer, &inout_buffer_size);
    uavcanPublish(UAVCAN_EQUIPMENT_AIR_DATA_INDICATED_AIRSPEED_SIGNATURE,
                  UAVCAN_EQUIPMENT_AIR_DATA_INDICATED_AIRSPEED_ID,
                  inout_transfer_id,
                  CANARD_TRANSFER_PRIORITY_MEDIUM,
                  buffer,
                  UAVCAN_EQUIPMENT_AIR_DATA_INDICATED_AIRSPEED_MESSAGE_SIZE);

    return 0;
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_AIR_DATA_INDICATED_AIRSPEED_H_
