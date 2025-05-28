/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_ICE_RECIPROCATING_FUEL_TANK_STATUS_H_
#define UAVCAN_EQUIPMENT_ICE_RECIPROCATING_FUEL_TANK_STATUS_H_

#include <stdint.h>
#include <stdbool.h>
#include "dcnode/dcnode.h"
#include "serialization_internal.h"

#define UAVCAN_EQUIPMENT_ICE_FUELTANKSTATUS_ID                      1129U
#define UAVCAN_EQUIPMENT_ICE_FUELTANKSTATUS_SIGNATURE               0x286b4a387ba84bc4ULL
#define UAVCAN_EQUIPMENT_ICE_FUELTANKSTATUS_MESSAGE_SIZE            13U

typedef struct {
    uint16_t reserved;                      // void9
    uint8_t available_fuel_volume_percent;  // uint7, percent, from 0% to 100%
    float available_fuel_volume_cm3;        // float32, centimeter^3
    float fuel_consumption_rate_cm3pm;      // float32, (centimeter^3)/minute
    float fuel_temperature;                 // float16 (set to NaN if not provided), kelvin
    uint8_t fuel_tank_id;                   // uint8, The ID of the current fuel tank.
} FuelTankStatus_t;

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t dronecan_equipment_ice_fuel_tank_status_serialize(
    const FuelTankStatus_t* const obj,
    uint8_t* const buffer)
{
    if (obj == NULL || buffer == NULL) {
        return 0;
    }

    canardEncodeScalar(buffer, 0,       9,  &obj->reserved);
    canardEncodeScalar(buffer, 9,       7,  &obj->available_fuel_volume_percent);
    canardEncodeScalar(buffer, 16,      32, &obj->available_fuel_volume_cm3);
    canardEncodeScalar(buffer, 16+32,   32, &obj->fuel_consumption_rate_cm3pm);

    uint16_t fuel_temp = canardConvertNativeFloatToFloat16(obj->fuel_temperature);
    canardEncodeScalar(buffer, 16+64,   16, &fuel_temp);

    canardEncodeScalar(buffer, 32+64,   8,  &obj->fuel_tank_id);

    return UAVCAN_EQUIPMENT_ICE_FUELTANKSTATUS_MESSAGE_SIZE;
}

#ifdef __cplusplus
}
#endif


#endif  // UAVCAN_EQUIPMENT_ICE_RECIPROCATING_FUEL_TANK_STATUS_H_
