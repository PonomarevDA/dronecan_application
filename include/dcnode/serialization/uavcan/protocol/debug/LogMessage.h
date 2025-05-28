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
#include <cstring>
#include "dcnode/dcnode.h"
#include "serialization_internal.h"

#define UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_ID                        16383U
#define UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_SIGNATURE                 0xd654a48e0c049d75ULL
#define UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_MESSAGE_SIZE              122U

#define UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_MAX_SOURCE_LEN            31U
#define UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_MAX_TEXT_LEN              90U

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
    LogLevel_t level;                                                   // uint3 value

    uint8_t source_size;                                                // uint5 source_size
    uint8_t source[UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_MAX_SOURCE_LEN];   // uint8[<=31]

    uint8_t text_size;
    uint8_t text[UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_MAX_TEXT_LEN];       // uint8[<=90]
} DebugLogMessage_t;

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t dronecan_protocol_debug_log_message_serialize(
    const DebugLogMessage_t* const obj,
    uint8_t* const buffer)
{
    if (obj == NULL || buffer == NULL) {
        return 0;
    }

    if (obj->source_size > UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_MAX_SOURCE_LEN ||
            obj->text_size > UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_MAX_TEXT_LEN) {
        return 0;
    }

    canardEncodeScalar(buffer, 0,   3,  &obj->level);
    canardEncodeScalar(buffer, 3,   5,  &obj->source_size);
    memcpy(&buffer[1], obj->source, obj->source_size);
    memcpy(&buffer[obj->source_size + 1], obj->text, obj->text_size);

    return 1 + obj->source_size + obj->text_size;
}

static inline int8_t dronecan_protocol_debug_log_message_set_severity_level(
    DebugLogMessage_t* msg,
    uint8_t severity)
{
    if (msg == nullptr || severity > LOG_LEVEL_ERROR) {
        return -1;
    }

    msg->level = (LogLevel_t)severity;
    return 0;
}

static inline int8_t dronecan_protocol_debug_log_message_set_source(
    DebugLogMessage_t* msg,
    const char* source)
{
    if (msg == nullptr || source == nullptr) {
        return -1;
    }

    msg->source_size = strnlen(source, UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_MAX_SOURCE_LEN);
    msg->source[UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_MAX_SOURCE_LEN - 1] = '\0';
    memcpy(msg->source, source,  msg->source_size);
    return 0;
}

static inline int8_t dronecan_protocol_debug_log_message_set_text(
    DebugLogMessage_t* msg,
    const char* text)
{
    if (msg == nullptr || text == nullptr) {
        return -1;
    }

    msg->text_size = strnlen(text, UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_MAX_TEXT_LEN);
    msg->text[UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_MAX_TEXT_LEN - 1] = '\0';
    memcpy(msg->text, text,  msg->text_size);
    return 0;
}

static inline int8_t dronecan_protocol_debug_log_message_publish(
    const DebugLogMessage_t* const obj,
    uint8_t* inout_transfer_id)
{
    uint8_t buffer[UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_MESSAGE_SIZE];
    uint32_t required_size = dronecan_protocol_debug_log_message_serialize(obj, buffer);
    if (required_size == 0) {
        return -1;
    }

    DronecanNode::publish(UAVCAN_PROTOCOL_DEBUG_LOG_MESSAGE_SIGNATURE,
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
