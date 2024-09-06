# Copyright (c) 2023 Dmitry Ponomarev
# Distributed under the MPL v2.0 License, available in the file LICENSE.
# Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

cmake_minimum_required(VERSION 3.15.3)
set(DRONECAN_LIB_DIR ${CMAKE_CURRENT_LIST_DIR})

set(SUPPORTED_PLATFORMS "bxcan;fdcan;socketcan")
if(NOT CAN_PLATFORM IN_LIST SUPPORTED_PLATFORMS)
  message(SEND_ERROR "CAN_PLATFORM is not specified or unsupported! Options: bxcan, fdcan, socketcan.")
endif()
include(${CMAKE_CURRENT_LIST_DIR}/platform_specific/${CAN_PLATFORM}/config.cmake)


if(NOT DEFINED LIBPARAMS_PATH)
	message(SEND_ERROR "LIBPARAMS_PATH is not specified!")
endif()
include(${LIBPARAMS_PATH}/CMakeLists.txt)

set(DRONECAN_SOURCES
    ${DRONECAN_PLATFORM_SOURCES}
    ${DRONECAN_LIB_DIR}/Libs/libcanard_v0/canard.c
    ${DRONECAN_LIB_DIR}/src/dronecan.c
    ${DRONECAN_LIB_DIR}/src/logger.cpp
    ${DRONECAN_LIB_DIR}/src/weak.c
    ${libparamsSrc}
)

set(DRONECAN_HEADERS
    ${DRONECAN_LIB_DIR}/Libs
    ${DRONECAN_LIB_DIR}/include/application
    ${DRONECAN_LIB_DIR}/include/serialization
    ${libparamsHeaders}
)
