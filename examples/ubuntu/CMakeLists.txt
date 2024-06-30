# Copyright (c) 2023-2024 Dmitry Ponomarev
# Distributed under the MPL v2.0 License, available in the file LICENSE.
# Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

cmake_minimum_required(VERSION 3.15.3)
project(ubuntu_application CXX C ASM)

# Pathes
set(EXAMPLES_UBUNTU_DIR ${CMAKE_CURRENT_LIST_DIR})
cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH EXAMPLES_DIR)
cmake_path(GET EXAMPLES_DIR PARENT_PATH ROOT_DIR)
set(CMAKE_DIR ${ROOT_DIR}/cmake)
set(BUILD_DIR ${ROOT_DIR}/build)
set(BUILD_UBUNTU_DIR ${ROOT_DIR}/build/ubuntu)
set(LIBPARAMS_PATH ${BUILD_DIR}/external/libparams)
set(BUILD_UBUNTU_LIB_DRONECAN_APP_DIR ${BUILD_UBUNTU_DIR}/libDronecanApp)

# Pre-build
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
add_definitions(-DAPP_NODE_NAME="${PROJECT_NAME}")

# Build dependencies
set(LIBPARAMS_PLATFORM ubuntu)
add_subdirectory(${LIBPARAMS_PATH} ${BUILD_UBUNTU_DIR}/libparams)

# Build libDronecanApp
set(CAN_PLATFORM socketcan)
add_subdirectory(${ROOT_DIR} ${BUILD_UBUNTU_LIB_DRONECAN_APP_DIR})

# Build application
add_executable(${PROJECT_NAME}
  ${EXAMPLES_UBUNTU_DIR}/main.cpp
  ${EXAMPLES_UBUNTU_DIR}/params.cpp
)

target_link_libraries(${PROJECT_NAME} DronecanApp)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${EXAMPLES_UBUNTU_DIR}
)

target_compile_options(${PROJECT_NAME} PRIVATE
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
