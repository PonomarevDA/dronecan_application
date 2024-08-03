/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_GNSS_FIX2_H_
#define UAVCAN_EQUIPMENT_GNSS_FIX2_H_

#include "dronecan.h"
#include "serialization_internal.h"

#define UAVCAN_EQUIPMENT_GNSS_FIX2_ID                               1063
#define UAVCAN_EQUIPMENT_GNSS_FIX2_SIGNATURE                        0xca41e7000f37435f
#define UAVCAN_EQUIPMENT_GNSS_FIX2_MESSAGE_SIZE                     62
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
} GnssFix2;

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_GNSS_FIX2_H_
