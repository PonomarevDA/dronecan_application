/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_ACTUATOR_ARRAY_COMMAND_H_
#define UAVCAN_EQUIPMENT_ACTUATOR_ARRAY_COMMAND_H_

#include "serialization_internal.h"

#define UAVCAN_EQUIPMENT_ACTUATOR_COMMAND_SIGNATURE                 0x8d9a6a920c1d616c
#define UAVCAN_EQUIPMENT_ACTUATOR_COMMAND_MESSAGE_SIZE              32/8

#define UAVCAN_EQUIPMENT_ACTUATOR_STATUS_ID                         1011
#define UAVCAN_EQUIPMENT_ACTUATOR_STATUS_SIGNATURE                  0x5e9bba44faf1ea04
#define UAVCAN_EQUIPMENT_ACTUATOR_STATUS_MESSAGE_SIZE               8

#define UAVCAN_EQUIPMENT_ACTUATOR_ARRAY_COMMAND_ID                  1010
#define UAVCAN_EQUIPMENT_ACTUATOR_ARRAY_COMMAND_SIGNATURE           0xd8a7486238ec3af3
#define UAVCAN_EQUIPMENT_ACTUATOR_ARRAY_COMMAND_MESSAGE_SIZE        484/8 + 1
#define UAVCAN_EQUIPMENT_ACTUATOR_ARRAY_COMMAND UAVCAN_EXPAND(UAVCAN_EQUIPMENT_ACTUATOR_ARRAY_COMMAND)

/**
 * @brief uavcan.equipment.actuator.Command
 * @note Nested type. Single actuator command.
 */
typedef struct {
    uint8_t actuator_id;
    uint8_t command_type;
    float command_value;    // float16
} Command_t;

/**
 * @brief uavcan.equipment.actuator.ArrayCommand
 * @note The system supports up to 256 actuators;
 * up to 15 of them can be commanded with one message.
 */
typedef struct {
    Command_t commads[16];
} ArrayCommand_t;

#ifdef __cplusplus
extern "C" {
#endif

static inline int8_t dronecan_equipment_actuator_arraycommand_deserialize(const CanardRxTransfer* transfer,
                                                                          ArrayCommand_t* obj) {
    if ((transfer == NULL) || (obj == NULL)) {
        return -2;
    }

    uint8_t num_of_cmds = transfer->payload_len / 4;

    uint32_t offset = 0;
    uint8_t ch_num;
    uint16_t f16_dummy;
    for (ch_num = 0; ch_num < num_of_cmds; ch_num++) {
        canardDecodeScalar(transfer, offset, 8, true, &obj->commads[ch_num].actuator_id);
        offset += 8;

        canardDecodeScalar(transfer, offset, 8, true, &obj->commads[ch_num].command_type);
        offset += 8;

        canardDecodeScalar(transfer, offset, 16, true, &f16_dummy);
        offset += 16;
        obj->commads[ch_num].command_value = canardConvertFloat16ToNativeFloat(f16_dummy);
    }
    return ch_num;
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_ACTUATOR_ARRAY_COMMAND_H_
