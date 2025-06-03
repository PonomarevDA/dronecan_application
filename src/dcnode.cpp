/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "dcnode/dcnode.h"
#include <string.h>
#include "dcnode/can_driver.h"
#include "uavcan/protocol/get_transport_stats.h"
#include "uavcan/protocol/node_status.h"
#include "uavcan/protocol/restart_node.h"
#include "uavcan/protocol/param/execute_opcode.h"
#include "uavcan/protocol/param/getset.h"
#include "storage.h"


#ifndef APP_NODE_NAME
    #define APP_NODE_NAME       (char*)"default"
#endif

#ifndef GIT_HASH
    #warning "GIT_HASH has been assigned to 0 by default."
    #define GIT_HASH            (uint64_t)0
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

#ifndef CANARD_BUFFER_SIZE
    #define CANARD_BUFFER_SIZE      2048
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
static DronecanPublisher<uavcan_protocol_NodeStatus> node_status;
static uint8_t buffer[CANARD_BUFFER_SIZE];
static Subscriber_t subscribers[DRONECAN_MAX_SUBS_AMOUNT] = {};
static uint8_t subs_amount = 0;
static GetTransportStats_t iface_stats = {};
static const char* node_name = APP_NODE_NAME;
static SoftwareVersion sw_version;
static HardwareVersion hw_version;
static bool id_duplication_detected = false;
static uint64_t last_node_status_msg_us = 0;
static PlatformHooks hooks = {};

static bool shouldAcceptTransfer(const CanardInstance* ins,
                                 uint64_t* out_data_type_signature,
                                 uint16_t data_type_id,
                                 CanardTransferType transfer_type,
                                 uint8_t source_node_id);
static void onTransferReceived(CanardInstance* ins, CanardRxTransfer* transfer);
static void uavcanProcessSending();
static bool uavcanProcessReceiving(uint32_t crnt_time_ms);
static void uavcanSpinNodeStatus(uint32_t crnt_time_ms);

static void uavcanProtocolGetNodeInfoHandle(CanardRxTransfer* transfer);
static void uavcanProtocolParamGetSetHandle(CanardRxTransfer* transfer);
static void uavcanParamExecuteOpcodeHandle(CanardRxTransfer* transfer);
static void uavcanProtocolRestartNodeHandle(CanardRxTransfer* transfer);
static void uavcanProtocolGetTransportStatHandle(CanardRxTransfer* transfer);
static void uavcanProtocolNodeStatusHandle(CanardRxTransfer* transfer);


