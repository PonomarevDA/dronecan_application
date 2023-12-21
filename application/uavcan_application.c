/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "uavcan_application.h"
#include <string.h>

#include "uavcan/protocol/get_transport_stats.h"
#include "uavcan/protocol/node_status.h"
#include "uavcan/protocol/restart_node.h"
#include "uavcan/protocol/param/execute_opcode.h"
#include "uavcan/protocol/param/getset.h"
#include "uavcan.h"
#include "storage.h"
#include "can_driver.h"


#ifndef APP_NODE_NAME
    #define APP_NODE_NAME       (char*)"default"
#endif

#ifndef GIT_HASH
    #define GIT_HASH            0
#endif

#ifndef APP_VERSION_MAJOR
    #define APP_VERSION_MAJOR   0
#endif

#ifndef APP_VERSION_MINOR
    #define APP_VERSION_MINOR   0
#endif

#ifndef HW_VERSION_MAJOR
    #define HW_VERSION_MAJOR    0
#endif

#ifndef HW_VERSION_MINOR
    #define HW_VERSION_MINOR    0
#endif


static GetTransportStats_t iface_stats = {0};
static const char* node_name = APP_NODE_NAME;
static SoftwareVersion sw_version;
static HardwareVersion hw_version;


static void uavcanProtocolGetNodeInfoHandleRequest(CanardRxTransfer* transfer);
static void uavcanProtocolParamGetSetHandleRequest(CanardRxTransfer* transfer);
static void uavcanParamExecuteOpcodeHandleRequest(CanardRxTransfer* transfer);
static void uavcanProtocolRestartNodeHandleRequest(CanardRxTransfer* transfer);
static void uavcanProtocolGetTransportStatHandleRequest(CanardRxTransfer* transfer);


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

    uavcanSubscribe(UAVCAN_GET_NODE_INFO_DATA_TYPE, uavcanProtocolGetNodeInfoHandleRequest);
    uavcanSubscribe(UAVCAN_PROTOCOL_PARAM_GETSET, uavcanProtocolParamGetSetHandleRequest);
    uavcanSubscribe(UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE, uavcanParamExecuteOpcodeHandleRequest);
    uavcanSubscribe(UAVCAN_PROTOCOL_RESTART_NODE, uavcanProtocolRestartNodeHandleRequest);
    uavcanSubscribe(UAVCAN_PROTOCOL_GET_TRANSPORT_STATS, uavcanProtocolGetTransportStatHandleRequest);

    return 0;
}

void uavcanConfigure(const SoftwareVersion* new_sw_vers, const HardwareVersion* new_hw_vers) {
    sw_version.major = new_sw_vers->major;
    sw_version.minor = new_sw_vers->minor;
    sw_version.vcs_commit = new_sw_vers->vcs_commit;
    hw_version.major = new_hw_vers->major;
    hw_version.minor = new_hw_vers->minor;
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

void uavcanSetNodeName(const char* new_node_name) {
    node_name = new_node_name;
}

/// ********************************* PRIVATE *********************************

static void uavcanProtocolGetNodeInfoHandleRequest(CanardRxTransfer* transfer) {
    uint8_t buf[UAVCAN_GET_NODE_INFO_RESPONSE_MAX_SIZE];
    const NodeStatus_t* status = uavcanGetNodeStatus();
    uint16_t len = uavcanEncodeParamGetNodeInfo(buf, status, &sw_version, &hw_version, node_name);
    uavcanRespond(transfer, UAVCAN_GET_NODE_INFO_DATA_TYPE, buf, len);
}

static void uavcanProtocolParamGetSetHandleRequest(CanardRxTransfer* transfer) {
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
        param_idx = paramsGetIndexByName(recv_name, param_name_length);
    } else {
        param_idx = uavcanParamGetSetDecodeIndex(transfer);
    }

    // @todo a response size must be carefully estimated because it may cause a segfault
    uint8_t resp[96] = "";
    uint16_t len;

    const char* name = paramsGetParamName(param_idx);
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
        uint8_t* str_value = (uint8_t*)paramsGetStringValue(param_idx);
        len = uavcanParamGetSetMakeStringResponse(resp, str_value, name);
    } else {
        len = uavcanParamGetSetMakeEmptyResponse(resp);
    }

    uavcanRespond(transfer, UAVCAN_PROTOCOL_PARAM_GETSET, resp, len);
}

static void uavcanParamExecuteOpcodeHandleRequest(CanardRxTransfer* transfer) {
    uavcanProtocolParamExecuteOpcodeDecode(transfer);

    uint8_t buffer[7];
    int8_t ok = (paramsLoadToFlash() == -1) ? 0 : 1;
    uavcanProtocolParamExecuteOpcodeEncode(buffer, ok);
    uavcanRespond(transfer, UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE, buffer, 7);
}

static void uavcanProtocolRestartNodeHandleRequest(__attribute__((unused)) CanardRxTransfer* transfer) {
    uavcanRestartNode();
}

static void uavcanProtocolGetTransportStatHandleRequest(CanardRxTransfer* transfer) {
    static uint8_t buffer[UAVCAN_PROTOCOL_GET_TRANSPORT_STATS_MAX_SIZE];
    iface_stats.transfer_errors = canDriverGetErrorCount();

    uavcanEncodeTransportStats(buffer, &iface_stats);
    uavcanRespond(transfer, UAVCAN_PROTOCOL_GET_TRANSPORT_STATS, buffer, 72);
}
