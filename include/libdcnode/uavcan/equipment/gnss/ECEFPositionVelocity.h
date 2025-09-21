/*
 * Copyright (C) 2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_GNSS_ECEFPOSITIONVELOCITY_H_
#define UAVCAN_EQUIPMENT_GNSS_ECEFPOSITIONVELOCITY_H_

#include "libdcnode/dronecan.h"
#include "libdcnode/serialization_internal.h"

#define UAVCAN_EQUIPMENT_GNSS_ECEF_MESSAGE_SIZE                     27  // (32*3+36*3+6)/8 + 1

typedef struct {
    float velocity_xyz[3];          // float32[3]
    int64_t position_xyz_mm[3];     // int36[3]

    // void6
    // float covariance[36];
} ECEFPositionVelocity;

#ifdef __cplusplus
extern "C" {
#endif

static inline int32_t dronecan_equipment_gnss_ecef_serialize(
    uint8_t* const buffer,
    const ECEFPositionVelocity* const obj)
{
    if ((obj == NULL) || (buffer == NULL)) {
        return -2;
    }

    uint32_t offset = 0;

    for (size_t i=0; i < 3; i++) {
        canardEncodeFloat32(buffer, offset, obj->velocity_xyz[i]);
        offset += 32;
    }
    for (size_t i=0; i < 3; i++) {
        canardEncodeScalar(buffer, offset, 36, &obj->position_xyz_mm[i]);
        offset += 36;
    }

    // Aligns the following array at byte boundary
    uint8_t void6 = 6;
    canardEncodeScalar(buffer, offset, 6,  &void6);
    offset += 6;

    // Aligns the following array at byte boundary
    uint8_t covariance_len = 0;
    canardEncodeScalar(buffer, offset, 6,  &covariance_len);
    offset += 6;

    return offset;  ///< should be 216 bit (or 27 bytes)
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_GNSS_ECEFPOSITIONVELOCITY_H_
