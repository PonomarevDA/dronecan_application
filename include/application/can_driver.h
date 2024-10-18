/*
 * Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef APPLICATION_CAN_DRIVER_H_
#define APPLICATION_CAN_DRIVER_H_

#include "dronecan_application_internal.h"

#define CAN_DRIVER_FIRST    0
#define CAN_DRIVER_SECOND   1

#ifdef __cplusplus
extern "C" {
#endif

int16_t canDriverInit(uint32_t can_speed, uint8_t can_driver_idx);

int16_t canDriverReceive(CanardCANFrame* const rx_frame, uint8_t can_driver_idx);

int16_t canDriverTransmit(const CanardCANFrame* const tx_frame, uint8_t can_driver_idx);

/*
* @brief Get protocol of the CAN driver
* @return 0 if protocol is Dronecan, 1 if Cyphal
*/
int8_t canDriverGetProtocol(uint8_t can_driver_idx) {
    CanardCANFrame rx_frame;
    while (true) {
        if (canDriverReceive(&rx_frame, can_driver_idx) == 0) {
            const uint8_t tail_byte = rx_frame.data[rx_frame.data_len - 1];
            if (IS_START_OF_TRANSFER(tail_byte)) {
                if (IS_END_OF_TRANSFER(tail_byte)) {
                    return TOGGLE_BIT(tail_byte);
                }
            }
        }
    }
}

uint64_t canDriverGetRxOverflowCount();
uint64_t canDriverGetErrorCount();

#ifdef __cplusplus
}
#endif

#endif  // APPLICATION_CAN_DRIVER_H_
