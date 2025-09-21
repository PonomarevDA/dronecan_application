/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_POWER_CIRCUIT_STATUS_H_
#define UAVCAN_EQUIPMENT_POWER_CIRCUIT_STATUS_H_

#include <stdbool.h>
#include "libdcnode/serialization_internal.h"

#define UAVCAN_EQUIPMENT_POWER_CIRCUIT_STATUS_ID                    1091
#define UAVCAN_EQUIPMENT_POWER_CIRCUIT_STATUS_SIGNATURE             0x8313d33d0ddda115
#define UAVCAN_EQUIPMENT_POWER_CIRCUIT_STATUS_MESSAGE_SIZE          7  // 56 bits

typedef enum {
    ERROR_FLAG_CLEAR = 0,
    ERROR_FLAG_OVERVOLTAGE = 1,
    ERROR_FLAG_UNDERVOLTAGE = 2,
    ERROR_FLAG_OVERCURRENT = 4,
    ERROR_FLAG_UNDERCURRENT = 8,
} CircuitStatusErrorFlags_t;

#define CIRCUIT_STATUS_BOTH_VOLTAGE_FLAGS   (ERROR_FLAG_UNDERVOLTAGE + ERROR_FLAG_OVERVOLTAGE)
#define CLEAR_VOLTAGE_FLAGS(flags)          (flags &= (0xFFFF - CIRCUIT_STATUS_BOTH_VOLTAGE_FLAGS))

typedef struct {
    uint16_t circuit_id;                    // uint16
    float voltage;                          // float16
    float current;                          // float16
    CircuitStatusErrorFlags_t error_flags;  // uint8
} CircuitStatus_t;

#ifdef __cplusplus
extern "C" {
#endif

static inline int8_t dronecan_equipment_power_circuit_status_serialize(
    const CircuitStatus_t* const obj,
    uint8_t* const buffer,
    size_t* const inout_buffer_size_bytes)
{
    if ((obj == NULL) || (buffer == NULL) || (inout_buffer_size_bytes == NULL)) {
        return -2;
    }

    const size_t capacity_bytes = *inout_buffer_size_bytes;
    if (capacity_bytes < UAVCAN_EQUIPMENT_POWER_CIRCUIT_STATUS_MESSAGE_SIZE) {
        return -3;
    }

    canardEncodeScalar(buffer, 0,  16,  &obj->circuit_id);
    canardEncodeFloat16(buffer, 16,     obj->voltage);
    canardEncodeFloat16(buffer, 32,     obj->current);
    canardEncodeScalar(buffer, 48, 8,   &obj->error_flags);

    return 0;
}

static inline int8_t dronecan_equipment_circuit_status_publish(
    const CircuitStatus_t* const obj,
    uint8_t* inout_transfer_id)
{
    uint8_t buffer[UAVCAN_EQUIPMENT_POWER_CIRCUIT_STATUS_MESSAGE_SIZE];
    size_t inout_buffer_size = UAVCAN_EQUIPMENT_POWER_CIRCUIT_STATUS_MESSAGE_SIZE;
    dronecan_equipment_power_circuit_status_serialize(obj, buffer, &inout_buffer_size);
    uavcanPublish(UAVCAN_EQUIPMENT_POWER_CIRCUIT_STATUS_SIGNATURE,
                  UAVCAN_EQUIPMENT_POWER_CIRCUIT_STATUS_ID,
                  inout_transfer_id,
                  CANARD_TRANSFER_PRIORITY_MEDIUM,
                  buffer,
                  UAVCAN_EQUIPMENT_POWER_CIRCUIT_STATUS_MESSAGE_SIZE);

    return 0;
}


#ifdef __cplusplus
}
#endif


#endif  // UAVCAN_EQUIPMENT_POWER_CIRCUIT_STATUS_H_
