/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UAVCAN_PROTOCOL_GET_NODE_INFO_H_
#define UAVCAN_PROTOCOL_GET_NODE_INFO_H_

#include <string.h>
#include "dronecan_application_internal.h"
#include "node_status.h"


#define UAVCAN_GET_NODE_INFO_DATA_TYPE_ID           1
#define UAVCAN_GET_NODE_INFO_DATA_TYPE_SIGNATURE    0xee468a8121c46a9e
#define UAVCAN_GET_NODE_INFO_RESPONSE_MAX_SIZE      ((3015 + 7) / 8)
#define UAVCAN_GET_NODE_INFO_DATA_TYPE              UAVCAN_EXPAND(UAVCAN_GET_NODE_INFO_DATA_TYPE)


typedef struct {
    uint32_t vcs_commit;
    uint8_t major;
    uint8_t minor;
    ///< uint8_t optional_field_flags;
    ///< uint64_t image_crc;
} SoftwareVersion;

typedef struct {
    uint8_t unique_id[16];
    uint8_t major;
    uint8_t minor;
    //< uint8_t[255] certificate_of_authenticity
} HardwareVersion;

#ifdef __cplusplus
extern "C" {
#endif

static inline uint16_t uavcanEncodeParamGetNodeInfo(
                        uint8_t buffer[UAVCAN_GET_NODE_INFO_RESPONSE_MAX_SIZE],
                        const NodeStatus_t* node_status,
                        const SoftwareVersion* sw_version,
                        const HardwareVersion* hw_version,
                        const char* node_name) {
    // 1. NodeStatus status. 7 bytes
    uavcanEncodeNodeStatus(buffer, node_status);

    // 2. SoftwareVersion software_version. 15 bytes
    buffer[7] = sw_version->major;
    buffer[8] = sw_version->minor;
    buffer[9] = 1;                              ///< optional_field_flags, VCS commit is set
    uint32_t u32 = sw_version->vcs_commit;
    canardEncodeScalar(buffer, 80, 32, &u32);
    memset(buffer + 14, 0x00, 8);    ///< uint64 image_crc

    // 3. HardwareVersion hardware_version. 18 bytes
    buffer[22] = hw_version->major;
    buffer[23] = hw_version->minor;
    memcpy(&buffer[24], hw_version->unique_id, 16);

    // 4. Node name. Up to 80 bytes
    size_t name_len = strlen(node_name);
    buffer[40] = 0;
    memcpy(&buffer[41], node_name, name_len);

    return 41 + name_len;
}

#ifdef __cplusplus
}
#endif

#endif  // UAVCAN_PROTOCOL_GET_NODE_INFO_H_
