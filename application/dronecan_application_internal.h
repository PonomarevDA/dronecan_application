/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef APPLICATION_DRONECAN_APPLICATION_INTERNAL_H_
#define APPLICATION_DRONECAN_APPLICATION_INTERNAL_H_

#include <stdint.h>
#include "libcanard_v0/canard.h"

#define UAVCAN_EXPAND(data_type) data_type##_SIGNATURE, data_type##_ID

#ifndef STATUS_ERROR
    #define STATUS_ERROR -1
#endif

/**
  * @brief Encapsulate everything related to a date type
  */
typedef struct {
    uint64_t sig;
    uint16_t id;
} UavcanDataType_t;

#ifdef __cplusplus
extern "C" {
#endif

uint32_t uavcanGetTimeMs();

inline float uavcanDecodeF16(const CanardRxTransfer* transfer, uint32_t bit_offset) {
    uint16_t f16_dummy;
    canardDecodeScalar(transfer, bit_offset, 16, true, &f16_dummy);
    return canardConvertFloat16ToNativeFloat(f16_dummy);
}

inline void canardEncodeFloat16(void* buffer, uint32_t bit_offset, float value) {
    uint16_t f16_dummy = canardConvertNativeFloatToFloat16(value);
    canardEncodeScalar(buffer, bit_offset, 16, &f16_dummy);
}

inline void canardEncodeFloat32(void* buffer, uint32_t bit_offset, float value) {
    canardEncodeScalar(buffer, bit_offset,  32, &value);
}

#ifdef __cplusplus
}
#endif

#endif  // APPLICATION_DRONECAN_APPLICATION_INTERNAL_H_
