/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_POWER_BATTERY_INFO_H_
#define UAVCAN_EQUIPMENT_POWER_BATTERY_INFO_H_

#include <stdint.h>
#include <stdbool.h>
#include "libdcnode/dronecan.h"
#include "libdcnode/serialization_internal.h"

#define UAVCAN_EQUIPMENT_POWER_BATTERY_INFO_ID                      1092
#define UAVCAN_EQUIPMENT_POWER_BATTERY_INFO_SIGNATURE               0x249c26548a711966
#define UAVCAN_EQUIPMENT_POWER_BATTERY_INFO_MESSAGE_SIZE            23  // from 23 up to 55

/**
 * @brief uavcan.equipment.power.BatteryInfo
 */
typedef struct {
    float temperature;              // [Kelvin]
    float voltage;                  // [Volt]
    float current;                  // [Ampere]
    float average_power_10sec;      // [Watt]
    float remaining_capacity_wh;    // [Watt hours]
    float full_charge_capacity_wh;  // [Watt hours]
    float hours_to_full_charge;     // [Hours]
    uint16_t status_flags;
    uint8_t state_of_health_pct;
    uint8_t state_of_charge_pct;    // Percent of the full charge [0, 100]. This field is required
    uint8_t state_of_charge_pct_stdev;
    uint8_t battery_id;
    uint32_t model_instance_id;
} BatteryInfo_t;

#ifdef __cplusplus
extern "C" {
#endif

static inline int8_t dronecan_equipment_power_battery_info_serialize(
    const BatteryInfo_t* const obj,
    uint8_t* const buffer,
    size_t* const inout_buffer_size_bytes)
{
    if ((obj == NULL) || (buffer == NULL) || (inout_buffer_size_bytes == NULL)) {
        return -2;
    }

    const size_t capacity_bytes = *inout_buffer_size_bytes;
    if (capacity_bytes < UAVCAN_EQUIPMENT_POWER_BATTERY_INFO_MESSAGE_SIZE) {
        return -3;
    }

    canardEncodeFloat16(buffer, 0,  obj->temperature);
    canardEncodeFloat16(buffer, 16, obj->voltage);
    canardEncodeFloat16(buffer, 32, obj->current);
    canardEncodeFloat16(buffer, 48, obj->average_power_10sec);
    canardEncodeFloat16(buffer, 64, obj->remaining_capacity_wh);
    canardEncodeFloat16(buffer, 80, obj->full_charge_capacity_wh);
    canardEncodeFloat16(buffer, 96, obj->hours_to_full_charge);

    canardEncodeScalar(buffer, 112, 11, &obj->status_flags);
    canardEncodeScalar(buffer, 123, 7,  &obj->state_of_health_pct);
    canardEncodeScalar(buffer, 130, 7,  &obj->state_of_charge_pct);
    canardEncodeScalar(buffer, 137, 7,  &obj->state_of_charge_pct_stdev);
    canardEncodeScalar(buffer, 144, 8,  &obj->battery_id);
    canardEncodeScalar(buffer, 152, 32,  &obj->model_instance_id);

    return 0;
}

static inline int8_t dronecan_equipment_battery_info_publish(
    const BatteryInfo_t* const obj,
    uint8_t* inout_transfer_id)
{
    uint8_t buffer[UAVCAN_EQUIPMENT_POWER_BATTERY_INFO_MESSAGE_SIZE];
    size_t inout_buffer_size = UAVCAN_EQUIPMENT_POWER_BATTERY_INFO_MESSAGE_SIZE;
    dronecan_equipment_power_battery_info_serialize(obj, buffer, &inout_buffer_size);
    uavcanPublish(UAVCAN_EQUIPMENT_POWER_BATTERY_INFO_SIGNATURE,
                  UAVCAN_EQUIPMENT_POWER_BATTERY_INFO_ID,
                  inout_transfer_id,
                  CANARD_TRANSFER_PRIORITY_MEDIUM,
                  buffer,
                  UAVCAN_EQUIPMENT_POWER_BATTERY_INFO_MESSAGE_SIZE);

    return 0;
}


#ifdef __cplusplus
}
#endif


#endif  // UAVCAN_EQUIPMENT_POWER_BATTERY_INFO_H_
