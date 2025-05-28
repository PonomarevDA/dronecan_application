/*
 * Copyright (C) 2023-2025 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * DESIGN NOTES:
 * - Designed for deeply embedded systems.
 * - No dynamic memory allocation; no heap usage.
 * - No use of RTTI (Run-Time Type Information).
 * - No use of C++ exceptions.
 * - All callbacks must be non-blocking and complete as fast as possible.
 * - API is **not** thread-safe; assumes single-threaded or cooperative environment.
 */

#ifndef DCNODE_DCNODE_H_
#define DCNODE_DCNODE_H_

#include <stdint.h>
#include <array>
#include <algorithm>
#include "dcnode/data_types.hpp"

#ifndef DRONECAN_MAX_SUBS_AMOUNT
    #define DRONECAN_MAX_SUBS_AMOUNT     10
#endif

using GetTimeMsFunc = uint32_t (*)();
using RequestRestartFunc = bool (*)();
using ReadUniqueIDFunc = void (*)(uint8_t out_uid[16]);

/**
 * PlatformHooks - a structure holds function pointers that allow the
 * application to interact with platform-specific features.
 *
 * - getTimeMs (required):
 *   A function that returns the number of milliseconds elapsed
 *   since the application started.
 *   The user must provide an implementation for this.
 *
 * - requestRestart (optional):
 *   A function that, when called, attempts to request or trigger
 *   a system/application restart.
 *   Providing this implementation is recommended, but optional.
 *
 * - readUniqueID (optional):
 *   A function that retrieves the hardware’s unique 16-byte identifier
 *   and writes it into the provided out_uid buffer.
 *   Providing this implementation is recommended, but optional.
 */
typedef struct {
    GetTimeMsFunc getTimeMs;
    RequestRestartFunc requestRestart;
    ReadUniqueIDFunc readUniqueID;
} PlatformHooks;

class DronecanNode {
public:
    /**
      * @brief Initialize the node and minimal required services
      * @return 0 on success, otherwise negative error
      */
    static int16_t init(const PlatformHooks& platform_hooks, uint8_t node_id);

    /**
      * @brief This should be periodically called to handle the application.
      */
    static void spinOnce();

    /**
    * @brief NodeStatus
    */
    static void setNodeHealth(NodeStatusHealth_t health);
    static NodeStatusHealth_t getNodeHealth();

    static NodeStatusMode_t getNodeStatusMode();
    static void setNodeStatusMode(NodeStatusMode_t mode);

    static void setVendorSpecificStatusCode(uint16_t vssc);
    static uint16_t getVendorSpecificStatusCode();

    /**
    * @brief NodeInfo API
    */
    static auto getNodeName() -> const char*;
    static auto setNodeName(const char* new_node_name) -> void;

    static void configure(const SoftwareVersion* new_sw_vers, const HardwareVersion* new_hw_vers);


    /**
    * @note TransportStats API
    */
    static void statsIncreaseCanErrors();
    static void statsIncreaseCanTx(uint8_t num_of_transfers);
    static void statsIncreaseCanRx();
    static void statsIncreaseUartErrors();
    static void statsIncreaseUartTx(uint32_t num);
    static void statsIncreaseUartRx(uint32_t num);
    static uint64_t getErrorCount();

public:
    /**
    * @brief Call this function once per each subscriber.
    * The application will automatically handle callbacks.
    * Callbacks should end ASAP.
    */
    static int8_t subscribe(uint64_t signature,
                            uint16_t id,
                            void (callback)(CanardRxTransfer* transfer));

    /**
    * @brief Broadcast a message.
    */
    static int16_t publish(uint64_t data_type_signature,
                           uint16_t data_type_id,
                           uint8_t* inout_transfer_id,
                           uint8_t priority,
                           const void* payload,
                           uint16_t payload_len);

