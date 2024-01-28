/*
 * Copyright (C) 2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMANDS_H_
#define UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMANDS_H_

#include "SingleLightCommand.h"
#include "dronecan.h"
#include "serialization_internal.h"

#define UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND_ID               1081
#define UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND_SIGNATURE        0x2031d93c8bdd1ec4
#define UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND_MESSAGE_SIZE     485/8

#define MAX_LIGHT_COMMAND_NUMBER 20

typedef struct {
    SingleLightCommand_t commands[MAX_LIGHT_COMMAND_NUMBER];
    uint8_t number_of_commands;
} LightsCommand_t;

#ifdef __cplusplus
extern "C" {
#endif

static inline int8_t dronecan_equipment_indication_lights_command_deserialize(
    const CanardRxTransfer* transfer, LightsCommand_t* obj) {

    uint8_t light_id, red, green, blue;
    obj->number_of_commands = transfer->payload_len / 3;
    if (obj->number_of_commands > MAX_LIGHT_COMMAND_NUMBER) {
        obj->number_of_commands = MAX_LIGHT_COMMAND_NUMBER;
    }
    for (uint_fast8_t cmd_idx = 0; cmd_idx < obj->number_of_commands; cmd_idx++) {
        canardDecodeScalar(transfer, cmd_idx * 24,      8, false, &light_id);
        canardDecodeScalar(transfer, cmd_idx * 24 + 8,  5, false, &red);
        canardDecodeScalar(transfer, cmd_idx * 24 + 13, 6, false, &green);
        canardDecodeScalar(transfer, cmd_idx * 24 + 19, 5, false, &blue);
        obj->commands[cmd_idx].light_id = light_id;
        obj->commands[cmd_idx].red = red;
        obj->commands[cmd_idx].green = green;
        obj->commands[cmd_idx].blue = blue;
    }

    return obj->number_of_commands;
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMANDS_H_
