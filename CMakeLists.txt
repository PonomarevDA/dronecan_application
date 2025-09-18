# Copyright (c) 2023 Dmitry Ponomarev
# Distributed under the MPL v2.0 License, available in the file LICENSE.
# Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

cmake_minimum_required(VERSION 3.15.3)
project(libdcnode VERSION 0.2.0 LANGUAGES C CXX)

#
# 1. Create library
#
add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_LIST_DIR}/Libs/libcanard_v0/canard.c
    ${CMAKE_CURRENT_LIST_DIR}/src/dronecan.c
    ${CMAKE_CURRENT_LIST_DIR}/src/logger.cpp
)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Libs
         ${CMAKE_CURRENT_LIST_DIR}/include/application
         ${CMAKE_CURRENT_LIST_DIR}/include/serialization
)
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wno-address-of-packed-member>
)
