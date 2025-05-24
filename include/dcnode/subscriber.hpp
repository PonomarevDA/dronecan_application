/*
 * Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef DCNODE_SUBSCRIBER_HPP_
#define DCNODE_SUBSCRIBER_HPP_

#include <stdint.h>
#include <array>
#include "uavcan/equipment/esc/RawCommand.h"
#include "uavcan/equipment/actuator/ArrayCommand.h"
#include "uavcan/equipment/indication/BeepCommand.h"
#include "uavcan/equipment/indication/LightsCommand.h"
#include "uavcan/equipment/power/CircuitStatus.h"
#include "uavcan/equipment/safety/ArmingStatus.h"
#include "uavcan/equipment/ahrs/Solution.h"
#include "uavcan/equipment/hardpoint/Command.h"

template <typename MessageType>
struct DronecanSubscriberTraits;

#define DEFINE_SUBSCRIBER_TRAITS(MessageType, DronecanConfig, DeserializeFunction) \
template <> \
struct DronecanSubscriberTraits<MessageType> { \
    static inline int8_t subscribe(void (*callback)(CanardRxTransfer*)) { \
        return uavcanSubscribe(DronecanConfig, callback); \
    } \
    static inline int8_t deserialize(CanardRxTransfer* transfer, MessageType* msg) { \
        return DeserializeFunction(transfer, msg); \
    } \
};

DEFINE_SUBSCRIBER_TRAITS(RawCommand_t,
                         UAVCAN_EQUIPMENT_ESC_RAWCOMMAND,
                         dronecan_equipment_esc_raw_command_deserialize)
DEFINE_SUBSCRIBER_TRAITS(ArrayCommand_t,
                         UAVCAN_EQUIPMENT_ACTUATOR_ARRAY_COMMAND,
                         dronecan_equipment_actuator_arraycommand_deserialize)
DEFINE_SUBSCRIBER_TRAITS(BeepCommand_t,
                         UAVCAN_EQUIPMENT_INDICATION_BEEPCOMMAND,
                         dronecan_equipment_indication_beep_command_deserialize)
DEFINE_SUBSCRIBER_TRAITS(LightsCommand_t,
                         UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND,
                         dronecan_equipment_indication_lights_command_deserialize)
DEFINE_SUBSCRIBER_TRAITS(SafetyArmingStatus,
                         UAVCAN_EQUIPMENT_SAFETY_ARMING_STATUS,
                         dronecan_equipment_safety_arming_status_deserialize)
DEFINE_SUBSCRIBER_TRAITS(HardpointCommand,
                         UAVCAN_EQUIPMENT_HARDPOINT_COMMAND,
                         dronecan_equipment_hardpoint_command_deserialize)
DEFINE_SUBSCRIBER_TRAITS(AhrsSolution_t,
                         UAVCAN_EQUIPMENT_AHRS_SOLUTION,
                         dronecan_equipment_ahrs_solution_deserialize)

template <typename MessageType>
class DronecanSubscriber {
public:
    DronecanSubscriber() = default;

    int8_t init(void (*callback)(const MessageType&), bool (*filter_)(const MessageType&)=nullptr) {
        user_callback = callback;
        filter = filter_;
        auto sub_id = DronecanSubscriberTraits<MessageType>::subscribe(transfer_callback);
        instances[sub_id] = this;
        return sub_id;
    }

    static inline void transfer_callback(CanardRxTransfer* transfer) {
        int8_t res = DronecanSubscriberTraits<MessageType>::deserialize(transfer, &msg);
        if (res < 0) {
            return;
        }

        auto instance = static_cast<DronecanSubscriber*>(instances[transfer->sub_id]);
        if (instance == nullptr) {
            return;
        }

        if (instance->filter != nullptr && !instance->filter(msg)) {
            return;
        }

        instance->user_callback(msg);
    }

    static inline std::array<void*, DRONECAN_MAX_SUBS_AMOUNT> instances{};
    static inline MessageType msg = {};
    void (*user_callback)(const MessageType&){nullptr};
    bool (*filter)(const MessageType&){nullptr};
};

#endif  // DCNODE_SUBSCRIBER_HPP_
