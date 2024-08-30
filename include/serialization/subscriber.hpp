/*
 * Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef SERIALZIATION_SUBSCRIBER_HPP_
#define SERIALZIATION_SUBSCRIBER_HPP_

#include <stdint.h>
#include <array>
#include "dronecan.h"
#include "uavcan/equipment/esc/RawCommand.h"
#include "uavcan/equipment/actuator/ArrayCommand.h"
#include "uavcan/equipment/indication/LightsCommand.h"
#include "uavcan/equipment/power/CircuitStatus.h"
#include "uavcan/equipment/safety/ArmingStatus.h"


template <typename MessageType>
struct DronecanSubscriberTraits;

#define DEFINE_SUBSCRIBER_TRAITS(MessageType, SubscribeFunction, DeserializeFunction) \
template <> \
struct DronecanSubscriberTraits<MessageType> { \
    static inline int8_t subscribe(void (*callback)(CanardRxTransfer*)) { \
        return SubscribeFunction(callback); \
    } \
    static inline int8_t deserialize(CanardRxTransfer* transfer, MessageType* msg) { \
        return DeserializeFunction(transfer, msg); \
    } \
};

DEFINE_SUBSCRIBER_TRAITS(RawCommand_t,
                         uavcanSubscribeEscRawCommand,
                         dronecan_equipment_esc_raw_command_deserialize)
DEFINE_SUBSCRIBER_TRAITS(ArrayCommand_t,
                         uavcanSubscribeActuatorArrayCommand,
                         dronecan_equipment_actuator_arraycommand_deserialize)
DEFINE_SUBSCRIBER_TRAITS(LightsCommand_t,
                         uavcanSubscribeIndicationLightsCommand,
                         dronecan_equipment_indication_lights_command_deserialize)
DEFINE_SUBSCRIBER_TRAITS(ArmingStatus,
                         uavcanSubscribeArmingStatus,
                         dronecan_equipment_safety_arming_status_deserialize)


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

#endif  // SERIALZIATION_SUBSCRIBER_HPP_
