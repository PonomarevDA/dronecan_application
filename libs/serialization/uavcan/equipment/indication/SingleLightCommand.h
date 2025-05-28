/*
 * Copyright (C) 2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_INDICATION_SINGLE_LIGHT_COMMAND_H_
#define UAVCAN_EQUIPMENT_INDICATION_SINGLE_LIGHT_COMMAND_H_

#include "RGB565.h"

#define LIGHT_ID_ANTI_COLLISION                 246
#define LIGHT_ID_RIGHT_OF_WAY                   247
#define LIGHT_ID_STROBE                         248
#define LIGHT_ID_LANDING                        254

/**
 * @brief Nested type.
 * Controls single light source, color or monochrome.
 */
#pragma pack(1)
typedef struct {
    uint8_t light_id;
    RGB565_t color;
} SingleLightCommand_t;
#pragma pack()
static_assert(sizeof(SingleLightCommand_t) == 3);

#endif  // UAVCAN_EQUIPMENT_INDICATION_SINGLE_LIGHT_COMMAND_H_
