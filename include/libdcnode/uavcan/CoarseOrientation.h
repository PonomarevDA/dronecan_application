/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file LICENSE.
***/


#ifndef UAVCAN_COARCE_ORIENTATION_H_
#define UAVCAN_COARCE_ORIENTATION_H_

#include "libdcnode/serialization_internal.h"
#include "libdcnode/dronecan.h"
#define UAVCAN_COARSE_ORIENTATION_MESSAGE_SIZE         2

/**
 * @brief       uavcan.CoarseOrientation beam_orientation_in_body_frame
 * @details     Nested type.
 *              Coarse, low-resolution 3D orientation represented as fixed axes in 16 bit.
 *              Roll, pitch, yaw angles in radians should be multiplied by
 *              ANGLE_MULTIPLIER in order to convert them to the coarse representation.
 *              ANGLE_MULTIPLIER = NORM / PI
 *              Where NORM is 12, because it:
 *               - Fits the maximum range of a signed 5 bit integer
 *               - Allows to exactly represent the following angles:
 *                 0, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180, and negatives
 */
typedef struct {
    int8_t fixed_axis_roll_pitch_yaw [3];
    bool orientation_defined;
} CoarseOrientation_t;

#ifdef __cplusplus
extern "C" {
#endif


static inline int8_t dronecan_coarse_orientation_serialize(
    const CoarseOrientation_t* const obj, uint8_t* const buffer,
    size_t* const inout_buffer_size_bytes, uint8_t buffer_offset) {
        if ((obj == NULL) || (buffer == NULL) ||
        (inout_buffer_size_bytes == NULL)) {
        return -2;
    }

    const size_t capacity_bytes = *inout_buffer_size_bytes - buffer_offset;
    if (capacity_bytes < UAVCAN_COARSE_ORIENTATION_MESSAGE_SIZE) {
        return -3;
    }
    int8_t size = 0;
    for (int i = 0; i < 3; i++) {
        canardEncodeScalar(buffer, buffer_offset, 5, &obj->fixed_axis_roll_pitch_yaw[i]);
        buffer_offset += 5;
        size += 5;
    }
    bool orientation_defined = obj->orientation_defined;
    canardEncodeScalar(buffer, buffer_offset, 1, &orientation_defined);
    buffer_offset += 1;
    return buffer_offset;
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_COARCE_ORIENTATION_H_
