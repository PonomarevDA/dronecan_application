/*
 * Copyright (C) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_ACTUATOR_STATUS_H_
#define UAVCAN_EQUIPMENT_ACTUATOR_STATUS_H_

#include "libdcnode/serialization_internal.h"
#include "ArrayCommand.h"

typedef struct {
    uint8_t actuator_id;

    float position;                 // meter or radian
    float force;                    // Newton or Newton metre
    float speed;                    // meter per second or radian per second

    uint8_t reserved;
    uint8_t power_rating_pct;       // 0 - unloaded, 100 - full load
} ActuatorStatus_t;

#ifdef __cplusplus
extern "C" {
#endif

static inline int8_t dronecan_equipment_actuator_status_serialize(
    const ActuatorStatus_t* const obj,
    uint8_t* const buffer,
    size_t* const inout_buffer_size_bytes)
{
    if ((obj == NULL) || (buffer == NULL) || (inout_buffer_size_bytes == NULL)) {
        return -2;
    }

    const size_t capacity_bytes = *inout_buffer_size_bytes;
    if (capacity_bytes < UAVCAN_EQUIPMENT_ACTUATOR_STATUS_MESSAGE_SIZE) {
        return -3;
    }

    uint16_t position = canardConvertNativeFloatToFloat16(obj->position);
    uint16_t force = canardConvertNativeFloatToFloat16(obj->force);
    uint16_t speed = canardConvertNativeFloatToFloat16(obj->speed);

    uint8_t power_rating_pct = obj->power_rating_pct;
    uint8_t reserved = obj->reserved;
    canardEncodeScalar(buffer, 0,   8,  &obj->actuator_id);
    canardEncodeScalar(buffer, 8,  16,  &position);
    canardEncodeScalar(buffer, 24,  16, &force);
    canardEncodeScalar(buffer, 40,  16, &speed);
    canardEncodeScalar(buffer, 56,  1,  &reserved);
    canardEncodeScalar(buffer, 57,  7,  &power_rating_pct);

    return 0;
}

static inline int8_t dronecan_equipment_actuator_status_publish(
    const ActuatorStatus_t* const obj,
    uint8_t* inout_transfer_id)
{
    uint8_t buffer[UAVCAN_EQUIPMENT_ACTUATOR_STATUS_MESSAGE_SIZE];
    size_t inout_buffer_size = UAVCAN_EQUIPMENT_ACTUATOR_STATUS_MESSAGE_SIZE;
    dronecan_equipment_actuator_status_serialize(obj, buffer, &inout_buffer_size);
    uavcanPublish(UAVCAN_EQUIPMENT_ACTUATOR_STATUS_SIGNATURE,
                  UAVCAN_EQUIPMENT_ACTUATOR_STATUS_ID,
                  inout_transfer_id,
                  CANARD_TRANSFER_PRIORITY_MEDIUM,
                  buffer,
                  UAVCAN_EQUIPMENT_ACTUATOR_STATUS_MESSAGE_SIZE);

    return 0;
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_ACTUATOR_STATUS_H_
