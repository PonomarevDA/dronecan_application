/*
 * Copyright (C) 2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <assert.h>
#include <string.h>
#include <iostream>
#include <memory>
#include <chrono>
#include "storage.h"
#include "dcnode/dcnode.h"

/**
 * @brief Platform specific functions which should be provided by a user
 */
static uint32_t platformSpecificGetTimeMs() {
    static auto start_time = std::chrono::high_resolution_clock::now();
    auto crnt_time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(crnt_time - start_time).count();
}

static bool platformSpecificRequestRestart() {
    std::cout << "Reboot is not supported by this platform" << std::endl;
    return false;
}

void platformSpecificReadUniqueID(uint8_t out_uid[16]) {
    const uint8_t UID[16] = {
        0x00, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B,
        0x0C, 0x0D, 0x0E, 0x0F,
    };
    memcpy(out_uid, UID, 16);
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

    PlatformHooks platform_hooks = {
        .getTimeMs = platformSpecificGetTimeMs,
        .requestRestart = platformSpecificRequestRestart,
        .readUniqueID = platformSpecificReadUniqueID
    };

    auto init_res = DronecanNode::init(platform_hooks, 42);
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

    std::array<std::unique_ptr<BaseDronecanPeriodicPublisher>, 21> publishers = {
        std::make_unique<DronecanPeriodicPublisher<Hygrometer>>(1.0f),
        std::make_unique<DronecanPeriodicPublisher<ActuatorStatus_t>>(1.0f),
        std::make_unique<DronecanPeriodicPublisher<MagneticFieldStrength2>>(1.0f),
        std::make_unique<DronecanPeriodicPublisher<AhrsRawImu>>(1.0f),
        std::make_unique<DronecanPeriodicPublisher<AhrsSolution_t>>(1.0f),
        std::make_unique<DronecanPeriodicPublisher<IndicatedAirspeed>>(1.0f),
        std::make_unique<DronecanPeriodicPublisher<RawAirData_t>>(1.0f),
        std::make_unique<DronecanPeriodicPublisher<StaticPressure>>(1.0f),
        std::make_unique<DronecanPeriodicPublisher<StaticTemperature>>(1.0f),
        std::make_unique<DronecanPeriodicPublisher<TrueAirspeed>>(1.0f),
        std::make_unique<DronecanPeriodicPublisher<Temperature_t>>(1.0f),
        std::make_unique<DronecanPeriodicPublisher<EscStatus_t>>(1.0f),
        std::make_unique<DronecanPeriodicPublisher<GnssFix2>>(1.0f),
        std::make_unique<DronecanPeriodicPublisher<HardpointStatus>>(1.0f),
        std::make_unique<DronecanPeriodicPublisher<FuelTankStatus_t>>(1.0f),
        std::make_unique<DronecanPeriodicPublisher<IceReciprocatingStatus>>(1.0f),
        std::make_unique<DronecanPeriodicPublisher<IceReciprocatingStatus>>(1.0f),
        std::make_unique<DronecanPeriodicPublisher<CircuitStatus_t>>(1.0f),
        std::make_unique<DronecanPeriodicPublisher<BatteryInfo_t>>(1.0f),
        std::make_unique<DronecanPeriodicPublisher<LightsCommand_t>>(1.0f),
        std::make_unique<DronecanPeriodicPublisher<RangeSensorMeasurement_t>>(1.0f),
    };

    while (platformSpecificGetTimeMs() < 50000) {
        for (auto& pub : publishers) {
            pub->spinOnce();
        }

        DronecanNode::spinOnce();
    }

    std::cout << "Good. Enough." << std::endl;
    return 0;
}