    /**
    * @brief Respond on RPC-request.
    */
    static void respond(CanardRxTransfer* transfer,
                        uint64_t data_type_signature,
                        uint16_t data_type_id,
                        const uint8_t* payload,
                        uint16_t len);

private:
    template <typename MessageType>
    friend struct DronecanPeriodicPublisher;

    template <typename MessageType>
    friend struct DronecanSubscriberTraits;

    template <typename MessageType>
    friend struct DronecanPublisherTraits;

    static uint32_t getTimeMs();
};

template <typename MessageType>
struct DronecanSubscriberTraits;

#define DEFINE_SUBSCRIBER_TRAITS(MessageType, DronecanConfig, DeserializeFunction) \
template <> \
struct DronecanSubscriberTraits<MessageType> { \
    static inline int8_t subscribe(void (*callback)(CanardRxTransfer*)) { \
        return DronecanNode::subscribe(DronecanConfig, callback); \
    } \
    static inline int8_t deserialize(const CanardRxTransfer* transfer, MessageType* msg) { \
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

template <typename MessageType>
struct DronecanPublisherTraits;

#define DEFINE_PUBLISHER_TRAITS(MessageType, SerializeFunction, Signature, Id, MessageSize) \
template <> \
struct DronecanPublisherTraits<MessageType> { \
    static inline int8_t publish(const MessageType* const obj, uint8_t* inout_transfer_id) { \
        uint8_t buffer[MessageSize]; \
        uint32_t number_of_bytes = SerializeFunction(obj, buffer); \
        if (number_of_bytes > MessageSize || number_of_bytes == 0) {\
            return -1;\
        }\
        int16_t res = DronecanNode::publish(Signature, \
                                    Id, \
                                    inout_transfer_id, \
                                    CANARD_TRANSFER_PRIORITY_MEDIUM, \
                                    buffer, \
                                    number_of_bytes); \
        return res; \
    } \
    static inline int8_t publish_once(const MessageType& msg, uint8_t* inout_transfer_id) { \
        return publish(&msg, inout_transfer_id); \
    } \
};

DEFINE_PUBLISHER_TRAITS(Hygrometer,
                        dronecan_sensors_hygrometer_hygrometer_serialize,
                        DRONECAN_SENSORS_HYGROMETER_HYGROMETER_SIGNATURE,
                        DRONECAN_SENSORS_HYGROMETER_HYGROMETER_ID,
                        DRONECAN_SENSORS_HYGROMETER_HYGROMETER_MESSAGE_SIZE
)
DEFINE_PUBLISHER_TRAITS(ActuatorStatus_t,
                        dronecan_equipment_actuator_status_serialize,
                        UAVCAN_EQUIPMENT_ACTUATOR_STATUS_SIGNATURE,
                        UAVCAN_EQUIPMENT_ACTUATOR_STATUS_ID,
                        UAVCAN_EQUIPMENT_ACTUATOR_STATUS_MESSAGE_SIZE
)
DEFINE_PUBLISHER_TRAITS(MagneticFieldStrength2,
                        dronecan_equipment_ahrs_magnetic_field_2_serialize,
                        UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH2_SIGNATURE,
                        UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH2_ID,
                        UAVCAN_EQUIPMENT_AHRS_MAGNETIC_FIELD_STRENGTH2_MESSAGE_SIZE
)
DEFINE_PUBLISHER_TRAITS(AhrsRawImu,
                        dronecan_equipment_ahrs_raw_imu_serialize,
                        UAVCAN_EQUIPMENT_AHRS_RAW_IMU_SIGNATURE,
                        UAVCAN_EQUIPMENT_AHRS_RAW_IMU_ID,
                        UAVCAN_EQUIPMENT_AHRS_RAW_IMU_MESSAGE_SIZE
)
DEFINE_PUBLISHER_TRAITS(AhrsSolution_t,
                        dronecan_equipment_ahrs_solution_serialize,
                        UAVCAN_EQUIPMENT_AHRS_SOLUTION_SIGNATURE,
                        UAVCAN_EQUIPMENT_AHRS_SOLUTION_ID,
                        UAVCAN_EQUIPMENT_AHRS_SOLUTION_MESSAGE_SIZE
)
DEFINE_PUBLISHER_TRAITS(IndicatedAirspeed,
                        dronecan_equipment_air_data_indicated_airspeed_serialize,
                        UAVCAN_EQUIPMENT_AIR_DATA_INDICATED_AIRSPEED_SIGNATURE,
                        UAVCAN_EQUIPMENT_AIR_DATA_INDICATED_AIRSPEED_ID,
                        UAVCAN_EQUIPMENT_AIR_DATA_INDICATED_AIRSPEED_MESSAGE_SIZE
)
DEFINE_PUBLISHER_TRAITS(RawAirData_t,
                        dronecan_equipment_air_data_raw_air_data_serialize,
                        UAVCAN_EQUIPMENT_AIR_DATA_RAW_AIR_DATA_SIGNATURE,
                        UAVCAN_EQUIPMENT_AIR_DATA_RAW_AIR_DATA_ID,
                        UAVCAN_EQUIPMENT_AIR_DATA_RAW_AIR_DATA_MESSAGE_SIZE
)
DEFINE_PUBLISHER_TRAITS(StaticPressure,
                        dronecan_equipment_air_data_static_pressure_serialize,
                        UAVCAN_EQUIPMENT_AIR_DATA_STATIC_PRESSURE_SIGNATURE,
                        UAVCAN_EQUIPMENT_AIR_DATA_STATIC_PRESSURE_ID,
                        UAVCAN_EQUIPMENT_AIR_DATA_STATIC_PRESSURE_MESSAGE_SIZE
)
DEFINE_PUBLISHER_TRAITS(StaticTemperature,
                        dronecan_equipment_air_data_static_temperature_serialize,
                        UAVCAN_EQUIPMENT_AIR_DATA_STATIC_TEMPERATURE_SIGNATURE,
                        UAVCAN_EQUIPMENT_AIR_DATA_STATIC_TEMPERATURE_ID,
                        UAVCAN_EQUIPMENT_AIR_DATA_STATIC_TEMPERATURE_MESSAGE_SIZE
)
DEFINE_PUBLISHER_TRAITS(TrueAirspeed,
                        dronecan_equipment_air_data_true_airspeed_serialize,
                        UAVCAN_EQUIPMENT_AIR_DATA_TRUE_AIRSPEED_SIGNATURE,
                        UAVCAN_EQUIPMENT_AIR_DATA_TRUE_AIRSPEED_ID,
                        UAVCAN_EQUIPMENT_AIR_DATA_TRUE_AIRSPEED_MESSAGE_SIZE
)
DEFINE_PUBLISHER_TRAITS(Temperature_t,
                        dronecan_equipment_temperature_serialize,
                        UAVCAN_EQUIPMENT_DEVICE_TEMPERATURE_SIGNATURE,
                        UAVCAN_EQUIPMENT_DEVICE_TEMPERATURE_ID,
                        UAVCAN_EQUIPMENT_DEVICE_TEMPERATURE_MESSAGE_SIZE
)
DEFINE_PUBLISHER_TRAITS(EscStatus_t,
                        dronecan_equipment_esc_status_serialize,
                        UAVCAN_EQUIPMENT_ESC_STATUS_SIGNATURE,
                        UAVCAN_EQUIPMENT_ESC_STATUS_ID,
                        UAVCAN_EQUIPMENT_ESC_STATUS_MESSAGE_SIZE
)
DEFINE_PUBLISHER_TRAITS(GnssFix2,
                        dronecan_equipment_gnss_fix2_serialize,
                        UAVCAN_EQUIPMENT_GNSS_FIX2_SIGNATURE,
                        UAVCAN_EQUIPMENT_GNSS_FIX2_ID,
                        UAVCAN_EQUIPMENT_GNSS_FIX2_MESSAGE_SIZE
)
DEFINE_PUBLISHER_TRAITS(HardpointStatus,
                        dronecan_equipment_hardpoint_status_serialize,
                        UAVCAN_EQUIPMENT_HARDPOINT_STATUS_SIGNATURE,
                        UAVCAN_EQUIPMENT_HARDPOINT_STATUS_ID,
                        UAVCAN_EQUIPMENT_HARDPOINT_STATUS_MESSAGE_SIZE
)
DEFINE_PUBLISHER_TRAITS(FuelTankStatus_t,
                        dronecan_equipment_ice_fuel_tank_status_serialize,
                        UAVCAN_EQUIPMENT_ICE_FUELTANKSTATUS_SIGNATURE,
                        UAVCAN_EQUIPMENT_ICE_FUELTANKSTATUS_ID,
                        UAVCAN_EQUIPMENT_ICE_FUELTANKSTATUS_MESSAGE_SIZE
)
DEFINE_PUBLISHER_TRAITS(IceReciprocatingStatus,
                        dronecan_equipment_ice_status_serialize,
                        UAVCAN_EQUIPMENT_ICE_RECIPROCATING_STATUS_SIGNATURE,
                        UAVCAN_EQUIPMENT_ICE_RECIPROCATING_STATUS_ID,
                        UAVCAN_EQUIPMENT_ICE_RECIPROCATING_STATUS_MESSAGE_SIZE
)
DEFINE_PUBLISHER_TRAITS(CircuitStatus_t,
                        dronecan_equipment_power_circuit_status_serialize,
                        UAVCAN_EQUIPMENT_POWER_CIRCUIT_STATUS_SIGNATURE,
                        UAVCAN_EQUIPMENT_POWER_CIRCUIT_STATUS_ID,
                        UAVCAN_EQUIPMENT_POWER_CIRCUIT_STATUS_MESSAGE_SIZE
)
DEFINE_PUBLISHER_TRAITS(BatteryInfo_t,
                        dronecan_equipment_power_battery_info_serialize,
                        UAVCAN_EQUIPMENT_POWER_BATTERY_INFO_SIGNATURE,
                        UAVCAN_EQUIPMENT_POWER_BATTERY_INFO_ID,
                        UAVCAN_EQUIPMENT_POWER_BATTERY_INFO_MESSAGE_SIZE
)
DEFINE_PUBLISHER_TRAITS(LightsCommand_t,
                        dronecan_equipment_indication_lights_command_serialize,
                        UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND_SIGNATURE,
                        UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND_ID,
                        UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND_MESSAGE_SIZE
)
DEFINE_PUBLISHER_TRAITS(RangeSensorMeasurement_t,
                        dronecan_equipment_range_sensor_measurement_serialize,
                        UAVCAN_EQUIPMENT_RANGE_SENSOR_MEASUREMENT_SIGNATURE,
                        UAVCAN_EQUIPMENT_RANGE_SENSOR_MEASUREMENT_ID,
                        UAVCAN_EQUIPMENT_RANGE_SENSOR_MEASUREMENT_MESSAGE_SIZE
)


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
    uint8_t inout_transfer_id{0};
};

class BaseDronecanPeriodicPublisher {
public:
    virtual void spinOnce() = 0;
};

template <typename MessageType>
class DronecanPeriodicPublisher : public BaseDronecanPeriodicPublisher, DronecanPublisher<MessageType> {
public:
    explicit DronecanPeriodicPublisher(float frequency) :
        DronecanPublisher<MessageType>(),
        PUB_PERIOD_MS(static_cast<uint32_t>(1000.0f / std::clamp(frequency, 0.001f, 1000.0f))) {};

    inline void spinOnce() override {
        auto crnt_time_ms = DronecanNode::getTimeMs();
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

#endif  // DCNODE_DCNODE_H_
