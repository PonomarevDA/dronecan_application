# Copyright (c) 2023 Dmitry Ponomarev
# Distributed under the MPL v2.0 License, available in the file LICENSE.
# Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

cmake_minimum_required(VERSION 3.15.3)
set(DRONECAN_LIB_DIR ${CMAKE_CURRENT_LIST_DIR})

if(CAN_PLATFORM STREQUAL "bxcan")
    set(DRONECAN_PLATFORM_SOURCES
        ${DRONECAN_LIB_DIR}/platform_specific/stm32_bxcan/can_driver.c
        ${DRONECAN_LIB_DIR}/platform_specific/stm32_bxcan/canard_stm32.c
    )
elseif(CAN_PLATFORM STREQUAL "fdcan")
    set(DRONECAN_PLATFORM_SOURCES
        ${DRONECAN_LIB_DIR}/platform_specific/stm32_fdcan/can_driver.c
    )
elseif(CAN_PLATFORM STREQUAL "socketcan")
    set(DRONECAN_PLATFORM_SOURCES
        ${DRONECAN_LIB_DIR}/platform_specific/socketcan/socketcan/can_driver.c
        ${DRONECAN_LIB_DIR}/platform_specific/socketcan/socketcan/socketcan.c
    )
else()
	message(SEND_ERROR "CAN_PLATFORM is not specified! Options: bxcan, fdcan, socketcan.")
endif()


if(NOT DEFINED LIBPARAMS_PATH)
	message(SEND_ERROR "LIBPARAMS_PATH is not specified!")
endif()
include(${LIBPARAMS_PATH}/CMakeLists.txt)

set(DRONECAN_SOURCES
    ${DRONECAN_PLATFORM_SOURCES}
    ${DRONECAN_LIB_DIR}/Libs/libcanard_v0/canard.c
    ${DRONECAN_LIB_DIR}/src/dronecan.c
    ${DRONECAN_LIB_DIR}/src/weak.c
    ${libparamsSrc}
)

set(DRONECAN_HEADERS
    ${DRONECAN_LIB_DIR}/Libs
    ${DRONECAN_LIB_DIR}/include/application
    ${DRONECAN_LIB_DIR}/include/serialization
    ${libparamsHeaders}
)
