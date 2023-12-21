/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef APPLICATION_UAVCAN_H_
#define APPLICATION_UAVCAN_H_

#include "dronecan_application_internal.h"
#include "uavcan/protocol/node_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief Call this function once during initialization.
  * It will automatically configure STM32 CAN settings.
  */
int16_t uavcanInit(uint8_t node_id);

/**
  * @brief Call this function once per each subscriber.
  * The application will automatically handle callbacks.
  * Callbacks should end ASAP.
  */
int8_t uavcanSubscribe(uint64_t data_type_signature,
                       uint16_t data_type_id,
                       void (callback)(CanardRxTransfer* transfer));
/**
  * @brief These functions are used to either broadcast messages or respond on RPC-request.
  */
int16_t uavcanPublish(uint64_t data_type_signature,
                      uint16_t data_type_id,
                      uint8_t* inout_transfer_id,
                      uint8_t priority,
                      const void* payload,
                      uint16_t payload_len);
void uavcanRespond(CanardRxTransfer* transfer,
                   uint64_t data_type_signature,
                   uint16_t data_type_id,
                   uint8_t* payload,
                   uint16_t len);

/**
  * @brief Functions below should be called periodically to handle the application.
  */
void uavcanSpinOnce();

/**
  * @brief NodeStatus API
  */
void uavcanSetNodeHealth(NodeStatusHealth_t health);
NodeStatusHealth_t uavcanGetNodeHealth();
NodeStatusMode_t uavcanGetNodeStatusMode();
void uavcanSetNodeStatusMode(NodeStatusMode_t mode);
const NodeStatus_t* uavcanGetNodeStatus();
void uavcanSetVendorSpecificStatusCode(uint16_t vssc);

#ifdef __cplusplus
}
#endif

#endif  // APPLICATION_UAVCAN_H_
