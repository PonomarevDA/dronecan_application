/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef APPLICATION_UAVCAN_APPLICATION_H_
#define APPLICATION_UAVCAN_APPLICATION_H_

#include <stdint.h>
#include "uavcan/protocol/get_node_info.h"
#include "uavcan.h"

#ifdef __cplusplus
extern "C" {
#endif


int16_t uavcanInitApplication(uint8_t node_id);


void uavcanConfigure(const SoftwareVersion* new_sw_vers, const HardwareVersion* new_hw_vers);

/**
  * @brief NodeInfo API
  */
void uavcanSetNodeName(const char* new_node_name);

/**
  * @note TransportStats API
  */
void uavcanStatsIncreaseCanErrors();
void uavcanStatsIncreaseCanTx(uint8_t num_of_transfers);
void uavcanStatsIncreaseCanRx();
void uavcanStatsIncreaseUartErrors();
void uavcanStatsIncreaseUartTx(uint32_t num);
void uavcanStatsIncreaseUartRx(uint32_t num);
uint64_t uavcanGetErrorCount();

/**
  * @note Weak - user defined
  */
void uavcanRestartNode();
void uavcanReadUniqueID(uint8_t out_uid[16]);

#ifdef __cplusplus
}
#endif


#endif  // APPLICATION_UAVCAN_APPLICATION_H_
