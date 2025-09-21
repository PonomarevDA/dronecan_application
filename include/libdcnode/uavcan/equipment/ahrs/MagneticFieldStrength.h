/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH_H_
#define UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH_H_

#include "libdcnode/dronecan.h"
#include "libdcnode/serialization_internal.h"

#define UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH_ID            1001
#define UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH_SIGNATURE     0xb6ac0c442430297e
#define UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH_MESSAGE_SIZE  6  // 14+
#define UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH   UAVCAN_EXPAND(UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH)

/**
 * @brief uavcan.equipment.ahrs.MagneticFieldStrength
 * @note Magnetic field readings, in Gauss, in body frame.
 * SI units are avoided because of float16 range limitations.
 * This message is deprecated. Use the newer 1002.MagneticFieldStrength2.uavcan message
 */
typedef struct {
    float magnetic_field_ga[3];     // float16[3]
    // float16[<=9] magnetic_field_covariance
} MagneticFieldStrength;

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH_H_
