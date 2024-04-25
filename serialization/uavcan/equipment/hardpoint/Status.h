/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine96@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file LICENSE.
***/

#include "dronecan.h"
#include "serialization_internal.h"

#define UAVCAN_EQUIPMENT_HARDPOINT_STATUS_ID                        1071
#define UAVCAN_EQUIPMENT_HARDPOINT_STATUS_SIGNATURE                 0x624a519d42553d82
#define UAVCAN_EQUIPMENT_HARDPOINT_STATUS_MESSAGE_SIZE              7  // 56/8

typedef struct {
    uint8_t hardpoint_id;
    float payload_weight;
    float payload_weight_variance;
    uint16_t status;
} HardpointStatus_t;

#ifdef __cplusplus
extern "C" {
#endif

static inline int8_t dronecan_equipment_hardpoint_status_deserialize(
    const CanardRxTransfer* transfer, HardpointStatus_t* obj) {
    if ((transfer == NULL) || (obj == NULL)) {
        return -2;
    }
    canardDecodeScalar(transfer, 0,     8,  false, &obj->hardpoint_id);
    canardDecodeScalar(transfer, 8,     16, true, &obj->payload_weight);
    canardDecodeScalar(transfer, 24,     16, true, &obj->payload_weight_variance);
    canardDecodeScalar(transfer, 40,     16,  false, &obj->status);

    return 0;
}


#ifdef __cplusplus
}
#endif
