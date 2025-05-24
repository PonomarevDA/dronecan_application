/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file LICENSE.
***/

#ifndef UAVCAN_EQUIPMENT_HARDPOINT_STATUS_H_
#define UAVCAN_EQUIPMENT_HARDPOINT_STATUS_H_

#include "dcnode/dcnode.h"
#include "serialization_internal.h"

#define UAVCAN_EQUIPMENT_HARDPOINT_STATUS_ID                        1071
#define UAVCAN_EQUIPMENT_HARDPOINT_STATUS_SIGNATURE                 0x624a519d42553d82
#define UAVCAN_EQUIPMENT_HARDPOINT_STATUS_MESSAGE_SIZE              7

typedef struct {
    uint8_t hardpoint_id;
    float payload_weight;
    float payload_weight_variance;
    uint16_t status;
} HardpointStatus;

#ifdef __cplusplus
extern "C" {
#endif

static inline int8_t dronecan_equipment_hardpoint_status_deserialize(
    const CanardRxTransfer* transfer, HardpointStatus* obj) {
    if ((transfer == NULL) || (obj == NULL)) {
        return -2;
    }
    canardDecodeScalar(transfer, 0,     8,  false, &obj->hardpoint_id);
    canardDecodeScalar(transfer, 8,     16, true, &obj->payload_weight);
    canardDecodeScalar(transfer, 24,     16, true, &obj->payload_weight_variance);
    canardDecodeScalar(transfer, 40,     16,  false, &obj->status);

    return 0;
}

static inline uint32_t dronecan_equipment_hardpoint_status_serialize(
    const HardpointStatus* const obj,
    uint8_t* const buffer,
    size_t* const inout_buffer_size_bytes)
{
    if ((obj == NULL) || (buffer == NULL) || (inout_buffer_size_bytes == NULL)) {
        return 0;
    }

    const size_t capacity_bytes = *inout_buffer_size_bytes;
    if (capacity_bytes < UAVCAN_EQUIPMENT_HARDPOINT_STATUS_MESSAGE_SIZE) {
        return 0;
    }

    canardEncodeScalar(buffer,  0,   8,  &obj->hardpoint_id);
    canardEncodeFloat16(buffer, 8,  obj->payload_weight);
    canardEncodeFloat16(buffer, 24, obj->payload_weight_variance);
    canardEncodeScalar(buffer,  40, 16,  &obj->status);

    return UAVCAN_EQUIPMENT_HARDPOINT_STATUS_MESSAGE_SIZE;
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_EQUIPMENT_HARDPOINT_STATUS_H_
