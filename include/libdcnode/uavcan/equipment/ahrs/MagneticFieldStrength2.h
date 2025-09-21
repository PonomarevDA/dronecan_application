/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH2_H_
#define UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH2_H_

#include "libdcnode/dronecan.h"
#include "libdcnode/serialization_internal.h"

#define UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH2_ID            1002
#define UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH2_SIGNATURE     0xB6AC0C442430297EULL
#define UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH2_MESSAGE_SIZE  7
#define UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH2   UAVCAN_EXPAND(UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH2)

/**
 * @brief uavcan.equipment.ahrs.MagneticFieldStrength2
 * @note Magnetic field readings, in Gauss, in body frame.
 * SI units are avoided because of float16 range limitations.
 */
typedef struct {
    uint8_t sensor_id;
    float magnetic_field_ga[3];     // float16[3]
    // float16[<=9] magnetic_field_covariance
} MagneticFieldStrength2;

#ifdef __cplusplus
extern "C" {
#endif

static inline int8_t dronecan_equipment_ahrs_magnetic_field_2_serialize(
    const MagneticFieldStrength2* const obj,
    uint8_t* const buffer,
    size_t* const inout_buffer_size_bytes)
{
    if ((obj == NULL) || (buffer == NULL) || (inout_buffer_size_bytes == NULL)) {
        return -2;
    }

    const size_t capacity_bytes = *inout_buffer_size_bytes;
    if (capacity_bytes < UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH2_MESSAGE_SIZE) {
        return -3;
    }

    size_t offset = 0;
    canardEncodeScalar(buffer, 0,  8,  &obj->sensor_id);
    offset += 8;

    for (uint_fast8_t idx = 0; idx < 3; idx++) {
        canardEncodeFloat16(buffer, offset, obj->magnetic_field_ga[idx]);
        offset += 16;
    }

    return 0;
}

static inline int8_t dronecan_equipment_ahrs_magnetic_field_2_publish(
    const MagneticFieldStrength2* const obj,
    uint8_t* inout_transfer_id)
{
    uint8_t buffer[UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH2_MESSAGE_SIZE];
    size_t inout_buffer_size = UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH2_MESSAGE_SIZE;
    dronecan_equipment_ahrs_magnetic_field_2_serialize(obj, buffer, &inout_buffer_size);
    uavcanPublish(UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH2_SIGNATURE,
                  UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH2_ID,
                  inout_transfer_id,
                  CANARD_TRANSFER_PRIORITY_MEDIUM,
                  buffer,
                  UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH2_MESSAGE_SIZE);

    return 0;
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH2_H_
