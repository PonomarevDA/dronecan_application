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
#include "uavcan/equipment/indication/LightsCommand.h"
#include "storage.h"

IntegerDesc_t __attribute__((weak)) integer_desc_pool[] = {
    {"uavcan.node.id",              0,      100,    50},
};
IntegerParamValue_t integer_values_pool[sizeof(integer_desc_pool) / sizeof(IntegerDesc_t)];

StringDesc_t __attribute__((weak)) string_desc_pool[] = {
    {"system.name",              "dronecan_application"},
};
StringParamValue_t string_values_pool[sizeof(string_desc_pool) / sizeof(StringDesc_t)];


/**
 * @brief Platform specific functions which should be provided by a user
 */
uint32_t uavcanGetTimeMs() {
    static auto start_time = std::chrono::high_resolution_clock::now();
    auto crnt_time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(crnt_time - start_time).count();
}


/**
 * @brief Just an example of encapsulated LightsCommand subscriber
 */
class LightsCommandSubscriber {
public:
    LightsCommandSubscriber() = default;
    int8_t init();
    static void callback(CanardRxTransfer* transfer);
    static LightsCommand_t lights_command;
private:
    uint32_t next_pub_time_ms = 500;
    uint8_t inout_transfer_id;
};

int8_t LightsCommandSubscriber::init() {
    int8_t sub_id = uavcanSubscribe(UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND_SIGNATURE,
                                    UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND_ID,
                                    LightsCommandSubscriber::callback);
    if (sub_id < 0) {
        std::cout << "Something wrong. Sobescriber has not been created. Abort." << sub_id << std::endl;
    }
    return sub_id;
}
void LightsCommandSubscriber::callback(CanardRxTransfer* transfer) {
    uint8_t cmds_amount = dronecan_equipment_indication_lights_command_deserialize(transfer, &lights_command);
    std::cout << "Get LightsCommand with " << (int)cmds_amount << " commands." << std::endl;
}

LightsCommand_t LightsCommandSubscriber::lights_command = {};


/**
 * @brief Main application entry point
 */
int main (int argc, char *argv[]) {
    paramsInit(1, 1);
    auto init_res = uavcanInitApplication(paramsGetIntegerValue(0));
    if (init_res < 0) {
        std::cout << "CAN interface could not be found. Exit with code " << init_res << std::endl;
        return init_res;
    }

    LightsCommandSubscriber lights_command_sub;
    lights_command_sub.init();

    while (uavcanGetTimeMs() < 5000) {
        uavcanSpinOnce();
    }

    std::cout << "Good. Enough." << std::endl;
    return 0;
}
