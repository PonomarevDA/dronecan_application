/*
 * Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef EXAMPLES_UBUNTU_SUBSCRIBERS_LIGHTS_HPP_
#define EXAMPLES_UBUNTU_SUBSCRIBERS_LIGHTS_HPP_

#include <stdint.h>
#include <iostream>
#include "dronecan.h"
#include "uavcan/equipment/indication/LightsCommand.h"

/**
 * @brief Just an example of encapsulated LightsCommand subscriber
 */
class LightsCommandSubscriber {
public:
    LightsCommandSubscriber() = default;

    inline int8_t init() {
        int8_t sub_id = uavcanSubscribe(UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND_SIGNATURE,
                                        UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND_ID,
                                        LightsCommandSubscriber::callback);
        if (sub_id < 0) {
            std::cout << "Something wrong. Sobescriber has not been created. Abort." << sub_id << std::endl;
        }
        return sub_id;
    }

    static inline  void callback(CanardRxTransfer* transfer) {
        uint8_t cmds_amount = dronecan_equipment_indication_lights_command_deserialize(transfer, &lights_command);
        std::cout << "Get LightsCommand with " << (int)cmds_amount << " commands." << std::endl;
    }

    static inline LightsCommand_t lights_command = {};
private:
    uint32_t next_pub_time_ms = 500;
    uint8_t inout_transfer_id;
};



#endif  // EXAMPLES_UBUNTU_SUBSCRIBERS_LIGHTS_HPP_
