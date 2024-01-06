/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <iostream>
#include <chrono>
#include <string.h>
#include "dronecan.h"

uint32_t uavcanGetTimeMs() {
    static auto start_time = std::chrono::high_resolution_clock::now();
    auto crnt_time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(crnt_time - start_time).count();
}

int main (int argc, char *argv[]) {
    const uint8_t node_id = 42;
    auto init_res = uavcanInitApplication(node_id);
    if (init_res < 0) {
        std::cout << "CAN interface could be found. Exit with code " << init_res << std::endl;
        return init_res;
    }

    for (int idx = 0; idx < 5000; idx++) {
        uavcanSpinOnce();
    }

    std::cout << "Good." << std::endl;
    return 0;
}
