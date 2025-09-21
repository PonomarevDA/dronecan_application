/*
 * Copyright (C) 2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef DRONECAN_SENSORS_HYGROMETER_HYGROMETER_H_
#define DRONECAN_SENSORS_HYGROMETER_HYGROMETER_H_

#include "libdcnode/serialization_internal.h"
#include "libdcnode/dronecan.h"

#define DRONECAN_SENSORS_HYGROMETER_HYGROMETER_ID               1032
#define DRONECAN_SENSORS_HYGROMETER_HYGROMETER_SIGNATURE        0xCEB308892BF163E8ULL
#define DRONECAN_SENSORS_HYGROMETER_HYGROMETER_MESSAGE_SIZE     5
#define DRONECAN_SENSORS_HYGROMETER_HYGROMETER                  UAVCAN_EXPAND(DRONECAN_SENSORS_HYGROMETER_HYGROMETER)

typedef struct {
    float temperature;
    float humidity;
    uint8_t id;
} Hygrometer;

#ifdef __cplusplus
extern "C" {
#endif

static inline int8_t dronecan_sensors_hygrometer_hygrometer_serialize(
    const Hygrometer* const obj,
    uint8_t* const buffer,
    size_t* const inout_buffer_size_bytes)
{
    if ((obj == NULL) || (buffer == NULL) || (inout_buffer_size_bytes == NULL)) {
        return -2;
    }

    const size_t capacity_bytes = *inout_buffer_size_bytes;
    if (capacity_bytes < DRONECAN_SENSORS_HYGROMETER_HYGROMETER_MESSAGE_SIZE) {
        return -3;
    }

    uint16_t temperature = canardConvertNativeFloatToFloat16(obj->temperature);
    canardEncodeScalar(buffer, 0, 16, &temperature);

    uint16_t humidity = canardConvertNativeFloatToFloat16(obj->humidity);
    canardEncodeScalar(buffer, 16, 16, &humidity);

    canardEncodeScalar(buffer, 32, 8, &obj->id);

    return 0;
}

static inline int8_t dronecan_sensors_hygrometer_hygrometer_publish(
    const Hygrometer* const obj,
    uint8_t* inout_transfer_id)
{
    uint8_t buffer[DRONECAN_SENSORS_HYGROMETER_HYGROMETER_MESSAGE_SIZE];
    size_t inout_buffer_size = DRONECAN_SENSORS_HYGROMETER_HYGROMETER_MESSAGE_SIZE;
    dronecan_sensors_hygrometer_hygrometer_serialize(obj, buffer, &inout_buffer_size);
    uavcanPublish(DRONECAN_SENSORS_HYGROMETER_HYGROMETER_SIGNATURE,
                  DRONECAN_SENSORS_HYGROMETER_HYGROMETER_ID,
                  inout_transfer_id,
                  CANARD_TRANSFER_PRIORITY_MEDIUM,
                  buffer,
                  DRONECAN_SENSORS_HYGROMETER_HYGROMETER_MESSAGE_SIZE);

    return 0;
}

#ifdef __cplusplus
}
#endif

#endif  // DRONECAN_SENSORS_HYGROMETER_HYGROMETER_H_
