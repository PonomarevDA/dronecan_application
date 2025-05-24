/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef DCNODE_DCNODE_H_
#define DCNODE_DCNODE_H_

#include <stdint.h>
#include <array>
#include <algorithm>
#include "uavcan/equipment/actuator/Status.h"
#include "uavcan/equipment/ahrs/MagneticFieldStrength2.h"
#include "uavcan/equipment/ahrs/RawImu.h"
#include "uavcan/equipment/ahrs/Solution.h"
#include "uavcan/equipment/air_data/IndicatedAirspeed.h"
#include "uavcan/equipment/air_data/RawAirData.h"
#include "uavcan/equipment/air_data/StaticPressure.h"
#include "uavcan/equipment/air_data/StaticTemperature.h"
#include "uavcan/equipment/air_data/TrueAirspeed.h"
#include "uavcan/equipment/esc/Status.h"
#include "uavcan/equipment/gnss/Fix2.h"
#include "uavcan/equipment/hardpoint/Status.h"
#include "uavcan/equipment/ice/FuelTankStatus.h"
#include "uavcan/equipment/ice/Status.h"
#include "uavcan/equipment/power/CircuitStatus.h"
#include "uavcan/equipment/power/BatteryInfo.h"
#include "uavcan/equipment/device/Temperature.h"
#include "dronecan/sensors/hygrometer/Hygrometer.h"
#include "uavcan/equipment/indication/LightsCommand.h"
#include "uavcan/equipment/range_sensor/Measurement.h"
#include "uavcan/protocol/node_status.h"
#include "uavcan/protocol/get_node_info.h"
#include "uavcan/equipment/esc/RawCommand.h"
#include "uavcan/equipment/actuator/ArrayCommand.h"
#include "uavcan/equipment/indication/BeepCommand.h"
#include "uavcan/equipment/safety/ArmingStatus.h"
#include "uavcan/equipment/hardpoint/Command.h"

#ifndef DRONECAN_MAX_SUBS_AMOUNT
    #define DRONECAN_MAX_SUBS_AMOUNT     10
#endif

typedef uint32_t (*GetTimeMsFunc)(void);
typedef bool (*RequestRestartFunc)(void);
typedef void (*ReadUniqueIDFunc)(uint8_t out_uid[16]);

typedef struct {
    // The time in milliseconds since the application started.
    // This function must be provided by a user!
    GetTimeMsFunc getTimeMs;

    // Implementation is recommended, but optional.
    RequestRestartFunc requestRestart;

    // Hardware Unique ID.
    // Implementation is recommended, but optional.
    ReadUniqueIDFunc readUniqueID;
} PlatformHooks;

/**
  * @brief Initialize the node and minimal required services
  * @return 0 on success, otherwise negative error
  */
int16_t uavcanInitApplication(PlatformHooks platform_hooks, uint8_t node_id);


/**
  * @brief Functions below should be called periodically to handle the application.
  */
void uavcanSpinOnce();


/**
  * @brief Call this function once per each subscriber.
  * The application will automatically handle callbacks.
  * Callbacks should end ASAP.
  */
int8_t uavcanSubscribe(uint64_t signature,
                       uint16_t id,
                       void (callback)(CanardRxTransfer* transfer));


/**
  * @brief Broadcast a message.
  */
int16_t uavcanPublish(uint64_t data_type_signature,
                      uint16_t data_type_id,
                      uint8_t* inout_transfer_id,
                      uint8_t priority,
                      const void* payload,
                      uint16_t payload_len);


/**
  * @brief Respond on RPC-request.
  */
void uavcanRespond(CanardRxTransfer* transfer,
                   uint64_t data_type_signature,
                   uint16_t data_type_id,
                   const uint8_t* payload,
                   uint16_t len);


/**
  * @brief NodeInfo API
  */
void uavcanConfigure(const SoftwareVersion* new_sw_vers, const HardwareVersion* new_hw_vers);
void uavcanSetNodeName(const char* new_node_name);


/**
  * @note TransportStats API
  */
void uavcanStatsIncreaseCanErrors();
void uavcanStatsIncreaseCanTx(uint8_t num_of_transfers);
void uavcanStatsIncreaseCanRx();
void uavcanStatsIncreaseUartErrors();
void uavcanStatsIncreaseUartTx(uint32_t num);
void uavcanStatsIncreaseUartRx(uint32_t num);
uint64_t uavcanGetErrorCount();


/**
  * @brief NodeStatus API
  */
void uavcanSetNodeHealth(NodeStatusHealth_t health);
NodeStatusHealth_t uavcanGetNodeHealth();

NodeStatusMode_t uavcanGetNodeStatusMode();
void uavcanSetNodeStatusMode(NodeStatusMode_t mode);

void uavcanSetVendorSpecificStatusCode(uint16_t vssc);

const NodeStatus_t* uavcanGetNodeStatus();

uint32_t uavcanGetTimeMs();

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

template <typename MessageType>
struct DronecanPublisherTraits;

#define DEFINE_PUBLISHER_TRAITS(MessageType, SerializeFunction, Signature, Id, MessageSize) \
template <> \
struct DronecanPublisherTraits<MessageType> { \
    static inline int8_t publish(const MessageType* const obj, uint8_t* inout_transfer_id) { \
        uint8_t buffer[MessageSize]; \
        size_t inout_buffer_size = MessageSize; \
        int32_t number_of_bytes = SerializeFunction(obj, buffer, &inout_buffer_size); \
        if ((size_t)number_of_bytes > inout_buffer_size || number_of_bytes <= 0) {\
            return -1;\
        }\
        int16_t res = uavcanPublish(Signature, \
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
DEFINE_PUBLISHER_TRAITS(Temperature_t,
                        dronecan_equipment_temperature_serialize,
                        UAVCAN_EQUIPMENT_DEVICE_TEMPERATURE_SIGNATURE,
                        UAVCAN_EQUIPMENT_DEVICE_TEMPERATURE_ID,
                        UAVCAN_EQUIPMENT_DEVICE_TEMPERATURE_MESSAGE_SIZE
)
DEFINE_PUBLISHER_TRAITS(BatteryInfo_t,
                        dronecan_equipment_power_battery_info_serialize,
                        UAVCAN_EQUIPMENT_POWER_BATTERY_INFO_SIGNATURE,
                        UAVCAN_EQUIPMENT_POWER_BATTERY_INFO_ID,
                        UAVCAN_EQUIPMENT_POWER_BATTERY_INFO_MESSAGE_SIZE
)
DEFINE_PUBLISHER_TRAITS(Hygrometer,
                        dronecan_sensors_hygrometer_hygrometer_serialize,
                        DRONECAN_SENSORS_HYGROMETER_HYGROMETER_SIGNATURE,
                        DRONECAN_SENSORS_HYGROMETER_HYGROMETER_ID,
                        DRONECAN_SENSORS_HYGROMETER_HYGROMETER_MESSAGE_SIZE
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
    uint8_t inout_transfer_id;
};


template <typename MessageType>
class DronecanPeriodicPublisher : public DronecanPublisher<MessageType> {
public:
    DronecanPeriodicPublisher(float frequency) :
        DronecanPublisher<MessageType>(),
        PUB_PERIOD_MS(static_cast<uint32_t>(1000.0f / std::clamp(frequency, 0.001f, 1000.0f))) {};

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

#endif  // DCNODE_DCNODE_H_
