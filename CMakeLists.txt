# Copyright (c) 2023 Dmitry Ponomarev
# Distributed under the MPL v2.0 License, available in the file LICENSE.
# Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

cmake_minimum_required(VERSION 3.15.3)
project(DronecanApp VERSION 0.9.0)

set(SUPPORTED_PLATFORMS "bxcan;fdcan;socketcan")
if(NOT CAN_PLATFORM IN_LIST SUPPORTED_PLATFORMS)
  message(SEND_ERROR "CAN_PLATFORM is not specified or unsupported! Options: bxcan, fdcan, socketcan.")
endif()

include(platform_specific/${CAN_PLATFORM}/config.cmake)
include(cmake/AddLibparams.cmake)

add_library(${PROJECT_NAME} STATIC
  ${DRONECAN_PLATFORM_SOURCES}
  Libs/libcanard_v0/canard.c
  src/dronecan.c
  src/weak.c
)
target_include_directories(${PROJECT_NAME} PUBLIC
  Libs
  include/application
  include/serialization
)

check_libparams_version(90000) # 0.9.0
target_link_libraries(${PROJECT_NAME} libparams)