int16_t DronecanNode::init(const PlatformHooks& platform_hooks, uint8_t node_id) {
    hooks = platform_hooks;

    if (int16_t res = canDriverInit(CAN_SPEED, CAN_DRIVER_FIRST); res < 0) {
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
    node_status.msg.vendor_specific_status_code = 1;
#else
    node_status.msg.vendor_specific_status_code = 2;
#endif
    node_status.msg.mode = NODE_STATUS_MODE_OPERATIONAL;
    node_status.msg.sub_mode = 0;

    sw_version.vcs_commit = GIT_HASH >> 32;
    sw_version.major = APP_VERSION_MAJOR;
    sw_version.minor = APP_VERSION_MINOR;
    hw_version.major = HW_VERSION_MAJOR;
    hw_version.minor = HW_VERSION_MINOR;

    if (hooks.readUniqueID != nullptr) {
        hooks.readUniqueID(hw_version.unique_id);
    }

    DronecanNode::subscribe(UAVCAN_GET_NODE_INFO_DATA_TYPE,      uavcanProtocolGetNodeInfoHandle);
    DronecanNode::subscribe(UAVCAN_PROTOCOL_PARAM_GETSET,        uavcanProtocolParamGetSetHandle);
    DronecanNode::subscribe(UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE, uavcanParamExecuteOpcodeHandle);
    DronecanNode::subscribe(UAVCAN_PROTOCOL_RESTART_NODE,        uavcanProtocolRestartNodeHandle);
    DronecanNode::subscribe(UAVCAN_PROTOCOL_GET_TRANSPORT_STATS, uavcanProtocolGetTransportStatHandle);
    DronecanNode::subscribe(UAVCAN_PROTOCOL_NODE_STATUS,         uavcanProtocolNodeStatusHandle);

    return 0;
}

void DronecanNode::spinOnce() {
    uint32_t crnt_time_ms = getTimeMs();
    uavcanProcessSending();
    uavcanProcessReceiving(crnt_time_ms);
    uavcanSpinNodeStatus(crnt_time_ms);
}

int8_t DronecanNode::subscribe(uint64_t signature,
                               uint16_t id,
                               void (*callback)(CanardRxTransfer*)) {
    if (subs_amount >= DRONECAN_MAX_SUBS_AMOUNT || signature == 0 || id == 0 || callback == NULL) {
        return -1;
    }

    callback = callback;  // dummy self-assignment

    subscribers[subs_amount].signature = signature;
    subscribers[subs_amount].id = id;
    subscribers[subs_amount].callback = callback;
    return subs_amount++;
}

int16_t DronecanNode::publish(uint64_t data_type_signature,
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

void DronecanNode::respond(CanardRxTransfer* transfer,
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

void DronecanNode::configure(const SoftwareVersion* new_sw_vers,
                             const HardwareVersion* new_hw_vers) {
    sw_version.major = new_sw_vers->major;
    sw_version.minor = new_sw_vers->minor;
    sw_version.vcs_commit = new_sw_vers->vcs_commit;
    hw_version.major = new_hw_vers->major;
    hw_version.minor = new_hw_vers->minor;
}

auto DronecanNode::setNodeName(const char* new_node_name) -> void {
    node_name = new_node_name;
}
auto DronecanNode::getNodeName() -> const char* {
    return node_name;
}

/**
* @note TransportStats API
*/
void DronecanNode::statsIncreaseUavcanTransfersTx() {
    iface_stats.transfers_tx++;
}
void DronecanNode::statsIncreaseUavcanTransfersRx() {
    iface_stats.transfers_rx++;
}
void DronecanNode::statsIncreaseUavcanTransfersErrors() {
    iface_stats.transfer_errors++;
}

void DronecanNode::statsIncreaseCanFrameTx() {
    iface_stats.can_iface_stats[0].frames_tx++;
}
void DronecanNode::statsIncreaseCanFrameRx() {
    iface_stats.can_iface_stats[0].frames_rx++;
}
void DronecanNode::statsIncreaseCanFrameErrors() {
    iface_stats.can_iface_stats[0].errors++;
}

uint64_t DronecanNode::getErrorCount() {
    return canDriverGetErrorCount();
}

void DronecanNode::setNodeHealth(NodeStatusHealth_t health) {
    if (node_status.msg.health != NODE_STATUS_HEALTH_CRITICAL &&
            health <= NODE_STATUS_HEALTH_CRITICAL) {
        node_status.msg.health = health;
    }
}
NodeStatusHealth_t DronecanNode::getNodeHealth() {
    return static_cast<NodeStatusHealth_t>(node_status.msg.health);
}

void DronecanNode::setNodeStatusMode(NodeStatusMode_t mode) {
    node_status.msg.mode = mode;
}

NodeStatusMode_t DronecanNode::getNodeStatusMode() {
    return static_cast<NodeStatusMode_t>(node_status.msg.mode);
}

void DronecanNode::setVendorSpecificStatusCode(uint16_t vssc) {
    node_status.msg.vendor_specific_status_code = vssc;
}
uint16_t DronecanNode::getVendorSpecificStatusCode() {
    return node_status.msg.vendor_specific_status_code;;
}

uint32_t DronecanNode::getTimeMs() {
    if (hooks.getTimeMs == nullptr) {
        return 0;
    }

    return hooks.getTimeMs();
}


/// ********************************* PRIVATE *********************************
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
    DronecanNode::statsIncreaseUavcanTransfersRx();

    for (uint8_t sub_idx = 0; sub_idx < subs_amount; sub_idx++) {
        if (transfer->data_type_id == subscribers[sub_idx].id) {
            transfer->sub_id = sub_idx;
            subscribers[sub_idx].callback(transfer);
        }
    }
}

static void uavcanProcessSending() {
    const CanardCANFrame* txf = canardPeekTxQueue(&g_canard);
    uint8_t tx_attempt = 0;
    while (txf) {
        const int tx_res = canDriverTransmit(txf, CAN_DRIVER_FIRST);
        if (tx_res > 0) {
            DronecanNode::statsIncreaseUavcanTransfersTx();
            DronecanNode::statsIncreaseCanFrameTx();
            canardPopTxQueue(&g_canard);
            txf = canardPeekTxQueue(&g_canard);
        } else if (tx_res < 0) {
            DronecanNode::statsIncreaseUavcanTransfersErrors();
            DronecanNode::statsIncreaseCanFrameErrors();
            break;
        }
        if ((tx_attempt++) > 20) {
            DronecanNode::statsIncreaseUavcanTransfersErrors();
            DronecanNode::statsIncreaseCanFrameErrors();
            break;
        }
    }
}

static bool uavcanProcessReceiving(uint32_t crnt_time_ms) {
    for (size_t idx = 0; idx < 10; idx++) {
        CanardCANFrame rx_frame;
        if (auto recv_res = canDriverReceive(&rx_frame, CAN_DRIVER_FIRST); recv_res > 0) {
            uint64_t crnt_time_us = static_cast<uint64_t>(crnt_time_ms) * 1000;
            auto handle_res = canardHandleRxFrame(&g_canard, &rx_frame, crnt_time_us);
            DronecanNode::statsIncreaseCanFrameRx();
            if (handle_res < 0) {
                DronecanNode::statsIncreaseUavcanTransfersErrors();
            }
        } else if (recv_res == 0) {
            break;
        } else {
            DronecanNode::statsIncreaseCanFrameErrors();
            break;
        }
    }

    return false;
}

static void uavcanSpinNodeStatus(uint32_t crnt_time_ms) {
    static uint32_t last_spin_time_ms = 0;

    if (crnt_time_ms < last_spin_time_ms + NODE_STATUS_SPIN_PERIOD_MS) {
        return;
    }
    last_spin_time_ms = crnt_time_ms;

    node_status.msg.uptime_sec = (crnt_time_ms / 1000);
    if (id_duplication_detected && node_status.msg.health < NODE_STATUS_HEALTH_WARNING) {
        if (last_node_status_msg_us + 2000000 < crnt_time_ms * 1000) {
            id_duplication_detected = false;
        } else {
            node_status.msg.health = NODE_STATUS_HEALTH_WARNING;
        }
    }

    node_status.publish();
}

static void uavcanProtocolGetNodeInfoHandle(CanardRxTransfer* transfer) {
    uint8_t buf[UAVCAN_GET_NODE_INFO_RESPONSE_MAX_SIZE];
    uint16_t len = uavcanEncodeParamGetNodeInfo(buf, &node_status.msg, &sw_version, &hw_version, node_name);
    DronecanNode::respond(transfer, UAVCAN_GET_NODE_INFO_DATA_TYPE, buf, len);
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
        auto str_value = reinterpret_cast<const char*>(paramsGetStringValue(param_idx));
        len = uavcanParamGetSetMakeStringResponse(resp, str_value, name);
    } else {
        len = uavcanParamGetSetMakeEmptyResponse(resp);
    }

    DronecanNode::respond(transfer, UAVCAN_PROTOCOL_PARAM_GETSET, resp, len);
}

static void uavcanParamExecuteOpcodeHandle(CanardRxTransfer* transfer) {
    uint8_t opcode = uavcanProtocolParamExecuteOpcodeDecode(transfer);

    uint8_t opcode_buffer[7];
    int8_t ok;
    switch (opcode) {
        case 0:
            ok = (paramsSave() == -1) ? 0 : 1;
            break;
        case 1:
            ok = (paramsResetToDefault() < 0) ? 0 : 1;
            break;
        default:
            ok = -1;
            break;
    }
    uavcanProtocolParamExecuteOpcodeEncode(opcode_buffer, ok);
    DronecanNode::respond(transfer, UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE, opcode_buffer, 7);
}

static void uavcanProtocolRestartNodeHandle(__attribute__((unused)) CanardRxTransfer* transfer) {
    uint8_t ok;
    if (hooks.requestRestart == nullptr) {
        ok = 0;
    } else {
        ok = hooks.requestRestart() ? 128 : 0;
    }
    DronecanNode::respond(transfer, UAVCAN_PROTOCOL_RESTART_NODE, &ok, 1);
}

static void uavcanProtocolGetTransportStatHandle(CanardRxTransfer* transfer) {
    static uint8_t transport_stats_buffer[UAVCAN_PROTOCOL_GET_TRANSPORT_STATS_MAX_SIZE];
    iface_stats.transfer_errors = canDriverGetErrorCount();

    auto num_of_bytes = uavcanEncodeTransportStats(transport_stats_buffer, &iface_stats);
    DronecanNode::respond(transfer,
                          UAVCAN_PROTOCOL_GET_TRANSPORT_STATS,
                          transport_stats_buffer,
                          num_of_bytes);
}

// cppcheck-suppress constParameterCallback
static void uavcanProtocolNodeStatusHandle(CanardRxTransfer* transfer) {
    if (transfer->source_node_id == g_canard.node_id) {
        id_duplication_detected = true;
        last_node_status_msg_us = transfer->timestamp_usec;
    }
}
