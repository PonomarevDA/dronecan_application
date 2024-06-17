/*
 * Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef EXAMPLES_UBUNTU_SUBSCRIBERS_ARRAY_COMMAND_HPP_
#define EXAMPLES_UBUNTU_SUBSCRIBERS_ARRAY_COMMAND_HPP_

#include <stdint.h>
#include <iostream>
#include "dronecan.h"
#include "uavcan/equipment/actuator/ArrayCommand.h"

/**
 * @brief Just an example of encapsulated ArrayCommand subscriber
 */
class ArrayCommandSubscriber {
public:
    ArrayCommandSubscriber() = default;

    inline int8_t init() {
        int8_t sub_id = uavcanSubscribe(UAVCAN_EQUIPMENT_ACTUATOR_ARRAY_COMMAND, ArrayCommandSubscriber::callback);
        if (sub_id < 0) {
            std::cout << "Something wrong. Sobescriber has not been created. Abort." << sub_id << std::endl;
        }
        return sub_id;
    }

    static inline void callback(CanardRxTransfer* transfer) {
        uint8_t cmds_amount = dronecan_equipment_actuator_arraycommand_deserialize(transfer, &array_command);
        std::cout << "Get ArrayCommand with " << (int)cmds_amount << " commands." << std::endl;
    }

    static inline ArrayCommand_t array_command = {};

private:
    uint32_t next_pub_time_ms = 500;
    uint8_t inout_transfer_id;
};

#endif  // EXAMPLES_UBUNTU_SUBSCRIBERS_ARRAY_COMMAND_HPP_
