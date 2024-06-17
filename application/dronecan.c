/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "dronecan.h"
#include <string.h>
#include "uavcan/protocol/get_transport_stats.h"
#include "uavcan/protocol/node_status.h"
#include "uavcan/protocol/restart_node.h"
#include "uavcan/protocol/param/execute_opcode.h"
#include "uavcan/protocol/param/getset.h"
#include "storage.h"
#include "can_driver.h"


#ifndef APP_NODE_NAME
    #define APP_NODE_NAME       (char*)"default"
#endif

#ifndef GIT_HASH
    #warning "GIT_HASH has been assigned to 0 by default."
    #define GIT_HASH            0
#endif

#ifndef APP_VERSION_MAJOR
    #warning "APP_VERSION_MAJOR has been assigned to 0 by default."
    #define APP_VERSION_MAJOR   0
#endif

#ifndef APP_VERSION_MINOR
    #warning "APP_VERSION_MINOR has been assigned to 0 by default."
    #define APP_VERSION_MINOR   0
#endif

#ifndef HW_VERSION_MAJOR
    #warning "HW_VERSION_MAJOR has been assigned to 0 by default."
    #define HW_VERSION_MAJOR    0
#endif

#ifndef HW_VERSION_MINOR
    #warning "HW_VERSION_MINOR has been assigned to 0 by default."
    #define HW_VERSION_MINOR    0
#endif

#ifndef MAX_SUBS_AMOUNT
    #define MAX_SUBS_AMOUNT     10
#endif

#ifndef CANARD_BUFFER_SIZE
    #define CANARD_BUFFER_SIZE      1024
#endif

#define CAN_SPEED               1000000


/**
  * @brief Encapsulate everything required for a subscriber
  */
typedef struct {
    uint64_t signature;
    uint16_t id;
    void (*callback)(CanardRxTransfer* transfer);
} Subscriber_t;


static CanardInstance g_canard;
static NodeStatus_t node_status;
static uint8_t buffer[CANARD_BUFFER_SIZE];
static Subscriber_t subscribers[MAX_SUBS_AMOUNT] = {0x00};
static uint8_t subs_amount = 0;
static uint8_t transfer_id = 0;
static GetTransportStats_t iface_stats = {0};
static const char* node_name = APP_NODE_NAME;
static SoftwareVersion sw_version;
static HardwareVersion hw_version;


static int16_t uavcanInit(uint8_t node_id);
static bool shouldAcceptTransfer(const CanardInstance* ins,
                                 uint64_t* out_data_type_signature,
                                 uint16_t data_type_id,
                                 CanardTransferType transfer_type,
                                 uint8_t source_node_id);
static void onTransferReceived(CanardInstance* ins, CanardRxTransfer* transfer);
static uint8_t uavcanProcessSending();
static bool uavcanProcessReceiving();
static void uavcanSpinNodeStatus();

static void uavcanProtocolGetNodeInfoHandle(CanardRxTransfer* transfer);
static void uavcanProtocolParamGetSetHandle(CanardRxTransfer* transfer);
static void uavcanParamExecuteOpcodeHandle(CanardRxTransfer* transfer);
static void uavcanProtocolRestartNodeHandle(CanardRxTransfer* transfer);
static void uavcanProtocolGetTransportStatHandle(CanardRxTransfer* transfer);


int16_t uavcanInitApplication(uint8_t node_id) {
    int16_t res = uavcanInit(node_id);
    if (res != 0) {
        return res;
    }

    sw_version.vcs_commit = GIT_HASH >> 32;
    sw_version.major = APP_VERSION_MAJOR;
    sw_version.minor = APP_VERSION_MINOR;
    hw_version.major = HW_VERSION_MAJOR;
    hw_version.minor = HW_VERSION_MINOR;

    uavcanReadUniqueID(hw_version.unique_id);

    uavcanSubscribe(UAVCAN_GET_NODE_INFO_DATA_TYPE,      uavcanProtocolGetNodeInfoHandle);
    uavcanSubscribe(UAVCAN_PROTOCOL_PARAM_GETSET,        uavcanProtocolParamGetSetHandle);
    uavcanSubscribe(UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE, uavcanParamExecuteOpcodeHandle);
    uavcanSubscribe(UAVCAN_PROTOCOL_RESTART_NODE,        uavcanProtocolRestartNodeHandle);
    uavcanSubscribe(UAVCAN_PROTOCOL_GET_TRANSPORT_STATS, uavcanProtocolGetTransportStatHandle);

    return 0;
}

