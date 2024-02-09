/*
 * Copyright (C) 2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_ESC_RAW_COMMAND_H_
#define UAVCAN_EQUIPMENT_ESC_RAW_COMMAND_H_

#include <stdbool.h>
#include <string.h>
#include "dronecan.h"
#include "serialization_internal.h"

#define UAVCAN_EQUIPMENT_ESC_RAWCOMMAND_ID                          1030
#define UAVCAN_EQUIPMENT_ESC_RAWCOMMAND_SIGNATURE                   0x217f5c87d7ec951d
#define UAVCAN_EQUIPMENT_ESC_RAWCOMMAND_MAX_VALUE                   8192

#define RAWCOMMAND_BIT_LENGTH                                       14
#define NUMBER_OF_RAW_CMD_CHANNELS                                  20
#define MIN_RAWCOMMAND_CHANNEL                                      0

#define UAVCAN_EQUIPMENT_ESC_RAWCOMMAND UAVCAN_EXPAND(UAVCAN_EQUIPMENT_ESC_RAWCOMMAND)

/**
 * @brief uavcan.equipment.esc.RawCommand
 * @note Raw ESC command normalized into [-8192, 8191]; negative values indicate reverse rotation.
 * The ESC should normalize the setpoint into its effective input range.
 * Non-zero setpoint value below minimum should be interpreted as min valid setpoint for the given motor.
 */
typedef struct {
    int16_t raw_cmd[NUMBER_OF_RAW_CMD_CHANNELS];
} RawCommand_t;

#ifdef __cplusplus
extern "C" {
#endif

static inline int8_t dronecan_equipment_esc_raw_command_deserialize(
    const CanardRxTransfer* transfer, RawCommand_t* obj) {

    if ((transfer == NULL) || (obj == NULL)) {
        return -2;
    }

    int offset = 0;
    int16_t len;
    uint8_t ch_num;
    for (ch_num = 0; ch_num < NUMBER_OF_RAW_CMD_CHANNELS; ch_num++) {
        int16_t temp_raw_cmd;
        len = canardDecodeScalar(transfer, offset, RAWCOMMAND_BIT_LENGTH, true, &temp_raw_cmd);
        if (len != RAWCOMMAND_BIT_LENGTH) {
            break;
        }
        obj->raw_cmd[ch_num] = temp_raw_cmd;
        offset += RAWCOMMAND_BIT_LENGTH;
    }

    return ch_num;
}

static inline bool dronecan_equipment_esc_raw_command_channel_deserialize(
                    const CanardRxTransfer* transfer,
                    uint8_t channel_num,
                    RawCommand_t* obj) {
    if ((transfer == NULL) || (obj == NULL) || (channel_num > NUMBER_OF_RAW_CMD_CHANNELS)) {
        return false;
    }

    const uint32_t FIRST_BIT = channel_num * RAWCOMMAND_BIT_LENGTH;
    int16_t len = canardDecodeScalar(transfer, FIRST_BIT, RAWCOMMAND_BIT_LENGTH, true, obj->raw_cmd);
    if (len < RAWCOMMAND_BIT_LENGTH) {
        return false;
    }
    return true;
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_ESC_RAW_COMMAND_H_
