/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_PROTOCOL_GET_TRANSPORT_STATS_H_
#define UAVCAN_PROTOCOL_GET_TRANSPORT_STATS_H_

#include "libdcnode/internal.h"

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

#define UAVCAN_PROTOCOL_GET_TRANSPORT_STATS_ID          4
#define UAVCAN_PROTOCOL_GET_TRANSPORT_STATS_SIGNATURE   0xbe6f76a7ec312b04
#define UAVCAN_PROTOCOL_GET_TRANSPORT_STATS_MAX_SIZE    72  // (6 + 6 + 6) * 4
#define UAVCAN_PROTOCOL_GET_TRANSPORT_STATS     UAVCAN_EXPAND(UAVCAN_PROTOCOL_GET_TRANSPORT_STATS)

#ifdef __cplusplus
extern "C" {
#endif

static inline void uavcanEncodeTransportStats(
    uint8_t buffer[UAVCAN_PROTOCOL_GET_TRANSPORT_STATS_MAX_SIZE],
    const GetTransportStats_t* iface_stats)
{
    canardEncodeScalar(buffer, 48 * 0,  48, &iface_stats->transfers_tx);
    canardEncodeScalar(buffer, 48 * 1,  48, &iface_stats->transfers_rx);
    canardEncodeScalar(buffer, 48 * 2,  48, &iface_stats->transfer_errors);

    canardEncodeScalar(buffer, 48 * 3,  48, &iface_stats->can_iface_stats[0].frames_tx);
    canardEncodeScalar(buffer, 48 * 4,  48, &iface_stats->can_iface_stats[0].frames_rx);
    canardEncodeScalar(buffer, 48 * 5,  48, &iface_stats->can_iface_stats[0].errors);

    canardEncodeScalar(buffer, 48 * 6,  48, &iface_stats->can_iface_stats[1].frames_tx);
    canardEncodeScalar(buffer, 48 * 7,  48, &iface_stats->can_iface_stats[1].frames_rx);
    canardEncodeScalar(buffer, 48 * 8,  48, &iface_stats->can_iface_stats[1].errors);

    canardEncodeScalar(buffer, 48 * 9,  48, &iface_stats->can_iface_stats[2].frames_tx);
    canardEncodeScalar(buffer, 48 * 10, 48, &iface_stats->can_iface_stats[2].frames_rx);
    canardEncodeScalar(buffer, 48 * 11, 48, &iface_stats->can_iface_stats[2].errors);
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_PROTOCOL_GET_TRANSPORT_STATS_H_
