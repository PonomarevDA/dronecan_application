/*
 * Copyright (C) 2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMANDS_H_
#define UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMANDS_H_

#include "SingleLightCommand.h"
#include "libdcnode/dronecan.h"
#include "libdcnode/serialization_internal.h"

#define UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND_ID               1081
#define UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND_SIGNATURE        0x2031d93c8bdd1ec4
#define UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND_MESSAGE_SIZE     485/8

#define UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND UAVCAN_EXPAND(UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND)

#define MAX_LIGHT_COMMAND_NUMBER 20

typedef struct {
    SingleLightCommand_t commands[MAX_LIGHT_COMMAND_NUMBER];
    uint8_t number_of_commands;
} LightsCommand_t;

#ifdef __cplusplus
extern "C" {
#endif

static inline int8_t dronecan_equipment_indication_lights_command_deserialize(
    const CanardRxTransfer* transfer, LightsCommand_t* obj)
{
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
        obj->commands[cmd_idx].color.red = red;
        obj->commands[cmd_idx].color.green = green;
        obj->commands[cmd_idx].color.blue = blue;
    }

    return obj->number_of_commands;
}

static inline int8_t dronecan_equipment_indication_lights_command_serialize(
    const LightsCommand_t* const obj,
    uint8_t* const buffer,
    size_t* const inout_buffer_size_bytes)
{
    if ((obj == NULL) || (buffer == NULL) || (inout_buffer_size_bytes == NULL)) {
        return -2;
    }

    const size_t capacity_bytes = *inout_buffer_size_bytes;
    if (capacity_bytes < UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND_MESSAGE_SIZE) {
        return -3;
    }

    if (obj->number_of_commands == 0) {
        return -4;
    }

    uint8_t red = obj->commands[0].color.red;
    uint8_t green = obj->commands[0].color.green;
    uint8_t blue = obj->commands[0].color.blue;

    canardEncodeScalar(buffer, 0,  8,   &obj->commands[0].light_id);
    canardEncodeScalar(buffer, 8,  5,   &red);
    canardEncodeScalar(buffer, 13, 6,   &green);
    canardEncodeScalar(buffer, 19, 5,   &blue);

    return 0;
}

static inline int8_t dronecan_equipment_indication_lights_command_publish(
    const LightsCommand_t* const obj,
    uint8_t* inout_transfer_id)
{
    uint8_t buffer[UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND_MESSAGE_SIZE];
    size_t inout_size = UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND_MESSAGE_SIZE;
    int8_t res = dronecan_equipment_indication_lights_command_serialize(obj, buffer, &inout_size);
    if (res < 0) {
        return res;
    }

    uavcanPublish(UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND_SIGNATURE,
                  UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND_ID,
                  inout_transfer_id,
                  CANARD_TRANSFER_PRIORITY_MEDIUM,
                  buffer,
                  3);

    return 0;
}

static inline int8_t uavcanSubscribeIndicationLightsCommand(void (*transfer_callback)(CanardRxTransfer*)) {
    return uavcanSubscribe(UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND, transfer_callback);
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMANDS_H_
