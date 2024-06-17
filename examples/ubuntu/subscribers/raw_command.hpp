/*
 * Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef EXAMPLES_UBUNTU_SUBSCRIBERS_RAW_COMMAND_HPP_
#define EXAMPLES_UBUNTU_SUBSCRIBERS_RAW_COMMAND_HPP_

#include <stdint.h>
#include <iostream>
#include "dronecan.h"
#include "uavcan/equipment/esc/RawCommand.h"

/**
 * @brief Just an example of encapsulated RawCommand subscriber
 */
class RawCommandSubscriber {
public:
    RawCommandSubscriber() = default;

    inline int8_t init() {
        int8_t sub_id = uavcanSubscribe(UAVCAN_EQUIPMENT_ESC_RAWCOMMAND, RawCommandSubscriber::callback);
        if (sub_id < 0) {
            std::cout << "Something wrong. Sobescriber has not been created. Abort." << sub_id << std::endl;
        }
        return sub_id;
    }

    static inline void callback(CanardRxTransfer* transfer) {
        uint8_t cmds_amount = dronecan_equipment_esc_raw_command_deserialize(transfer, &raw_command);
        std::cout << "Get RawCommand with " << (int)cmds_amount << " commands." << std::endl;
    }

    static inline RawCommand_t raw_command = {};

private:
    uint32_t next_pub_time_ms = 500;
    uint8_t inout_transfer_id;
};

#endif  // EXAMPLES_UBUNTU_SUBSCRIBERS_RAW_COMMAND_HPP_
