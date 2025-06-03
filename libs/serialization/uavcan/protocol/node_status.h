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
} uavcan_protocol_NodeStatus;

// uavcan.protocol.NodeStatus
#define UAVCAN_PROTOCOL_NODE_STATUS_ID                              341U
#define UAVCAN_PROTOCOL_NODE_STATUS_SIGNATURE                       0x0f0868d0c1a7c6f1ULL
#define UAVCAN_PROTOCOL_NODE_STATUS_MESSAGE_SIZE                    7U
#define NODE_STATUS_SPIN_PERIOD_MS                                  500U
#define UAVCAN_PROTOCOL_NODE_STATUS     UAVCAN_EXPAND(UAVCAN_PROTOCOL_NODE_STATUS)

static inline int8_t uavcan_protocol_node_status_deserialize(const CanardRxTransfer* transfer,
                                                             uavcan_protocol_NodeStatus* obj) {
    if ((transfer == NULL) || (obj == NULL)) {
        return -2;
    }

    return 0;
}

static inline uint32_t uavcan_protocol_node_status_serialize(const uavcan_protocol_NodeStatus* const obj,
                                                             uint8_t* const buffer) {
    if (obj == NULL || buffer == NULL) {
        return 0;
    }

    canardEncodeScalar(buffer,  0,  32, &obj->uptime_sec);
    canardEncodeScalar(buffer,  32, 2,  &obj->health);
    canardEncodeScalar(buffer,  34, 3,  &obj->mode);
    canardEncodeScalar(buffer,  37, 3,  &obj->sub_mode);
    canardEncodeScalar(buffer,  40, 16, &obj->vendor_specific_status_code);

    return UAVCAN_PROTOCOL_NODE_STATUS_MESSAGE_SIZE;
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_PROTOCOL_NODE_STATUS_H_
