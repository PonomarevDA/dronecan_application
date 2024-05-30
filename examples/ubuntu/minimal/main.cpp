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
#include "storage.h"
#include "flash_driver.h"

IntegerDesc_t __attribute__((weak)) integer_desc_pool[] = {
    {"uavcan.node.id", 0, 100, 50, true, false},
};
IntegerParamValue_t integer_values_pool[sizeof(integer_desc_pool) / sizeof(IntegerDesc_t)];
StringDesc_t __attribute__((weak)) string_desc_pool[] = {
    {"system.name", "dronecan_application", true},
};
StringParamValue_t string_values_pool[sizeof(string_desc_pool) / sizeof(StringDesc_t)];


uint32_t uavcanGetTimeMs() {
    static auto start_time = std::chrono::high_resolution_clock::now();
    auto crnt_time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(crnt_time - start_time).count();
}

int main() {
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
