/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file
 *LICENSE.
 ***/

#ifndef UAVCAN_EQUIPMENT_HARDPOINT_COMMAND_H_
#define UAVCAN_EQUIPMENT_HARDPOINT_COMMAND_H_

#include "dcnode/dcnode.h"
#include "serialization_internal.h"

#define UAVCAN_EQUIPMENT_HARDPOINT_COMMAND_ID                1070U
#define UAVCAN_EQUIPMENT_HARDPOINT_COMMAND_SIGNATURE         0xa1a036268b0c3455ULL
#define UAVCAN_EQUIPMENT_HARDPOINT_COMMAND_MESSAGE_SIZE      3U

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

static inline uint32_t dronecan_equipment_hardpoint_command_serialize(
    const HardpointCommand* const obj, uint8_t* const buffer) {
    if (obj == NULL || buffer == NULL) {
        return 0;
    }

    canardEncodeScalar(buffer, 0, 8, &obj->hardpoint_id);
    canardEncodeScalar(buffer, 8, 16, &obj->command);

    return UAVCAN_EQUIPMENT_HARDPOINT_COMMAND_MESSAGE_SIZE;
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_HARDPOINT_COMMAND_H_
