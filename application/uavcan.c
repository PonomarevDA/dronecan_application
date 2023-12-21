/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "uavcan.h"
#include "main.h"
#include "can_driver.h"

#define MAX_SUBS_AMOUNT         10
#define CAN_SPEED               1000000
#define BUFFER_SIZE             1024

/**
  * @brief Encapsulate everything required for a subscriber
  */
typedef struct {
    uint64_t signature;
    uint16_t id;
    void (*callback)(CanardRxTransfer* transfer);
} Subscriber_t;


/**
  * @brief Must have canard callback.
  * The library calls this function on each transfer.
  * @return true if data type is supported and fill signature, otherwise return false
  */
static bool shouldAcceptTransfer(const CanardInstance* ins,
                                 uint64_t* out_data_type_signature,
                                 uint16_t data_type_id,
                                 CanardTransferType transfer_type,
                                 uint8_t source_node_id);

/**
  * @brief Must have canard callback.
  * The library calls this function only when shouldAcceptTransfer returns true
  */
static void onTransferReceived(CanardInstance* ins, CanardRxTransfer* transfer);


CanardInstance g_canard;
static NodeStatus_t node_status;
static uint8_t buffer[BUFFER_SIZE];
static Subscriber_t subscribers[MAX_SUBS_AMOUNT] = {0x00};
static uint8_t subs_amount = 0;
static uint8_t transfer_id = 0;


int16_t uavcanInit(uint8_t node_id) {
    int16_t res = canDriverInit(CAN_SPEED, CAN_DRIVER_FIRST);
    if (res < 0) {
        return res;
    }

    canardInit(&g_canard, buffer, BUFFER_SIZE, onTransferReceived, shouldAcceptTransfer, NULL);
    canardSetLocalNodeID(&g_canard, node_id);

#ifdef DEBUG
    node_status.vendor_specific_status_code = 1;
#else
    node_status.vendor_specific_status_code = 2;
#endif
    node_status.mode = NODE_STATUS_MODE_OPERATIONAL;
    node_status.sub_mode = 0;

    return 0;
}

int16_t uavcanPublish(uint64_t data_type_signature,
                      uint16_t data_type_id,
                      uint8_t* inout_transfer_id,
                      uint8_t priority,
                      const void* payload,
                      uint16_t payload_len) {
    return canardBroadcast(&g_canard,
                           data_type_signature,
                           data_type_id,
                           inout_transfer_id,
                           priority,
                           payload,
                           payload_len);
}


void uavcanRespond(CanardRxTransfer* transfer,
                   uint64_t data_type_signature,
                   uint16_t data_type_id,
                   uint8_t* payload,
                   uint16_t len) {
    if (!transfer || !payload || len == 0) {
        return;
    }
    canardRequestOrRespond(&g_canard,
                           transfer->source_node_id,
                           data_type_signature,
                           data_type_id,
                           &transfer->transfer_id,
                           transfer->priority,
                           CanardResponse,
                           payload,
                           len);
}

uint8_t uavcanProcessSending() {
    const CanardCANFrame* txf = canardPeekTxQueue(&g_canard);
    uint8_t tx_attempt = 0;
    uint8_t tx_frames_counter = 0;
    while (txf) {
        const int tx_res = canDriverTransmit(txf, CAN_DRIVER_FIRST);
        if (tx_res > 0) {
            canardPopTxQueue(&g_canard);
            txf = canardPeekTxQueue(&g_canard);
            tx_frames_counter++;
        } else if (tx_res < 0) {
            break;
        }
        if ((tx_attempt++) > 20) {
            break;
        }
    }
    return tx_frames_counter;
}

bool uavcanProcessReceiving() {
    CanardCANFrame rx_frame;
    int16_t res = canDriverReceive(&rx_frame, CAN_DRIVER_FIRST);
    if (res) {
        uint64_t crnt_time_us = uavcanGetTimeMs() * 1000;
        canardHandleRxFrame(&g_canard, &rx_frame, crnt_time_us);
        return true;
    }
    return false;
}

void canardSpinNodeStatus() {
    static uint32_t last_spin_time_ms = 0;
    uint32_t crnt_time_ms = uavcanGetTimeMs();
    if (crnt_time_ms < last_spin_time_ms + NODE_STATUS_SPIN_PERIOD_MS) {
        return;
    }
    last_spin_time_ms = crnt_time_ms;

    uint8_t buffer[UAVCAN_PROTOCOL_NODE_STATUS_MESSAGE_SIZE];
    node_status.uptime_sec = (crnt_time_ms / 1000);
    uavcanEncodeNodeStatus(buffer, &node_status);
    uavcanPublish(UAVCAN_PROTOCOL_NODE_STATUS_SIGNATURE,
                  UAVCAN_PROTOCOL_NODE_STATUS_ID,
                  &transfer_id,
                  CANARD_TRANSFER_PRIORITY_LOW,
                  buffer,
                  UAVCAN_PROTOCOL_NODE_STATUS_MESSAGE_SIZE);
}

int8_t uavcanSubscribe(uint64_t signature, uint16_t id, void (*callback)(CanardRxTransfer*)) {
    if (subs_amount >= MAX_SUBS_AMOUNT || signature == 0 || id == 0 || callback == NULL) {
        return -1;
    }
    subscribers[subs_amount].signature = signature;
    subscribers[subs_amount].id = id;
    subscribers[subs_amount].callback = callback;
    return subs_amount++;
}

void uavcanSetNodeHealth(NodeStatusHealth_t health) {
    if (node_status.health != NODE_STATUS_HEALTH_CRITICAL &&
            health <= NODE_STATUS_HEALTH_CRITICAL) {
        node_status.health = health;
    }
}
NodeStatusHealth_t uavcanGetNodeHealth() {
    return node_status.health;
}
void uavcanSetNodeStatusMode(NodeStatusMode_t mode) {
    node_status.mode = mode;
}
NodeStatusMode_t uavcanGetNodeStatusMode() {
    return node_status.mode;
}
const NodeStatus_t* uavcanGetNodeStatus() {
    return &node_status;
}

void uavcanSetVendorSpecificStatusCode(uint16_t vssc) {
    node_status.vendor_specific_status_code = vssc;
}

/// ********************************* PRIVATE *********************************

static bool shouldAcceptTransfer(__attribute__((unused)) const CanardInstance* ins,
                          uint64_t* out_data_type_signature,
                          uint16_t data_type_id,
                          CanardTransferType transfer_type,
                          __attribute__((unused)) uint8_t source_node_id) {
    for (uint8_t sub_idx = 0; sub_idx < subs_amount; sub_idx++) {
        if (data_type_id == subscribers[sub_idx].id) {
            *out_data_type_signature = subscribers[sub_idx].signature;
            return true;
        }
    }

    return false;
}

static void onTransferReceived(__attribute__((unused)) CanardInstance* ins,
                        CanardRxTransfer* transfer) {
    for (uint8_t sub_idx = 0; sub_idx < subs_amount; sub_idx++) {
        if (transfer->data_type_id == subscribers[sub_idx].id) {
            subscribers[sub_idx].callback(transfer);
        }
    }
}
