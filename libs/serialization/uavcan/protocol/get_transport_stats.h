/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_PROTOCOL_GET_TRANSPORT_STATS_H_
#define UAVCAN_PROTOCOL_GET_TRANSPORT_STATS_H_

#include "serialization_internal.h"

/**
 * @brief uavcan.protocol.CANIfaceStats
 * @note Single CAN iface statistics.
 */
typedef struct {
    uint64_t frames_tx;                     // uint48 - Number of transmitted CAN frames.
    uint64_t frames_rx;                     // uint48 - Number of received CAN frames.
    uint64_t errors;                        // uint48 - Number of errors in the CAN layer.
} CANIfaceStats_t;

/**
 * @brief uavcan.protocol.GetTransportStats
 * @note Get transport statistics.
 */
typedef struct {
    uint64_t transfers_tx;                  // uint48 - Number of transmitted transfers.
    uint64_t transfers_rx;                  // uint48 - Number of received transfers.
    uint64_t transfer_errors;               // uint48 - Number of errors.
    CANIfaceStats_t can_iface_stats[3];     // CAN bus statistics, for each interface
} GetTransportStats_t;

#define UAVCAN_PROTOCOL_GET_TRANSPORT_STATS_ID          4U
#define UAVCAN_PROTOCOL_GET_TRANSPORT_STATS_SIGNATURE   0xbe6f76a7ec312b04ULL
#define UAVCAN_PROTOCOL_GET_TRANSPORT_STATS_MAX_SIZE    72U
#define UAVCAN_PROTOCOL_GET_TRANSPORT_STATS     UAVCAN_EXPAND(UAVCAN_PROTOCOL_GET_TRANSPORT_STATS)

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t uavcanEncodeTransportStats(
    uint8_t buffer[UAVCAN_PROTOCOL_GET_TRANSPORT_STATS_MAX_SIZE],
    const GetTransportStats_t* iface_stats)
{
    uint32_t offset = 0;

    canardEncodeScalar(buffer, offset, 48, &iface_stats->transfers_tx);
    offset += 48;
    canardEncodeScalar(buffer, offset, 48, &iface_stats->transfers_rx);
    offset += 48;
    canardEncodeScalar(buffer, offset, 48, &iface_stats->transfer_errors);
    offset += 48;

    for (size_t i = 0; i < 3; ++i) {
        canardEncodeScalar(buffer, offset, 48, &iface_stats->can_iface_stats[i].frames_tx);
        offset += 48;
        canardEncodeScalar(buffer, offset, 48, &iface_stats->can_iface_stats[i].frames_rx);
        offset += 48;
        canardEncodeScalar(buffer, offset, 48, &iface_stats->can_iface_stats[i].errors);
        offset += 48;
    }

    return ((offset + 7) / 8);
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_PROTOCOL_GET_TRANSPORT_STATS_H_
