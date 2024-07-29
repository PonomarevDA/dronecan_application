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
#include "uavcan/equipment/actuator/Status.h"
#include "uavcan/equipment/esc/Status.h"
#include "uavcan/equipment/hardpoint/Status.h"

template <typename MessageType>
struct DronecanPublisherTraits;

#define DEFINE_PUBLISHER_TRAITS(MessageType, PublishFunction) \
template <> \
struct DronecanPublisherTraits<MessageType> { \
    static inline int8_t publish_once(const MessageType& msg, uint8_t* inout_transfer_id) { \
        return PublishFunction(&msg, inout_transfer_id); \
    } \
};

DEFINE_PUBLISHER_TRAITS(ActuatorStatus_t, dronecan_equipment_actuator_status_publish)
DEFINE_PUBLISHER_TRAITS(CircuitStatus_t, dronecan_equipment_circuit_status_publish)
DEFINE_PUBLISHER_TRAITS(Temperature_t, dronecan_equipment_temperature_publish)
DEFINE_PUBLISHER_TRAITS(BatteryInfo_t, dronecan_equipment_battery_info_publish)
DEFINE_PUBLISHER_TRAITS(EscStatus_t, dronecan_equipment_esc_status_publish)
DEFINE_PUBLISHER_TRAITS(HardpointStatus, dronecan_equipment_hardpoint_status_publish)


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
        PUB_PERIOD_MS(static_cast<uint32_t>(1000.0f / std::clamp(frequency, 0.001f, 100.0f))) {};

    inline void spinOnce() {
        auto crnt_time_ms = uavcanGetTimeMs();
        if (crnt_time_ms < next_pub_time_ms) {
            return;
        }
        next_pub_time_ms = crnt_time_ms + PUB_PERIOD_MS;

        this->publish();
    }

private:
    const uint32_t PUB_PERIOD_MS;
    uint32_t next_pub_time_ms{500};
};

#endif  // SERIALZIATION_PUBLISHER_HPP_
