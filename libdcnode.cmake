# Copyright (c) 2023 Dmitry Ponomarev
# Distributed under the MPL v2.0 License, available in the file LICENSE.
# Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

cmake_minimum_required(VERSION 3.15.3)

set(DRONECAN_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/Libs/libcanard_v0/canard.c
    ${CMAKE_CURRENT_LIST_DIR}/src/dronecan.c
    ${CMAKE_CURRENT_LIST_DIR}/src/logger.cpp
)

set(DRONECAN_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/Libs
    ${CMAKE_CURRENT_LIST_DIR}/include/application
    ${CMAKE_CURRENT_LIST_DIR}/include/serialization
)
