/*
 * Copyright (C) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_EQUIPMENT_SAFETY_ARMING_STATUS_H_
#define UAVCAN_EQUIPMENT_SAFETY_ARMING_STATUS_H_

#include <stdint.h>
#include <stdbool.h>
#include "libdcnode/dronecan.h"
#include "libdcnode/serialization_internal.h"

#define UAVCAN_EQUIPMENT_SAFETY_ARMING_STATUS_ID                     1100
#define UAVCAN_EQUIPMENT_SAFETY_ARMING_STATUS_SIGNATURE              0x8700f375556a8003
#define UAVCAN_EQUIPMENT_SAFETY_ARMING_STATUS_MESSAGE_SIZE           1
#define UAVCAN_EQUIPMENT_SAFETY_ARMING_STATUS UAVCAN_EXPAND(UAVCAN_EQUIPMENT_SAFETY_ARMING_STATUS)

enum class SafetyArmingStatus: uint8_t {
    STATUS_DISARMED     = 0,
    STATUS_FULLY_ARMED  = 255,
};

#ifdef __cplusplus
extern "C" {
#endif


static inline int8_t dronecan_equipment_safety_arming_status_deserialize(
    const CanardRxTransfer* transfer, SafetyArmingStatus* obj)
{
    canardDecodeScalar(transfer, 0,      8, false, obj);

    return 0;
}

static inline int8_t uavcanSubscribeArmingStatus(void (*transfer_callback)(CanardRxTransfer*)) {
    return uavcanSubscribe(UAVCAN_EQUIPMENT_SAFETY_ARMING_STATUS, transfer_callback);
}

#ifdef __cplusplus
}
#endif


#endif  // UAVCAN_EQUIPMENT_SAFETY_ARMING_STATUS_H_
