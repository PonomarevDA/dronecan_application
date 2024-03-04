/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine96@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file LICENSE.
***/ 

#ifndef UAVCAN_EQUIPMENT_HARDPOINT_COMMAND_H_
#define UAVCAN_EQUIPMENT_HARDPOINT_COMMAND_H_

#include "dronecan.h"
#include "serialization_internal.h"

#define UAVCAN_EQUIPMENT_HARDPOINT_COMMAND_ID                       1070
#define UAVCAN_EQUIPMENT_HARDPOINT_COMMAND_SIGNATURE                0xa1a036268b0c3455
#define UAVCAN_EQUIPMENT_HARDPOINT_COMMAND_MESSAGE_SIZE             3  // 24/8

typedef struct {
    uint8_t hardpoint_id;
    uint16_t command; // Either a binary command (0 - release, 1+ - hold) or bitmask
} HardpointCommand_t;

#ifdef __cplusplus
extern "C" {
#endif

static inline int8_t dronecan_equipment_hardpoint_command_deserialize(
    const CanardRxTransfer* transfer, HardpointCommand_t* obj) {
    
    if ((transfer == NULL) || (obj == NULL)) {
        return -2;
    }

    canardDecodeScalar(transfer, 0,     8,  false, &obj->hardpoint_id);
    canardDecodeScalar(transfer, 8,     16, false, &obj->command);

    return 0;
}


#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_HARDPOINT_COMMAND_H_
