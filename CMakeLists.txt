cmake_minimum_required(VERSION 3.15.3)


if(DRONECAN_PLATFORM STREQUAL "bxcan")
    set(DRONECAN_PLATFORM_SOURCES)
	message(SEND_ERROR "not yet!")
elseif(DRONECAN_PLATFORM STREQUAL "fdcan")
	message(SEND_ERROR "not yet!")
elseif(DRONECAN_PLATFORM STREQUAL "socketcan")
    set(DRONECAN_PLATFORM_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/platform_specific/socketcan/socketcan/can_driver.c
        ${CMAKE_CURRENT_LIST_DIR}/platform_specific/socketcan/socketcan/socketcan.c
    )
else()
	message(SEND_ERROR "DRONECAN_PLATFORM is not specified!")
endif()


if(NOT DEFINED LIBPARAMS_PATH)
	message(SEND_ERROR "LIBPARAMS_PATH is not specified!")
endif()
include(${LIBPARAMS_PATH}/CMakeLists.txt)

set(DRONECAN_SOURCES
    ${DRONECAN_PLATFORM_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/Libs/libcanard_v0/canard.c
    ${CMAKE_CURRENT_LIST_DIR}/application/uavcan.c
    ${CMAKE_CURRENT_LIST_DIR}/application/uavcan_application.c
    ${CMAKE_CURRENT_LIST_DIR}/application/weak.c
)

set(DRONECAN_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/Libs
    ${CMAKE_CURRENT_LIST_DIR}/application
    ${CMAKE_CURRENT_LIST_DIR}/serialization
)