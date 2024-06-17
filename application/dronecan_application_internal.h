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


/**
  * @brief Weak platform specific functions.
  * A user should write an implementation by his own
  */
void uavcanRestartNode();
void uavcanReadUniqueID(uint8_t out_uid[16]);

#ifdef __cplusplus
}
#endif

#endif  // APPLICATION_DRONECAN_APPLICATION_INTERNAL_H_
