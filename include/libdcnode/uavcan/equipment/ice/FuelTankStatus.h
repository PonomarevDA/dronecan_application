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
#include "libdcnode/dronecan.h"
#include "libdcnode/serialization_internal.h"

#define UAVCAN_EQUIPMENT_ICE_FUELTANK_STATUS_ID                     1129
#define UAVCAN_EQUIPMENT_ICE_FUELTANK_STATUS_SIGNATURE              0x286b4a387ba84bc4
#define UAVCAN_EQUIPMENT_ICE_FUELTANK_STATUS_MESSAGE_SIZE           13

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

static inline int8_t dronecan_equipment_ice_fuel_tank_status_serialize(
    const FuelTankStatus_t* const obj,
    uint8_t* const buffer,
    size_t* const inout_buffer_size_bytes)
{
    if ((obj == NULL) || (buffer == NULL) || (inout_buffer_size_bytes == NULL)) {
        return -2;
    }

    const size_t capacity_bytes = *inout_buffer_size_bytes;
    if (capacity_bytes < UAVCAN_EQUIPMENT_ICE_FUELTANK_STATUS_MESSAGE_SIZE) {
        return -3;
    }

    canardEncodeScalar(buffer, 0,       9,  &obj->reserved);
    canardEncodeScalar(buffer, 9,       7,  &obj->available_fuel_volume_percent);
    canardEncodeScalar(buffer, 16,      32, &obj->available_fuel_volume_cm3);
    canardEncodeScalar(buffer, 16+32,   32, &obj->fuel_consumption_rate_cm3pm);

    uint16_t fuel_temp = canardConvertNativeFloatToFloat16(obj->fuel_temperature);
    canardEncodeScalar(buffer, 16+64,   16, &fuel_temp);

    canardEncodeScalar(buffer, 32+64,   8,  &obj->fuel_tank_id);

    return 0;
}

static inline int8_t dronecan_equipment_ice_fuel_tank_status_publish(
    const FuelTankStatus_t* const obj,
    uint8_t* inout_transfer_id)
{
    uint8_t buffer[UAVCAN_EQUIPMENT_ICE_FUELTANK_STATUS_MESSAGE_SIZE];
    size_t inout_buffer_size = UAVCAN_EQUIPMENT_ICE_FUELTANK_STATUS_MESSAGE_SIZE;
    dronecan_equipment_ice_fuel_tank_status_serialize(obj, buffer, &inout_buffer_size);
    uavcanPublish(UAVCAN_EQUIPMENT_ICE_FUELTANK_STATUS_SIGNATURE,
                  UAVCAN_EQUIPMENT_ICE_FUELTANK_STATUS_ID,
                  inout_transfer_id,
                  CANARD_TRANSFER_PRIORITY_MEDIUM,
                  buffer,
                  UAVCAN_EQUIPMENT_ICE_FUELTANK_STATUS_MESSAGE_SIZE);

    return 0;
}


#ifdef __cplusplus
}
#endif


#endif  // UAVCAN_EQUIPMENT_ICE_RECIPROCATING_FUEL_TANK_STATUS_H_
