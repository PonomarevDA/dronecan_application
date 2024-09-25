/*
 * Copyright (C) 2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <assert.h>
#include <iostream>
#include <chrono>
#include <string.h>
#include "storage.h"
#include "dronecan.h"
#include "storage.h"
#include "subscriber.hpp"
#include "publisher.hpp"

/**
 * @brief Platform specific functions which should be provided by a user
 */
uint32_t platformSpecificGetTimeMs() {
    static auto start_time = std::chrono::high_resolution_clock::now();
    auto crnt_time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(crnt_time - start_time).count();
}

/**
 * @brief Application specific functions
 */
void rc1_callback(const RawCommand_t& msg) {
    std::cout << "Get RawCommand 1 with " << (int)msg.size << " commands." << std::endl;
}
void rc2_callback(const RawCommand_t& msg) {
    std::cout << "Get RawCommand 2 with " << (int)msg.size << " commands." << std::endl;
}

void ac1_callback(const ArrayCommand_t& msg) {
    std::cout << "Get ArrayCommand_t1 with " << msg.size << "commands." << std::endl;
}
void ac2_callback(const ArrayCommand_t& msg) {
    std::cout << "Get ArrayCommand_t2 with " << msg.size << "commands." << std::endl;
}
bool ac1_filter(const ArrayCommand_t& msg) {
    for (size_t idx = 0; idx < msg.size; idx++) {
        if (msg.commads[idx].actuator_id == 0) {
            return true;
        }
    }
    return false;
}
bool ac2_filter(const ArrayCommand_t& msg) {
    for (size_t idx = 0; idx < msg.size; idx++) {
        if (msg.commads[idx].actuator_id == 1) {
            return true;
        }
    }
    return false;
}

void lights_callback(const LightsCommand_t& msg) {
    std::cout << "Get LightsCommand_t with " << msg.number_of_commands << " commands." << std::endl;
}


/**
 * @brief Main application entry point
 */
int main() {
    paramsInit(1, 1, -1, 1);
    auto init_res = uavcanInitApplication(42);
    if (init_res < 0) {
        std::cout << "CAN interface could not be found. Exit with code " << init_res << std::endl;
        return init_res;
    }

    DronecanSubscriber<LightsCommand_t> lights_command_sub;
    lights_command_sub.init(&lights_callback);

    DronecanSubscriber<ArrayCommand_t> array_command_sub1;
    array_command_sub1.init(&ac1_callback, &ac1_filter);

    DronecanSubscriber<ArrayCommand_t> array_command_sub2;
    array_command_sub2.init(&ac2_callback, &ac2_filter);

    DronecanSubscriber<RawCommand_t> raw_command_sub1;
    raw_command_sub1.init(&rc1_callback);

    DronecanSubscriber<RawCommand_t> raw_command_sub2;
    raw_command_sub2.init(&rc2_callback);

    DronecanPeriodicPublisher<CircuitStatus_t> circuit_status(2.0f);
    DronecanPeriodicPublisher<BatteryInfo_t> battery_info(1.0f);

    while (platformSpecificGetTimeMs() < 50000) {
        circuit_status.spinOnce();
        battery_info.spinOnce();
        uavcanSpinOnce();
    }

    std::cout << "Good. Enough." << std::endl;
    return 0;
}
