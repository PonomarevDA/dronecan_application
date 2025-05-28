/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_AHRS_SOLUTION_H_
#define UAVCAN_EQUIPMENT_AHRS_SOLUTION_H_

#include "dcnode/dcnode.h"
#include "serialization_internal.h"

#define UAVCAN_EQUIPMENT_AHRS_SOLUTION_ID                           1000U
#define UAVCAN_EQUIPMENT_AHRS_SOLUTION_SIGNATURE                    0x72a63a3c6f41fa9bULL
#define UAVCAN_EQUIPMENT_AHRS_SOLUTION_MESSAGE_SIZE                 29U
#define UAVCAN_EQUIPMENT_AHRS_SOLUTION                              UAVCAN_EXPAND(UAVCAN_EQUIPMENT_AHRS_SOLUTION)
#define UAVCAN_EQUIPMENT_AHRS_SOLUTION UAVCAN_EXPAND(UAVCAN_EQUIPMENT_AHRS_SOLUTION)

/**
 * @brief uavcan.equipment.ahrs.Solution
 * @note Inertial data and orientation in body frame.
 */
typedef struct {
    uint64_t timestamp;             // uint56 usec, normalized quaternion

    float orientation_xyzw[4];      // float16
    // void4
    // float16[<=9] orientation_covariance

    float angular_velocity[3];      // float16, rad/sec
    // void4
    // float16[<=9] angular_velocity_covariance

    float linear_acceleration[3];   // float16, m/s^2
    // float16[<=9] linear_acceleration_covariance
} AhrsSolution_t;

#ifdef __cplusplus
extern "C" {
#endif


static inline int8_t dronecan_equipment_ahrs_solution_deserialize(
    const CanardRxTransfer* transfer, AhrsSolution_t* obj) {
    if ((transfer == NULL) || (obj == NULL)) {
        return -2;
    }
    size_t offset = 0;
    canardDecodeScalar(transfer, 0, 56, false, &obj->timestamp);
    offset += 56;
    uint16_t f16_dummy;

    for (uint_fast8_t idx = 0; idx < 4; idx++) {
        canardDecodeScalar(transfer, offset, 16, true, &f16_dummy);
        offset += 16;
        obj->orientation_xyzw[idx] = canardConvertFloat16ToNativeFloat(f16_dummy);
    }

    offset+= 4;  // reserved void4

    uint8_t covariance_len;
    canardDecodeScalar(transfer, offset, 4, true, &covariance_len);
    offset += 4;
    offset += 16 * covariance_len;

    for (uint_fast8_t idx = 0; idx < 3; idx++) {
        canardDecodeScalar(transfer, offset, 16, true, &f16_dummy);
        offset += 16;
        obj->angular_velocity[idx] = canardConvertFloat16ToNativeFloat(f16_dummy);
    }

    offset+= 4;  // reserved void4
    canardDecodeScalar(transfer, offset, 4, true, &covariance_len);
    offset += 4;

    offset += 16 * covariance_len;

    for (uint_fast8_t idx = 0; idx < 3; idx++) {
        canardDecodeScalar(transfer, offset, 16, true, &f16_dummy);
        offset += 16;
        obj->linear_acceleration[idx] = canardConvertFloat16ToNativeFloat(f16_dummy);
    }

    return 0;
}


static inline uint32_t dronecan_equipment_ahrs_solution_serialize(
    const AhrsSolution_t* const obj,
    uint8_t* const buffer)
{
    if (obj == NULL || buffer == NULL) {
        return 0;
    }

    size_t offset = 0;

    // uavcan.Timestamp timestamp
    canardEncodeScalar(buffer, 0,  56,  &obj->timestamp);
    offset += 56;

    // orientation_xyzw
    for (uint_fast8_t idx = 0; idx < 4; idx++) {
        canardEncodeFloat16(buffer, offset, obj->orientation_xyzw[idx]);
        offset += 16;
    }

    // void4
    offset += 4;

    // orientation_covariance
    uint8_t orientation_covariance_len = 0;
    canardEncodeScalar(buffer, offset, 4, &orientation_covariance_len);
    offset += 4;

    // angular_velocity
    for (uint_fast8_t idx = 0; idx < 3; idx++) {
        canardEncodeFloat16(buffer, offset, obj->angular_velocity[idx]);
        offset += 16;
    }

    // void4
    offset += 4;

    // angular_velocity_covariance
    uint8_t angular_velocity_covariance_len = 0;
    canardEncodeScalar(buffer, offset, 4, &angular_velocity_covariance_len);
    offset += 4;

    // linear_acceleration
    for (uint_fast8_t idx = 0; idx < 3; idx++) {
        canardEncodeFloat16(buffer, offset, obj->linear_acceleration[idx]);
        offset += 16;
    }

    // linear_acceleration_covariance
    uint8_t linear_acceleration_covariance_len = 0;
    canardEncodeScalar(buffer, offset, 4, &linear_acceleration_covariance_len);
    offset += 4;

    return ((offset + 7) / 8);
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_AHRS_SOLUTION_H_
