/*
 * Copyright (C) 2023-2024 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef DCNODE_PUBLISHER_HPP_
#define DCNODE_PUBLISHER_HPP_

#include <stdint.h>
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
            std::cout << "fail: id=" << Id << ", buf_size=" << inout_buffer_size << ", bytes=" << number_of_bytes << std::endl;\
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

#endif  // DCNODE_PUBLISHER_HPP_
