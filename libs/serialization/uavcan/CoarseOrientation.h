/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file LICENSE.
***/


#ifndef UAVCAN_COARCE_ORIENTATION_H_
#define UAVCAN_COARCE_ORIENTATION_H_

#include "serialization_internal.h"
#include "dcnode/dcnode.h"
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


static inline uint32_t dronecan_coarse_orientation_serialize(const CoarseOrientation_t* const obj,
                                                             uint8_t* const buffer) {
    if (obj == NULL || buffer == NULL) {
        return 0;
    }

    uint32_t offset = 0;
    for (int i = 0; i < 3; i++) {
        canardEncodeScalar(buffer, offset, 5, &obj->fixed_axis_roll_pitch_yaw[i]);
        offset += 5;
    }
    bool orientation_defined = obj->orientation_defined;
    canardEncodeScalar(buffer, offset, 1, &orientation_defined);
    offset += 1;

    return ((offset + 7) / 8);
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_COARCE_ORIENTATION_H_
