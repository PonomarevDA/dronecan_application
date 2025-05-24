/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <iostream>
#include <string.h>
#include "dcnode/dcnode.h"

void platformSpecificReadUniqueID(uint8_t out_uid[4]) {
    const uint32_t UNIQUE_ID_16_BYTES[4] = {
        HAL_GetUIDw0(),
        HAL_GetUIDw1(),
        HAL_GetUIDw2(),
        0
    };
    memset(out_uid, UNIQUE_ID_16_BYTES, 16);
}

bool platformSpecificRequestRestart() {
    return false;
}

uint32_t platformSpecificGetTimeMs() {
    return HAL_GetTick();
}
