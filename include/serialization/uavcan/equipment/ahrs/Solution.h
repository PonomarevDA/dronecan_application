/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_AHRS_SOLUTION_H_
#define UAVCAN_EQUIPMENT_AHRS_SOLUTION_H_

#include "dronecan.h"
#include "serialization_internal.h"

#define UAVCAN_EQUIPMENT_AHRS_SOLUTION_ID                           1000
#define UAVCAN_EQUIPMENT_AHRS_SOLUTION_SIGNATURE                    0x72a63a3c6f41fa9b
#define UAVCAN_EQUIPMENT_AHRS_SOLUTION_MESSAGE_SIZE                 29  // 668 bits
#define UAVCAN_EQUIPMENT_AHRS_SOLUTION                              UAVCAN_EXPAND(UAVCAN_EQUIPMENT_AHRS_SOLUTION)

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



#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_AHRS_SOLUTION_H_
