cmake_minimum_required(VERSION 3.15.3)
project(minimal_ubuntu_example)


# 1. Specify the DRONECAN_PLATFORM
# options: bxcan, fdcan or socketcan
set(DRONECAN_PLATFORM socketcan)

# 2. Specify path to libparams
set(LIBPARAMS_PATH        ../../build/libparams)
set(LIBPARAMS_PLATFORM    ubuntu)

# 2. Include the CMakeLists.txt
include(../../CMakeLists.txt)

# 3. Add DroneCAN related source files and headers to you target
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


execute_process(
  COMMAND git rev-parse --short=16 HEAD
  COMMAND_ERROR_IS_FATAL ANY
  OUTPUT_VARIABLE GIT_HASH_SHORT
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
set(GIT_HASH "0x${GIT_HASH_SHORT}")
add_definitions(-DGIT_HASH=${GIT_HASH})
