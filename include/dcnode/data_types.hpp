/*
 * Copyright (C) 2025 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef DCNODE_DATA_TYPES_H_
#define DCNODE_DATA_TYPES_H_

#include "dronecan/sensors/hygrometer/Hygrometer.h"
#include "uavcan/equipment/actuator/ArrayCommand.h"
#include "uavcan/equipment/actuator/Status.h"
#include "uavcan/equipment/ahrs/MagneticFieldStrength2.h"
#include "uavcan/equipment/ahrs/RawImu.h"
#include "uavcan/equipment/ahrs/Solution.h"
#include "uavcan/equipment/air_data/IndicatedAirspeed.h"
#include "uavcan/equipment/air_data/RawAirData.h"
#include "uavcan/equipment/air_data/StaticPressure.h"
#include "uavcan/equipment/air_data/StaticTemperature.h"
#include "uavcan/equipment/air_data/TrueAirspeed.h"
#include "uavcan/equipment/device/Temperature.h"
#include "uavcan/equipment/esc/RawCommand.h"
#include "uavcan/equipment/esc/Status.h"
#include "uavcan/equipment/gnss/Fix2.h"
#include "uavcan/equipment/hardpoint/Command.h"
#include "uavcan/equipment/hardpoint/Status.h"
#include "uavcan/equipment/ice/FuelTankStatus.h"
#include "uavcan/equipment/ice/Status.h"
#include "uavcan/equipment/indication/BeepCommand.h"
#include "uavcan/equipment/indication/LightsCommand.h"
#include "uavcan/equipment/power/CircuitStatus.h"
#include "uavcan/equipment/power/BatteryInfo.h"
#include "uavcan/equipment/range_sensor/Measurement.h"
#include "uavcan/equipment/safety/ArmingStatus.h"
#include "uavcan/protocol/get_node_info.h"
#include "uavcan/protocol/node_status.h"

#endif  // DCNODE_DATA_TYPES_H_
