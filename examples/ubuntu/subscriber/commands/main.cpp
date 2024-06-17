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
#include "uavcan/equipment/esc/RawCommand.h"
#include "uavcan/equipment/actuator/ArrayCommand.h"
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
 * @brief Just an example of encapsulated ArrayCommand subscriber
 */
class ArrayCommandSubscriber {
public:
    ArrayCommandSubscriber() = default;
    int8_t init();
    static void callback(CanardRxTransfer* transfer);
    static ArrayCommand_t array_command;
private:
    uint32_t next_pub_time_ms = 500;
    uint8_t inout_transfer_id;
};

int8_t ArrayCommandSubscriber::init() {
    int8_t sub_id = uavcanSubscribe(UAVCAN_EQUIPMENT_ACTUATOR_ARRAY_COMMAND, ArrayCommandSubscriber::callback);
    if (sub_id < 0) {
        std::cout << "Something wrong. Sobescriber has not been created. Abort." << sub_id << std::endl;
    }
    return sub_id;
}
void ArrayCommandSubscriber::callback(CanardRxTransfer* transfer) {
    uint8_t cmds_amount = dronecan_equipment_actuator_arraycommand_deserialize(transfer, &array_command);
    std::cout << "Get ArrayCommand with " << (int)cmds_amount << " commands." << std::endl;
}

ArrayCommand_t ArrayCommandSubscriber::array_command = {};


/**
 * @brief Just an example of encapsulated RawCommand subscriber
 */
class RawCommandSubscriber {
public:
    RawCommandSubscriber() = default;
    int8_t init();
    static void callback(CanardRxTransfer* transfer);
    static RawCommand_t raw_command;
private:
    uint32_t next_pub_time_ms = 500;
    uint8_t inout_transfer_id;
};

int8_t RawCommandSubscriber::init() {
    int8_t sub_id = uavcanSubscribe(UAVCAN_EQUIPMENT_ESC_RAWCOMMAND, RawCommandSubscriber::callback);
    if (sub_id < 0) {
        std::cout << "Something wrong. Sobescriber has not been created. Abort." << sub_id << std::endl;
    }
    return sub_id;
}
void RawCommandSubscriber::callback(CanardRxTransfer* transfer) {
    uint8_t cmds_amount = dronecan_equipment_esc_raw_command_deserialize(transfer, &raw_command);
    std::cout << "Get RawCommand with " << (int)cmds_amount << " commands." << std::endl;
}

RawCommand_t RawCommandSubscriber::raw_command = {};


/**
 * @brief Main application entry point
 */
int main (int argc, char *argv[]) {
    paramsInit(1, 1, -1, 1);
    auto init_res = uavcanInitApplication(paramsGetIntegerValue(0));
    if (init_res < 0) {
        std::cout << "CAN interface could not be found. Exit with code " << init_res << std::endl;
        return init_res;
    }

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
