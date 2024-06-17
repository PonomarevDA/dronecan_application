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
#include "subscribers/lights.hpp"
#include "subscribers/raw_command.hpp"
#include "subscribers/array_command.hpp"

/**
 * @brief Platform specific functions which should be provided by a user
 */
uint32_t uavcanGetTimeMs() {
    static auto start_time = std::chrono::high_resolution_clock::now();
    auto crnt_time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(crnt_time - start_time).count();
}


/**
 * @brief Main application entry point
 */
int main() {
    paramsInit(1, 1, -1, 1);
    auto init_res = uavcanInitApplication(paramsGetIntegerValue(0));
    if (init_res < 0) {
        std::cout << "CAN interface could not be found. Exit with code " << init_res << std::endl;
        return init_res;
    }

    LightsCommandSubscriber lights_command_sub;
    lights_command_sub.init();

    ArrayCommandSubscriber array_command_sub;
    array_command_sub.init();

    RawCommandSubscriber raw_command_sub;
    raw_command_sub.init();

    while (uavcanGetTimeMs() < 5000) {
        uavcanSpinOnce(uavcanGetTimeMs());
    }

    std::cout << "Good. Enough." << std::endl;
    return 0;
}
