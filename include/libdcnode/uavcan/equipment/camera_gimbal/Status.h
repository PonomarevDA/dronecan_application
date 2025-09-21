/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_CAMERA_GIMBAL_STATUS_H_
#define UAVCAN_EQUIPMENT_CAMERA_GIMBAL_STATUS_H_

#include "libdcnode/dronecan.h"
#include "libdcnode/serialization_internal.h"

#define UAVCAN_EQUIPMENT_CAMERA_GIMBAL_STATUS_ID                1044
#define UAVCAN_EQUIPMENT_CAMERA_GIMBAL_STATUS_SIGNATURE         0xb9f127865be0d61e
#define UAVCAN_EQUIPMENT_CAMERA_GIMBAL_STATUS_MESSAGE_SIZE      10  // 29=228/8
#define UAVCAN_EQUIPMENT_CAMERA_GIMBAL_STATUS                   UAVCAN_EXPAND(UAVCAN_EQUIPMENT_CAMERA_GIMBAL_STATUS)

typedef struct {
    uint8_t gimbal_id;
    uint8_t mode;
    float camera_orientation_in_body_frame_xyzw[4];
} CameraGimbalStatus;

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_CAMERA_GIMBAL_STATUS_H_
