# Copyright (c) 2023-2024 Dmitry Ponomarev
# Distributed under the MPL v2.0 License, available in the file LICENSE.
# Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

cmake_minimum_required(VERSION 3.15.3)
project(ubuntu CXX C ASM)

cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH EXAMPLES_DIR)
cmake_path(GET EXAMPLES_DIR PARENT_PATH ROOT_DIR)


# 1. Specify the CAN_PLATFORM. Options: bxcan, fdcan or socketcan
set(CAN_PLATFORM socketcan)

# 2. Specify path to libparams and platform. Options: stm32f103, stm32g0b1, ubuntu
set(LIBPARAMS_PATH        ${ROOT_DIR}/build/external/libparams)
set(LIBPARAMS_PLATFORM    ubuntu)
set(LIBPARAMS_PARAMS_DIR="${CMAKE_CURRENT_BINARY_DIR}/params")
add_definitions(-DLIBPARAMS_PARAMS_DIR="${CMAKE_CURRENT_BINARY_DIR}/params")

# 3. Include the dronecan.cmake
include(${ROOT_DIR}/dronecan.cmake)

# 4. Add DroneCAN related source files and headers to you target
add_executable(application
  main.cpp
  params.cpp
  ${DRONECAN_SOURCES}
  ${libparamsSrc}
)
target_include_directories(application PRIVATE
  .
  ${DRONECAN_HEADERS}
  ${libparamsHeaders}
)
target_compile_options(application PRIVATE
  -Wall
  -Wextra
  -Wfloat-equal
  -Werror
  -Wundef
  -Wshadow
  -Wpointer-arith
  -Wunreachable-code
  -Wstrict-overflow=5
  -Wwrite-strings
  -Wswitch-default
)

# 5. (Recommended) Add GIT_HASH based on git repository
execute_process(
  COMMAND git rev-parse --short=16 HEAD
  COMMAND_ERROR_IS_FATAL ANY
  OUTPUT_VARIABLE GIT_HASH_SHORT
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
set(GIT_HASH "0x${GIT_HASH_SHORT}")
add_definitions(-DGIT_HASH=${GIT_HASH})
add_definitions(-DAPP_VERSION_MAJOR=0)
add_definitions(-DAPP_VERSION_MINOR=1)
add_definitions(-DHW_VERSION_MAJOR=0)
add_definitions(-DHW_VERSION_MINOR=0)

# 6. (Recommended) Set node name
add_definitions(-DAPP_NODE_NAME="${PROJECT_NAME}")
