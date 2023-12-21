/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <iostream>
#include <chrono>
#include <string.h>
#include "uavcan.h"

uint32_t uavcanGetTimeMs() {
    static auto start_time = std::chrono::high_resolution_clock::now();
    auto crnt_time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(crnt_time - start_time).count();
}

int main (int argc, char *argv[]) {
    auto init_res = uavcanInit(42);
    if (init_res < 0) {
        std::cout << "CAN interface could be found. Exit with code " << init_res << std::endl;
        return init_res;
    }

    for (int idx = 0; idx < 100; idx++) {
        uavcanProcessSending();
        uavcanProcessReceiving();
        canardSpinNodeStatus();
    }

    std::cout << "Good." << std::endl;
    return 0;
}
