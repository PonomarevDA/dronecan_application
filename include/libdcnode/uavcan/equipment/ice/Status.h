/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_ICE_RECIPROCATING_STATUS_H_
#define UAVCAN_EQUIPMENT_ICE_RECIPROCATING_STATUS_H_

#include <stdint.h>
#include <stdbool.h>
#include "libdcnode/dronecan.h"
#include "libdcnode/serialization_internal.h"

#define UAVCAN_EQUIPMENT_ICE_RECIPROCATING_STATUS_ID                1120
#define UAVCAN_EQUIPMENT_ICE_RECIPROCATING_STATUS_SIGNATURE         0xd38aa3ee75537ec6
#define UAVCAN_EQUIPMENT_ICE_RECIPROCATING_STATUS_MESSAGE_SIZE      (280/8)

/**
 * Abstract engine state. The flags defined below can provide further elaboration.
 * This is a required field.
 */
typedef enum {
    ICE_STATUS_STATE_STOPPED = 0,
    ICE_STATUS_STATE_STARTING,
    ICE_STATUS_STATE_RUNNING,
    ICE_STATUS_STATE_FAULT,
} IceStatusState;

#define FLAG_GENERAL_ERROR                      (1 << 0)
#define FLAG_CRANKSHAFT_SENSOR_ERROR_SUPPORTED  (1 << 1)
#define FLAG_CRANKSHAFT_SENSOR_ERROR            (1 << 2)
#define FLAG_TEMPERATURE_SUPPORTED              (1 << 3)
#define FLAG_TEMPERATURE_BELOW_NOMINAL          (1 << 4)
#define FLAG_TEMPERATURE_ABOVE_NOMINAL          (1 << 5)
#define FLAG_TEMPERATURE_OVERHEATING            (1 << 6)
#define FLAG_TEMPERATURE_EGT_ABOVE_NOMINAL      (1 << 7)
#define FLAG_FUEL_PRESSURE_SUPPORTED            (1 << 8)
#define FLAG_FUEL_PRESSURE_BELOW_NOMINAL        (1 << 9)
#define FLAG_FUEL_PRESSURE_ABOVE_NOMINAL        (1 << 10)
#define FLAG_DETONATION_SUPPORTED               (1 << 11)
#define FLAG_DETONATION_OBSERVED                (1 << 12)
#define FLAG_MISFIRE_SUPPORTED                  (1 << 13)
#define FLAG_MISFIRE_OBSERVED                   (1 << 14)
#define FLAG_OIL_PRESSURE_SUPPORTED             (1 << 15)
#define FLAG_OIL_PRESSURE_BELOW_NOMINAL         (1 << 16)
#define FLAG_OIL_PRESSURE_ABOVE_NOMINAL         (1 << 17)
#define FLAG_DEBRIS_SUPPORTED                   (1 << 18)
#define FLAG_DEBRIS_DETECTED                    (1 << 19)

#define SPARK_PLUG_SINGLE           0
#define SPARK_PLUG_FIRST_ACTIVE     1
#define SPARK_PLUG_SECOND_ACTIVE    2
#define SPARK_PLUG_BOTH_ACTIVE      3

typedef struct {
    IceStatusState state;                       // uint2    required
    uint32_t flags;                             // uint30   required
    uint16_t reserverd;                         // void16   Reserved space
    uint8_t engine_load_percent;                // uint7
    uint32_t engine_speed_rpm;                  // uint17
    float spark_dwell_time_ms;                  // float16

    float atmospheric_pressure_kpa;             // float16
    float intake_manifold_pressure_kpa;         // float16
    float intake_manifold_temperature;          // float16
    float coolant_temperature;                  // float16
    float oil_pressure;                         // float16
    float oil_temperature;                      // float16
    float fuel_pressure;                        // float16
    float fuel_consumption_rate_cm3pm;          // float32
    float estimated_consumed_fuel_volume_cm3;   // float32
    uint8_t throttle_position_percent;          // uint7
    uint8_t ecu_index;                          // uint6
    uint8_t spark_plug_usage;                   // uint3
} IceReciprocatingStatus;

