/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_GNSS_FIX2_H_
#define UAVCAN_EQUIPMENT_GNSS_FIX2_H_

#include "dcnode/dcnode.h"
#include "serialization_internal.h"
#include "ECEFPositionVelocity.h"

#define UAVCAN_EQUIPMENT_GNSS_FIX2_ID                               1063U
#define UAVCAN_EQUIPMENT_GNSS_FIX2_SIGNATURE                        0xca41e7000f37435fULL
#define UAVCAN_EQUIPMENT_GNSS_FIX2_MESSAGE_SIZE                     89U
#define UAVCAN_EQUIPMENT_GNSS_FIX2                                  UAVCAN_EXPAND(UAVCAN_EQUIPMENT_GNSS_FIX2)

typedef enum {
    STATUS_NO_FIX = 0,
    STATUS_TIME_ONLY = 1,
    STATUS_2D_FIX = 2,
    STATUS_3D_FIX = 3,
} GnssFix2_status;

typedef struct {
    uint64_t timestamp;
    uint64_t gnss_timestamp;
    uint8_t gnss_time_standard;
    uint8_t num_leap_seconds;
    int64_t longitude_deg_1e8;
    int64_t latitude_deg_1e8;
    int32_t height_ellipsoid_mm;
    int32_t height_msl_mm;
    float ned_velocity[3];
    uint8_t sats_used;
    GnssFix2_status status;

    uint8_t mode;
    uint8_t sub_mode;
    uint8_t covariance_len;
    float covariance[36];

    float pdop;

    uint8_t ecef_size;
    ECEFPositionVelocity ecef;
} GnssFix2;

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t dronecan_equipment_gnss_fix2_serialize(
    const GnssFix2* const obj,
    uint8_t* const buffer)
{
    if (obj == NULL || buffer == NULL) {
        return 0;
    }

    uint32_t offset = 0;

    canardEncodeScalar(buffer, offset,  56, &obj->timestamp);
    offset += 56;
    canardEncodeScalar(buffer, offset,  56, &obj->gnss_timestamp);
    offset += 56;
    canardEncodeScalar(buffer, offset,  3,  &obj->gnss_time_standard);
    offset += 3;

    // void13   # Reserved space
    offset += 13;

    canardEncodeScalar(buffer, offset, 8,  &obj->num_leap_seconds);
    offset += 8;

    canardEncodeScalar(buffer, offset, 37, &obj->longitude_deg_1e8);
    offset += 37;
    canardEncodeScalar(buffer, offset, 37, &obj->latitude_deg_1e8);
    offset += 37;
    canardEncodeScalar(buffer, offset, 27, &obj->height_ellipsoid_mm);
    offset += 27;
    canardEncodeScalar(buffer, offset, 27, &obj->height_msl_mm);
    offset += 27;

    canardEncodeFloat32(buffer, offset, obj->ned_velocity[0]);
    offset += 32;
    canardEncodeFloat32(buffer, offset, obj->ned_velocity[1]);
    offset += 32;
    canardEncodeFloat32(buffer, offset, obj->ned_velocity[2]);
    offset += 32;

    canardEncodeScalar(buffer, offset, 6,  &obj->sats_used);
    offset += 6;
    canardEncodeScalar(buffer, offset, 2,  &obj->status);
    offset += 2;
    canardEncodeScalar(buffer, offset, 4,  &obj->mode);
    offset += 4;
    canardEncodeScalar(buffer, offset, 6,  &obj->sub_mode);
    offset += 6;

    uint8_t covariance_len = 6;
    canardEncodeScalar(buffer, offset, 6,  &covariance_len);
    offset += 6;
    for (uint_fast8_t idx = 0; idx < 6; idx++) {
        canardEncodeFloat16(buffer, offset, obj->covariance[idx]);
        offset += 16;
    }

    canardEncodeFloat16(buffer, offset, obj->pdop);
    offset += 16;

    if (obj->ecef_size == 1) {
        offset += 8 * dronecan_equipment_gnss_ecef_serialize(buffer + offset / 8, &obj->ecef);
    }

    return ((offset + 7) / 8);  // either 496 bits (62 bytes) or 496+216 bits (89 bytes)
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_GNSS_FIX2_H_
