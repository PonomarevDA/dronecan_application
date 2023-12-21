/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef SERIALZIATION_SERIALZIATION_INTERNAL_H_
#define SERIALZIATION_SERIALZIATION_INTERNAL_H_

#include <stdint.h>
#include "libcanard_v0/canard.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief For serialization
  */
static inline float uavcanDecodeF16(const CanardRxTransfer* transfer, uint32_t bit_offset) {
    uint16_t f16_dummy;
    canardDecodeScalar(transfer, bit_offset, 16, true, &f16_dummy);
    return canardConvertFloat16ToNativeFloat(f16_dummy);
}

static inline void canardEncodeFloat16(void* buffer, uint32_t bit_offset, float value) {
    uint16_t f16_dummy = canardConvertNativeFloatToFloat16(value);
    canardEncodeScalar(buffer, bit_offset, 16, &f16_dummy);
}

static inline void canardEncodeFloat32(void* buffer, uint32_t bit_offset, float value) {
    canardEncodeScalar(buffer, bit_offset,  32, &value);
}

#ifdef __cplusplus
}
#endif

#endif  // SERIALZIATION_SERIALZIATION_INTERNAL_H_