void uavcanSpinOnce(uint32_t crnt_time_ms) {
    uavcanProcessSending();
    uavcanProcessReceiving(crnt_time_ms);
    uavcanSpinNodeStatus(crnt_time_ms);
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
                   const uint8_t* payload,
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

void uavcanConfigure(const SoftwareVersion* new_sw_vers, const HardwareVersion* new_hw_vers) {
    sw_version.major = new_sw_vers->major;
    sw_version.minor = new_sw_vers->minor;
    sw_version.vcs_commit = new_sw_vers->vcs_commit;
    hw_version.major = new_hw_vers->major;
    hw_version.minor = new_hw_vers->minor;
}

void uavcanSetNodeName(const char* new_node_name) {
    node_name = new_node_name;
}

void uavcanStatsIncreaseCanErrors() {
    iface_stats.transfer_errors++;
}
void uavcanStatsIncreaseCanTx(uint8_t num_of_transfers) {
    iface_stats.transfers_tx += num_of_transfers;
}
void uavcanStatsIncreaseCanRx() {
    iface_stats.transfers_rx++;
}
void uavcanStatsIncreaseUartErrors() {
    iface_stats.can_iface_stats[0].errors++;
}
void uavcanStatsIncreaseUartTx(uint32_t num) {
    iface_stats.can_iface_stats[0].frames_tx += num;
}
void uavcanStatsIncreaseUartRx(uint32_t num) {
    iface_stats.can_iface_stats[0].frames_rx += num;
}
uint64_t uavcanGetErrorCount() {
    return canDriverGetErrorCount();
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

void uavcanSetVendorSpecificStatusCode(uint16_t vssc) {
    node_status.vendor_specific_status_code = vssc;
}

const NodeStatus_t* uavcanGetNodeStatus() {
    return &node_status;
}

/// ********************************* PRIVATE *********************************
/**
  * @brief Call this function once during initialization.
  * It will automatically configure STM32 CAN settings.
  */
static int16_t uavcanInit(uint8_t node_id) {
    int16_t res = canDriverInit(CAN_SPEED, CAN_DRIVER_FIRST);
    if (res < 0) {
        return res;
    }

    canardInit(&g_canard,
               buffer,
               CANARD_BUFFER_SIZE,
               onTransferReceived,
               shouldAcceptTransfer,
               NULL);
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

/**
  * @brief Must have canard callback.
  * The library calls this function on each transfer.
  * @return true if data type is supported and fill signature, otherwise return false
  */
static bool shouldAcceptTransfer(__attribute__((unused)) const CanardInstance* ins,
                                 uint64_t* out_data_type_signature,
                                 uint16_t data_type_id,
                                 __attribute__((unused)) CanardTransferType transfer_type,
                                 __attribute__((unused)) uint8_t source_node_id) {
    for (uint8_t sub_idx = 0; sub_idx < subs_amount; sub_idx++) {
        if (data_type_id == subscribers[sub_idx].id) {
            *out_data_type_signature = subscribers[sub_idx].signature;
            return true;
        }
    }

    return false;
}

/**
  * @brief Must have canard callback.
  * The library calls this function only when shouldAcceptTransfer returns true
  */
static void onTransferReceived(__attribute__((unused)) CanardInstance* ins,
                        CanardRxTransfer* transfer) {
    for (uint8_t sub_idx = 0; sub_idx < subs_amount; sub_idx++) {
        if (transfer->data_type_id == subscribers[sub_idx].id) {
            subscribers[sub_idx].callback(transfer);
        }
    }
}

static uint8_t uavcanProcessSending() {
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

static bool uavcanProcessReceiving(uint32_t crnt_time_ms) {
    CanardCANFrame rx_frame;
    int16_t res = canDriverReceive(&rx_frame, CAN_DRIVER_FIRST);
    if (res) {
        uint64_t crnt_time_us = crnt_time_ms * 1000;
        canardHandleRxFrame(&g_canard, &rx_frame, crnt_time_us);
        return true;
    }
    return false;
}

static void uavcanSpinNodeStatus(uint32_t crnt_time_ms) {
    static uint32_t last_spin_time_ms = 0;
    if (crnt_time_ms < last_spin_time_ms + NODE_STATUS_SPIN_PERIOD_MS) {
        return;
    }
    last_spin_time_ms = crnt_time_ms;

    uint8_t node_status_buffer[UAVCAN_PROTOCOL_NODE_STATUS_MESSAGE_SIZE];
    node_status.uptime_sec = (crnt_time_ms / 1000);
    uavcanEncodeNodeStatus(node_status_buffer, &node_status);
    uavcanPublish(UAVCAN_PROTOCOL_NODE_STATUS_SIGNATURE,
                  UAVCAN_PROTOCOL_NODE_STATUS_ID,
                  &transfer_id,
                  CANARD_TRANSFER_PRIORITY_LOW,
                  node_status_buffer,
                  UAVCAN_PROTOCOL_NODE_STATUS_MESSAGE_SIZE);
}

static void uavcanProtocolGetNodeInfoHandle(CanardRxTransfer* transfer) {
    uint8_t buf[UAVCAN_GET_NODE_INFO_RESPONSE_MAX_SIZE];
    const NodeStatus_t* status = uavcanGetNodeStatus();
    uint16_t len = uavcanEncodeParamGetNodeInfo(buf, status, &sw_version, &hw_version, node_name);
    uavcanRespond(transfer, UAVCAN_GET_NODE_INFO_DATA_TYPE, buf, len);
}

static void uavcanProtocolParamGetSetHandle(CanardRxTransfer* transfer) {
    // Value value
    uint8_t set_value_type_tag = uavcanParamGetSetDecodeValueTag(transfer);
    int64_t val_int64 = 0;
    uint8_t val_string[STRING_MAX_SIZE];
    int offset;
    uint8_t str_len = 0;
    switch (set_value_type_tag) {
        case PARAM_VALUE_INTEGER:
            val_int64 = uavcanParamGetSetDecodeInteger(transfer);
            offset = 80;
            break;
        case PARAM_VALUE_STRING:
            str_len = uavcanParamGetSetDecodeString(transfer, val_string);
            offset = 24 + 8 * str_len;
            break;
        default:
            offset = 16;
            break;
    }

    // uint8[<=92] recv_name
    uint8_t recv_name[MAX_PARAM_NAME_LENGTH] = "";
    uint16_t param_name_length = uavcanParamGetSetDecodeName(transfer, offset, recv_name);
    offset += 8 * param_name_length;

    // uint13 index
    uint16_t param_idx;
    if (param_name_length) {
        param_idx = paramsFind(recv_name, param_name_length);
    } else {
        param_idx = uavcanParamGetSetDecodeIndex(transfer);
    }

    // @todo a response size must be carefully estimated because it may cause a segfault
    uint8_t resp[96] = "";
    uint16_t len;

    const char* name = paramsGetName(param_idx);
    if (paramsGetType(param_idx) == PARAM_TYPE_INTEGER) {
        if (set_value_type_tag == PARAM_VALUE_INTEGER) {
            paramsSetIntegerValue(param_idx, val_int64);
        }
        const IntegerDesc_t* desc = paramsGetIntegerDesc(param_idx);
        IntegerParamValue_t val = paramsGetIntegerValue(param_idx);
        len = uavcanParamGetSetMakeIntResponse(resp, val, desc->def, desc->min, desc->max, name);
    } else if (paramsGetType(param_idx) == PARAM_TYPE_STRING) {
        if (set_value_type_tag == PARAM_VALUE_STRING) {
            paramsSetStringValue(param_idx, str_len, val_string);
        }
        char* str_value = (char*)paramsGetStringValue(param_idx);
        len = uavcanParamGetSetMakeStringResponse(resp, str_value, name);
    } else {
        len = uavcanParamGetSetMakeEmptyResponse(resp);
    }

    uavcanRespond(transfer, UAVCAN_PROTOCOL_PARAM_GETSET, resp, len);
}

static void uavcanParamExecuteOpcodeHandle(CanardRxTransfer* transfer) {
    uavcanProtocolParamExecuteOpcodeDecode(transfer);

    uint8_t opcode_buffer[7];
    int8_t ok = (paramsSave() == -1) ? 0 : 1;
    uavcanProtocolParamExecuteOpcodeEncode(opcode_buffer, ok);
    uavcanRespond(transfer, UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE, opcode_buffer, 7);
}

static void uavcanProtocolRestartNodeHandle(__attribute__((unused)) CanardRxTransfer* transfer) {
    uavcanRestartNode();
}

static void uavcanProtocolGetTransportStatHandle(CanardRxTransfer* transfer) {
    static uint8_t transport_stats_buffer[UAVCAN_PROTOCOL_GET_TRANSPORT_STATS_MAX_SIZE];
    iface_stats.transfer_errors = canDriverGetErrorCount();

    uavcanEncodeTransportStats(transport_stats_buffer, &iface_stats);
    uavcanRespond(transfer, UAVCAN_PROTOCOL_GET_TRANSPORT_STATS, transport_stats_buffer, 72);
}
