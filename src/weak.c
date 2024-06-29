/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <string.h>
#include "dronecan_application_internal.h"

__attribute__((weak)) void uavcanRestartNode() {
    asm("NOP");
}

__attribute__((weak)) void uavcanReadUniqueID(uint8_t out_uid[16]) {
    memset(out_uid, 0x00, 16);
}
