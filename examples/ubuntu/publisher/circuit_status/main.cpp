/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <assert.h>
#include <iostream>
#include <chrono>
#include <string.h>
#include "dronecan.h"
#include "uavcan/equipment/power/CircuitStatus.h"
#include "storage.h"
#include "flash_driver.h"

IntegerDesc_t __attribute__((weak)) integer_desc_pool[] = {
    {"uavcan.node.id",              0,      100,    50},
};
IntegerParamValue_t integer_values_pool[sizeof(integer_desc_pool) / sizeof(IntegerDesc_t)];
StringDesc_t __attribute__((weak)) string_desc_pool[] = {
    {"system.name",              "dronecan_application"},
};
StringParamValue_t string_values_pool[sizeof(string_desc_pool) / sizeof(StringDesc_t)];

uint32_t uavcanGetTimeMs() {
    static auto start_time = std::chrono::high_resolution_clock::now();
    auto crnt_time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(crnt_time - start_time).count();
}


class CircuitStatusPublisher {
public:
    CircuitStatusPublisher() = default;
    void spinOnce();
    CircuitStatus_t circuit_status;
private:
    uint32_t next_pub_time_ms = 500;
    uint8_t inout_transfer_id;
};

void CircuitStatusPublisher::spinOnce() {
    auto crnt_time_ms = uavcanGetTimeMs();
    if (crnt_time_ms > next_pub_time_ms) {
        next_pub_time_ms += 500;
        dronecan_equipment_circuit_status_publish(&circuit_status, &inout_transfer_id);
        std::cout << "Publish CircuitStatus." << std::endl;
    }
}


int main (int argc, char *argv[]) {
    const uint8_t node_id = 42;
    auto init_res = uavcanInitApplication(node_id);
    if (init_res < 0) {
        std::cout << "CAN interface could be found. Exit with code " << init_res << std::endl;
        return init_res;
    }

    CircuitStatusPublisher circuit_status;

    while (uavcanGetTimeMs() < 3000) {
        circuit_status.spinOnce();
        uavcanSpinOnce();
    }

    std::cout << "Good. Enough." << std::endl;
    return 0;
}