#ifdef __cplusplus
extern "C" {
#endif

static inline int8_t dronecan_equipment_ice_status_serialize(
    const IceReciprocatingStatus* const obj,
    uint8_t* const buffer,
    size_t* const inout_buffer_size_bytes)
{
    if ((obj == NULL) || (buffer == NULL) || (inout_buffer_size_bytes == NULL)) {
        return -2;
    }

    const size_t capacity_bytes = *inout_buffer_size_bytes;
    if (capacity_bytes < UAVCAN_EQUIPMENT_ICE_RECIPROCATING_STATUS_MESSAGE_SIZE) {
        return -3;
    }

    uint16_t float16;

    canardEncodeScalar(buffer, 0,   2,  &obj->state);
    canardEncodeScalar(buffer, 2,   30, &obj->flags);
    canardEncodeScalar(buffer, 32,  16, &obj->reserverd);
    canardEncodeScalar(buffer, 48,  7,  &obj->engine_load_percent);
    canardEncodeScalar(buffer, 55,  17, &obj->engine_speed_rpm);

    float16 = canardConvertNativeFloatToFloat16(obj->spark_dwell_time_ms);
    canardEncodeScalar(buffer, 72,  16, &float16);

    float16 = canardConvertNativeFloatToFloat16(obj->atmospheric_pressure_kpa);
    canardEncodeScalar(buffer, 88,  16, &float16);

    float16 = canardConvertNativeFloatToFloat16(obj->intake_manifold_pressure_kpa);
    canardEncodeScalar(buffer, 104, 16, &float16);

    float16 = canardConvertNativeFloatToFloat16(obj->intake_manifold_temperature);
    canardEncodeScalar(buffer, 120, 16, &float16);

    float16 = canardConvertNativeFloatToFloat16(obj->coolant_temperature);
    canardEncodeScalar(buffer, 136, 16, &float16);

    float16 = canardConvertNativeFloatToFloat16(obj->oil_pressure);
    canardEncodeScalar(buffer, 152, 16, &float16);

    float16 = canardConvertNativeFloatToFloat16(obj->oil_temperature);
    canardEncodeScalar(buffer, 168, 16, &float16);

    float16 = canardConvertNativeFloatToFloat16(obj->fuel_pressure);
    canardEncodeScalar(buffer, 184, 16, &float16);

    canardEncodeScalar(buffer, 200, 32, &obj->fuel_consumption_rate_cm3pm);
    canardEncodeScalar(buffer, 232, 32, &obj->estimated_consumed_fuel_volume_cm3);
    canardEncodeScalar(buffer, 264, 7, &obj->throttle_position_percent);
    canardEncodeScalar(buffer, 271, 6, &obj->ecu_index);
    canardEncodeScalar(buffer, 277, 3, &obj->spark_plug_usage);
    return 0;
}

static inline int8_t dronecan_equipment_ice_status_publish(
    const IceReciprocatingStatus* const obj,
    uint8_t* inout_transfer_id)
{
    uint8_t buffer[UAVCAN_EQUIPMENT_ICE_RECIPROCATING_STATUS_MESSAGE_SIZE];
    size_t inout_buffer_size = UAVCAN_EQUIPMENT_ICE_RECIPROCATING_STATUS_MESSAGE_SIZE;
    dronecan_equipment_ice_status_serialize(obj, buffer, &inout_buffer_size);
    uavcanPublish(UAVCAN_EQUIPMENT_ICE_RECIPROCATING_STATUS_SIGNATURE,
                  UAVCAN_EQUIPMENT_ICE_RECIPROCATING_STATUS_ID,
                  inout_transfer_id,
                  CANARD_TRANSFER_PRIORITY_MEDIUM,
                  buffer,
                  UAVCAN_EQUIPMENT_ICE_RECIPROCATING_STATUS_MESSAGE_SIZE);

    return 0;
}


#ifdef __cplusplus
}
#endif


#endif  // UAVCAN_EQUIPMENT_ICE_RECIPROCATING_STATUS_H_
