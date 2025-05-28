/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_AHRS_RAW_IMU_H_
#define UAVCAN_EQUIPMENT_AHRS_RAW_IMU_H_

#include "dcnode/dcnode.h"
#include "serialization_internal.h"

#define UAVCAN_EQUIPMENT_AHRS_RAW_IMU_ID                1003U
#define UAVCAN_EQUIPMENT_AHRS_RAW_IMU_SIGNATURE         (0x8280632C40E574B5ULL)
#define UAVCAN_EQUIPMENT_AHRS_RAW_IMU_MESSAGE_SIZE      47U
#define UAVCAN_EQUIPMENT_AHRS_RAW_IMU                   UAVCAN_EXPAND(UAVCAN_EQUIPMENT_AHRS_RAW_IMU)

/**
 * @brief uavcan.equipment.ahrs.RawImu
 * @note Inertial data and orientation in body frame.
 */
typedef struct {
    uint64_t timestamp;                 // uint56 usec
    float integration_interval;         // float32
    float rate_gyro_latest[3];          // float16
    float rate_gyro_integral[3];        // float32
    float accelerometer_latest[3];      // float16
    float accelerometer_integral[3];    // float32
} AhrsRawImu;

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t dronecan_equipment_ahrs_raw_imu_serialize(
    const AhrsRawImu* const obj,
    uint8_t* const buffer)
{
    if (obj == NULL || buffer == NULL) {
        return 0;
    }

    size_t offset = 0;
    canardEncodeScalar(buffer, 0,  56,  &obj->timestamp);
    offset += 56;

    canardEncodeFloat32(buffer, offset, obj->integration_interval);
    offset += 32;

    for (uint_fast8_t idx = 0; idx < 3; idx++) {
        canardEncodeFloat16(buffer, offset, obj->rate_gyro_latest[idx]);
        offset += 16;
    }
    for (uint_fast8_t idx = 0; idx < 3; idx++) {
        canardEncodeFloat32(buffer, offset, obj->rate_gyro_integral[idx]);
        offset += 32;
    }

    for (uint_fast8_t idx = 0; idx < 3; idx++) {
        canardEncodeFloat16(buffer, offset, obj->accelerometer_latest[idx]);
        offset += 16;
    }
    for (uint_fast8_t idx = 0; idx < 3; idx++) {
        canardEncodeFloat32(buffer, offset, obj->accelerometer_integral[idx]);
        offset += 32;
    }

    return UAVCAN_EQUIPMENT_AHRS_RAW_IMU_MESSAGE_SIZE;
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_AHRS_RAW_IMU_H_
