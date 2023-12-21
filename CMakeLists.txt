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


set(DRONECAN_SOURCES
    ${DRONECAN_PLATFORM_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/Libs/libcanard_v0/canard.c
    ${CMAKE_CURRENT_LIST_DIR}/application/uavcan.c
)

set(DRONECAN_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/Libs
    ${CMAKE_CURRENT_LIST_DIR}/application
    ${CMAKE_CURRENT_LIST_DIR}/serialization
)
