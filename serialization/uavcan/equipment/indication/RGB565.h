/*
 * Copyright (C) 2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_INDICATION_RGB565_H_
#define UAVCAN_EQUIPMENT_INDICATION_RGB565_H_

#include <stdint.h>
#include <assert.h>

/**
 * @brief Nested type.
 * RGB color in the standard 5-6-5 16-bit palette.
 * Monocolor lights should interpret this as brightness setpoint: from zero (0, 0, 0) to
 * full brightness (31, 63, 31).
 */
#pragma pack(1)
typedef struct {
    uint8_t red         : 5;
    uint8_t green       : 6;
    uint8_t blue        : 5;
} RGB565_t;
#pragma pack()
static_assert(sizeof(RGB565_t) == 2);

#endif  // UAVCAN_EQUIPMENT_INDICATION_RGB565_H_
