/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file
 *LICENSE.
 ***/

#ifndef UAVCAN_EQUIPMENT_HARDPOINT_COMMAND_H_
#define UAVCAN_EQUIPMENT_HARDPOINT_COMMAND_H_

#include "libdcnode/dronecan.h"
#include "libdcnode/serialization_internal.h"

#define UAVCAN_EQUIPMENT_HARDPOINT_COMMAND_ID                1070
#define UAVCAN_EQUIPMENT_HARDPOINT_COMMAND_SIGNATURE         0xa1a036268b0c3455
#define UAVCAN_EQUIPMENT_HARDPOINT_COMMAND_MESSAGE_SIZE      3  // 24/8

#define UAVCAN_EQUIPMENT_HARDPOINT_COMMAND UAVCAN_EXPAND(UAVCAN_EQUIPMENT_HARDPOINT_COMMAND)

typedef struct {
    uint8_t hardpoint_id;
    uint16_t command;  // Either a binary command (0 - release, 1+ - hold) or bitmask
} HardpointCommand;

#ifdef __cplusplus
extern "C" {
#endif

static inline int8_t dronecan_equipment_hardpoint_command_deserialize(
    const CanardRxTransfer* transfer, HardpointCommand* obj) {
    if ((transfer == NULL) || (obj == NULL)) {
        return -2;
    }

    canardDecodeScalar(transfer, 0, 8, false, &obj->hardpoint_id);
    canardDecodeScalar(transfer, 8, 16, false, &obj->command);

    return 0;
}

static inline int8_t dronecan_equipment_hardpoint_command_serialize(
    const HardpointCommand* const obj, uint8_t* const buffer,
    size_t* const inout_buffer_size_bytes) {
    if ((obj == NULL) || (buffer == NULL) ||
        (inout_buffer_size_bytes == NULL)) {
        return -2;
    }

    const size_t capacity_bytes = *inout_buffer_size_bytes;
    if (capacity_bytes < UAVCAN_PROTOCOL_NODE_STATUS_MESSAGE_SIZE) {
        return -3;
    }

    canardEncodeScalar(buffer, 0, 8, &obj->hardpoint_id);
    canardEncodeScalar(buffer, 8, 16, &obj->command);

    return 0;
}

static inline int8_t dronecan_equipment_hardpoint_command_publish(
    const HardpointCommand* const obj, uint8_t* inout_transfer_id) {
    uint8_t buffer[UAVCAN_PROTOCOL_NODE_STATUS_MESSAGE_SIZE];
    size_t inout_buffer_size = UAVCAN_PROTOCOL_NODE_STATUS_MESSAGE_SIZE;
    dronecan_equipment_hardpoint_command_serialize(obj, buffer, &inout_buffer_size);
    uavcanPublish(UAVCAN_EQUIPMENT_HARDPOINT_COMMAND_SIGNATURE,
                  UAVCAN_EQUIPMENT_HARDPOINT_COMMAND_ID,
                  inout_transfer_id,
                  CANARD_TRANSFER_PRIORITY_MEDIUM,
                  buffer,
                  UAVCAN_PROTOCOL_NODE_STATUS_MESSAGE_SIZE);

    return 0;
}

static inline int8_t uavcanSubscribeHardpointCommand(void (*transfer_callback)(CanardRxTransfer*)) {
    return uavcanSubscribe(UAVCAN_EQUIPMENT_HARDPOINT_COMMAND, transfer_callback);
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_HARDPOINT_COMMAND_H_
