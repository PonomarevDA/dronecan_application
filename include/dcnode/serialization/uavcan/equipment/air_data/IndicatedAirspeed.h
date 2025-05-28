/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_AIR_DATA_INDICATED_AIRSPEED_H_
#define UAVCAN_EQUIPMENT_AIR_DATA_INDICATED_AIRSPEED_H_

#include "serialization_internal.h"
#include "dcnode/dcnode.h"

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

static inline uint32_t dronecan_equipment_air_data_indicated_airspeed_serialize(
    const IndicatedAirspeed* const obj,
    uint8_t* const buffer)
{
    if (obj == NULL || buffer == NULL) {
        return 0;
    }

    canardEncodeFloat16(buffer, 0,  obj->indicated_airspeed);
    canardEncodeFloat16(buffer, 16, obj->indicated_airspeed_variance);

    return UAVCAN_EQUIPMENT_AIR_DATA_INDICATED_AIRSPEED_MESSAGE_SIZE;
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_AIR_DATA_INDICATED_AIRSPEED_H_
