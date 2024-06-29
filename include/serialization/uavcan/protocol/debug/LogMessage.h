/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_PROTOCOL_DEBUG_DEBUG_MESSAGE_H_
#define UAVCAN_PROTOCOL_DEBUG_DEBUG_MESSAGE_H_

#include <stdint.h>
#include <stdbool.h>
#include "dronecan.h"
#include "serialization_internal.h"

#define UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_ID                        16383
#define UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_SIGNATURE                 0xd654a48e0c049d75
#define UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_MESSAGE_SIZE              (983/8)

/**
 * @brief uavcan.protocol.debug.LogLevel
 * @note Log message severity.
 */
typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
} LogLevel_t;

/**
 * @brief uavcan.protocol.debug.LogMessage
 * @note  Generic log message.
 *        All items are byte aligned.
 */
typedef struct {
    LogLevel_t level;                           // uint3 value (LogLevel level)

    uint8_t source_size;                        // uint5 source_size
    uint8_t source[31];                         // uint8[<=31]

    uint8_t text_size;
    uint8_t text[90];                           // uint8[<=90]
} DebugLogMessage_t;

#ifdef __cplusplus
extern "C" {
#endif

static inline int8_t dronecan_protocol_debug_log_message_serialize(
    const DebugLogMessage_t* const obj,
    uint8_t* const buffer,
    size_t* const inout_buffer_size_bytes)
{
    if ((obj == NULL) || (buffer == NULL) || (inout_buffer_size_bytes == NULL)) {
        return -2;
    }

    const size_t capacity_bytes = *inout_buffer_size_bytes;
    if (capacity_bytes < UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_MESSAGE_SIZE) {
        return -3;
    }

    if (obj->source_size > 31 || obj->text_size > 90) {
        return -4;
    }

    canardEncodeScalar(buffer, 0,   3,  &obj->level);
    canardEncodeScalar(buffer, 3,   5,  &obj->source_size);
    memcpy(&buffer[1], obj->source, obj->source_size);
    memcpy(&buffer[obj->source_size + 1], obj->text, obj->text_size);

    return 0;
}

static inline int8_t dronecan_protocol_debug_log_message_publish(
    const DebugLogMessage_t* const obj,
    uint8_t* inout_transfer_id)
{
    uint8_t buffer[UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_MESSAGE_SIZE];
    size_t inout_buffer_size = UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_MESSAGE_SIZE;
    int8_t res = dronecan_protocol_debug_log_message_serialize(obj, buffer, &inout_buffer_size);
    if (res < 0) {
        return res;
    }

    uint8_t required_size = 1 + obj->source_size + obj->text_size;
    uavcanPublish(UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_SIGNATURE,
                  UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_ID,
                  inout_transfer_id,
                  CANARD_TRANSFER_PRIORITY_MEDIUM,
                  buffer,
                  required_size);

    return 0;
}


#ifdef __cplusplus
}
#endif


#endif  // UAVCAN_PROTOCOL_DEBUG_DEBUG_MESSAGE_H_
