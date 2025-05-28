/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_PROTOCOL_NODE_STATUS_H_
#define UAVCAN_PROTOCOL_NODE_STATUS_H_

#include "serialization_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief uavcan.protocol.NodeStatus
 * @note Abstract node status information.
 */
typedef enum {
    NODE_STATUS_HEALTH_OK = 0,
    NODE_STATUS_HEALTH_WARNING,
    NODE_STATUS_HEALTH_ERROR,
    NODE_STATUS_HEALTH_CRITICAL,
} NodeStatusHealth_t;
typedef enum {
    NODE_STATUS_MODE_OPERATIONAL        = 0,
    NODE_STATUS_MODE_INITIALIZATION     = 1,
    NODE_STATUS_MODE_MAINTENANCE        = 2,
    NODE_STATUS_MODE_SOFTWARE_UPDATE    = 3,
    NODE_STATUS_MODE_OFFLINE            = 7
} NodeStatusMode_t;

/**
 * @brief uavcan.protocol.NodeStatus
 * @note Abstract node status information.
 */
typedef struct {
    uint32_t uptime_sec;                    // uint32
    uint8_t health;                         // uint2
    uint8_t mode;                           // uint3
    uint8_t sub_mode;                       // uint3
    uint16_t vendor_specific_status_code;   // uint16
} NodeStatus_t;

// uavcan.protocol.NodeStatus
#define UAVCAN_PROTOCOL_NODE_STATUS_ID                              341U
#define UAVCAN_PROTOCOL_NODE_STATUS_SIGNATURE                       0x0f0868d0c1a7c6f1ULL
#define UAVCAN_PROTOCOL_NODE_STATUS_MESSAGE_SIZE                    7U
#define NODE_STATUS_SPIN_PERIOD_MS                                  500U
#define UAVCAN_PROTOCOL_NODE_STATUS     UAVCAN_EXPAND(UAVCAN_PROTOCOL_NODE_STATUS)


static inline void uavcanEncodeNodeStatus(
    uint8_t buffer[UAVCAN_PROTOCOL_NODE_STATUS_MESSAGE_SIZE],
    const NodeStatus_t* node_status)
{
    canardEncodeScalar(buffer,  0,  32, &node_status->uptime_sec);
    canardEncodeScalar(buffer,  32, 2,  &node_status->health);
    canardEncodeScalar(buffer,  34, 3,  &node_status->mode);
    canardEncodeScalar(buffer,  37, 3,  &node_status->sub_mode);
    canardEncodeScalar(buffer,  40, 16, &node_status->vendor_specific_status_code);
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_PROTOCOL_NODE_STATUS_H_
