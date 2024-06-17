/*
 * Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef EXAMPLES_UBUNTU_PUBLISHERS_CIRCUIT_STATUS_HPP_
#define EXAMPLES_UBUNTU_PUBLISHERS_CIRCUIT_STATUS_HPP_

#include <stdint.h>
#include <iostream>
#include "dronecan.h"
#include "uavcan/equipment/power/CircuitStatus.h"


class CircuitStatusPublisher {
public:
    CircuitStatusPublisher() = default;

    inline void spinOnce(uint32_t crnt_time_ms) {
        if (crnt_time_ms < next_pub_time_ms) {
            return;
        }
        next_pub_time_ms = crnt_time_ms + 500;

        publish();
        std::cout << "Publish CircuitStatus." << std::endl;
    }

    inline void publish() {
        dronecan_equipment_circuit_status_publish(&msg, &inout_transfer_id);
        inout_transfer_id++;
    }

    CircuitStatus_t msg;
private:
    uint32_t next_pub_time_ms = 500;
    uint8_t inout_transfer_id;
};

#endif  // EXAMPLES_UBUNTU_PUBLISHERS_CIRCUIT_STATUS_HPP_
