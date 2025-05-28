/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_AIR_DATA_RAWAIRDATA_H_
#define UAVCAN_EQUIPMENT_AIR_DATA_RAWAIRDATA_H_

#include "serialization_internal.h"
#include "dcnode/dcnode.h"

#define UAVCAN_EQUIPMENT_AIR_DATA_RAW_AIR_DATA_ID               1027U
#define UAVCAN_EQUIPMENT_AIR_DATA_RAW_AIR_DATA_SIGNATURE        0xc77df38ba122f5daULL
#define UAVCAN_EQUIPMENT_AIR_DATA_RAW_AIR_DATA_MESSAGE_SIZE     17U
#define UAVCAN_EQUIPMENT_AIR_DATA_RAW_AIR_DATA                  UAVCAN_EXPAND(UAVCAN_EQUIPMENT_AIR_DATA_RAW_AIR_DATA)

/**
 * @brief uavcan.equipment.air_data.RawAirData
 */
typedef struct {
    uint8_t flag;
    float static_pressure;
    float differential_pressure;
    float static_pressure_sensor_temperature;
    float differential_pressure_sensor_temperature;
    float static_air_temperature;
    float pitot_temperature;
    float covariance[16];  // is not used because the msg size will be > 17
} RawAirData_t;

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t dronecan_equipment_air_data_raw_air_data_serialize(
    const RawAirData_t* const obj, uint8_t* const buffer) {
    if (obj == NULL || buffer == NULL) {
        return 0;
    }

    uint16_t differential_pressure_sensor_temperature = canardConvertNativeFloatToFloat16(
                                            obj->differential_pressure_sensor_temperature);
    uint16_t static_pressure_sensor_temperature = canardConvertNativeFloatToFloat16(
                                            obj->static_pressure_sensor_temperature);
    uint16_t static_air_temperature = canardConvertNativeFloatToFloat16(
                                            obj->static_air_temperature);
    uint16_t pitot_temperature = canardConvertNativeFloatToFloat16(obj->pitot_temperature);


    canardEncodeScalar(buffer, 0, 8, &obj->flag);
    canardEncodeScalar(buffer, 8, 32, &obj->static_pressure);
    canardEncodeScalar(buffer, 40, 32, &obj->differential_pressure);
    canardEncodeScalar(buffer, 72, 16, &static_pressure_sensor_temperature);
    canardEncodeScalar(buffer, 88, 16, &differential_pressure_sensor_temperature);
    canardEncodeScalar(buffer, 104, 16, &static_air_temperature);
    canardEncodeScalar(buffer, 120, 16, &pitot_temperature);

    return UAVCAN_EQUIPMENT_AIR_DATA_RAW_AIR_DATA_MESSAGE_SIZE;
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_AIR_DATA_RAWAIRDATA_H_
