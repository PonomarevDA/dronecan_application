/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file LICENSE.
***/

#ifndef UAVCAN_EQUIPMENT_RANGE_SENSOR_MEASUREMENT_H_
#define UAVCAN_EQUIPMENT_RANGE_SENSOR_MEASUREMENT_H_

#include "serialization_internal.h"
#include "dcnode/dcnode.h"
#include "../../CoarseOrientation.h"

#define UAVCAN_EQUIPMENT_RANGE_SENSOR_MEASUREMENT_ID                   1050U
#define UAVCAN_EQUIPMENT_RANGE_SENSOR_MEASUREMENT_SIGNATURE            0x68fffe70fc771952ULL
#define UAVCAN_EQUIPMENT_RANGE_SENSOR_MEASUREMENT_MESSAGE_SIZE         15U

enum class RangeFinderSensorType: uint8_t {
    SENSOR_TYPE_UNDEFINED     = 0,
    SENSOR_TYPE_SONAR         = 1,
    SENSOR_TYPE_LIDAR         = 2,
    SENSOR_TYPE_RADAR         = 3,
};

/**
 * @brief uavcan.equipment.range_sensor.Measurement
 */
typedef struct {
    uint64_t timestamp;
    uint8_t sensor_id;
    CoarseOrientation_t beam_orientation_in_body_frame;
    float field_of_view;
    RangeFinderSensorType sensor_type;
    uint8_t reading_type;
    float range;
} RangeSensorMeasurement_t;

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t dronecan_equipment_range_sensor_measurement_serialize(
    const RangeSensorMeasurement_t* const obj, uint8_t* const buffer) {
    if (obj == NULL || buffer == NULL) {
        return 0;
    }

    uint16_t field_of_view = canardConvertNativeFloatToFloat16(obj->field_of_view);
    uint16_t range = canardConvertNativeFloatToFloat16(obj->range);

    uint8_t sensor_type = (uint8_t)(obj->sensor_type);
    uint8_t reading_type = obj->reading_type;
    uint8_t offset = 0;
    uint64_t timestamp = obj->timestamp;
    canardEncodeScalar(buffer, offset, 56, &timestamp);
    offset += 56;
    canardEncodeScalar(buffer, offset, 8, &obj->sensor_id);
    offset += 8;

    offset += 8 * dronecan_coarse_orientation_serialize(&obj->beam_orientation_in_body_frame,
                                                        buffer + 8);

    canardEncodeScalar(buffer, offset, 16, &field_of_view);
    offset += 16;
    canardEncodeScalar(buffer, offset, 5, &sensor_type);
    offset += 5;
    canardEncodeScalar(buffer, offset, 3, &reading_type);
    offset += 3;
    canardEncodeScalar(buffer, offset, 16, &range);
    offset +=16;

    return ((offset + 7) / 8);
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_RANGE_SENSOR_MEASUREMENT_H_
