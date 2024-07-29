/*
 * Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef SERIALZIATION_PUBLISHER_HPP_
#define SERIALZIATION_PUBLISHER_HPP_

#include <stdint.h>
#include <algorithm>
#include "dronecan.h"
#include "uavcan/equipment/power/CircuitStatus.h"
#include "uavcan/equipment/power/BatteryInfo.h"
#include "uavcan/equipment/temperature/Temperature.h"

template <typename MessageType>
struct DronecanPublisherTraits;

template <>
struct DronecanPublisherTraits<CircuitStatus_t> {
    static inline int8_t publish_once(const CircuitStatus_t& msg, uint8_t* inout_transfer_id) {
        return dronecan_equipment_circuit_status_publish(&msg, inout_transfer_id);
    }
};

template <>
struct DronecanPublisherTraits<Temperature_t> {
    static inline int8_t publish_once(const Temperature_t& msg, uint8_t* inout_transfer_id) {
        return dronecan_equipment_temperature_publish(&msg, inout_transfer_id);
    }
};

template <>
struct DronecanPublisherTraits<BatteryInfo_t> {
    static inline int8_t publish_once(const BatteryInfo_t& msg, uint8_t* inout_transfer_id) {
        return dronecan_equipment_battery_info_publish(&msg, inout_transfer_id);
    }
};

template <typename MessageType>
class DronecanPublisher {
public:
    DronecanPublisher() = default;

    inline void publish() {
        DronecanPublisherTraits<MessageType>::publish_once(msg, &inout_transfer_id);
        inout_transfer_id++;
    }

    MessageType msg;
private:
    uint8_t inout_transfer_id;
};


template <typename MessageType>
class DronecanPeriodicPublisher : public DronecanPublisher<MessageType> {
public:
    DronecanPeriodicPublisher(float frequency) :
        DronecanPublisher<MessageType>(),
        PUB_PERIOD_MS(1000.0f / std::clamp(frequency, 0.001f, 100.0f)) {};

    inline void spinOnce() {
        auto crnt_time_ms = uavcanGetTimeMs();
        if (crnt_time_ms < next_pub_time_ms) {
            return;
        }
        next_pub_time_ms = crnt_time_ms + PUB_PERIOD_MS;

        this->publish();
    }

private:
    const uint32_t PUB_PERIOD_MS{1000};
    uint32_t next_pub_time_ms = PUB_PERIOD_MS;
};

#endif  // SERIALZIATION_PUBLISHER_HPP_
