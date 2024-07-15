[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=PonomarevDA_dronecan_application&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=PonomarevDA_dronecan_application) [![Lines of Code](https://sonarcloud.io/api/project_badges/measure?project=PonomarevDA_dronecan_application&metric=ncloc)](https://sonarcloud.io/summary/new_code?id=PonomarevDA_dronecan_application) [![build](https://github.com/PonomarevDA/dronecan_application/actions/workflows/build.yml/badge.svg)](https://github.com/PonomarevDA/dronecan_application/actions/workflows/build.yml)  [![check_crlf](https://github.com/PonomarevDA/dronecan_application/actions/workflows/check_crlf.yml/badge.svg)](https://github.com/PonomarevDA/dronecan_application/actions/workflows/check_crlf.yml)

# DroneCAN application

This is C library that brings up the [libcanard](https://github.com/dronecan/libcanard), platform specific drivers and serialization together to build a minimal DroneCAN application.

A minimal application includes the following protocol-features:

| № | type      | message  |
| - | --------- | -------- |
| 1 | broadcast | [uavcan.protocol.NodeStatus](https://legacy.uavcan.org/Specification/7._List_of_standard_data_types/#nodestatus) |
| 2 | RPC-service | [uavcan.protocol.GetNodeInfo](https://legacy.uavcan.org/Specification/7._List_of_standard_data_types/#getnodeinfo) |
| 3 | RPC-service | [uavcan.protocol.param.*](https://legacy.uavcan.org/Specification/7._List_of_standard_data_types/#uavcanprotocolparam) |
| 4 | RPC-service | [uavcan.protocol.RestartNode](https://legacy.uavcan.org/Specification/7._List_of_standard_data_types/#restartnode) |
| 5 | RPC-service | [uavcan.protocol.GetTransportStats](https://legacy.uavcan.org/Specification/7._List_of_standard_data_types/#gettransportstats) |

The following auxilliary features should be provided as well:

- [x] actuator
- [ ] airspeed
- [ ] baro
- [x] circuit status
- [x] fuel tank
- [x] esc
- [x] ice
- [x] indication
- [x] power
- [x] rangefinder
- [ ] gnss
- [ ] mag
- [ ] etc

The library should support the following platforms:
- [x] Ubuntu: socketcan
- [x] stm32f103: BXCAN based on platform specific
- [x] stm32g0: FDCAN based on STM32 HAL
- [ ] stm32f103: DroneCAN/Serial based on STM32 HAL

## Design

The source code is divided into a few folders:

- `application` has the main source code. It brings up all uavcan/protocol features such as NodeStatus, GetNodeInfo, Params, RestartNode, GetTransportStats.
- `examples` provides examples with this library on different platforms
- `serialization` has a set of headers for data serialization from C-structures to DroneCAN and vice versa
- `Libs` - dependencies
- `platform_specific` has a platform specific drivers
- `tests` - unit tests


Notes:
- It depends on libparams v0.9.0 library.
- It is not thread safe.

## How to integrate the library into a project

It is expected to build a static library from source code into your local build directory.

The most important steps are:

```cmake
# 1. Build the only dependency - libparams. Specify a desired LIBPARAMS_PLATFORM.
set(LIBPARAMS_PLATFORM ubuntu)
add_subdirectory(${LIBPARAMS_PATH} ${BUILD_UBUNTU_DIR}/libparams)

# 2. Build libDronecanApp itself. Specify a desired plaform with CAN_PLATFORM.
set(CAN_PLATFORM socketcan)
add_subdirectory(${ROOT_DIR} ${BUILD_UBUNTU_LIB_DRONECAN_APP_DIR})

# 3. Add executable for your application
add_executable(${APPLICATION}
    ...
)

# 4. Link the library to your application
target_link_libraries(${APPLICATION} DronecanApp)
```

For more details, please check an example: [examples/ubuntu/CMakeLists.txt](examples/ubuntu/CMakeLists.txt).

## Minimal application example

The example is avaliable in [examples/ubuntu/minimal](examples/ubuntu/minimal/) folder.

```c++
// Include dronecan.h header file
#include "dronecan.h"

// Initialize the library somewhere
const uint8_t node_id = 42;
auto init_res = uavcanInitApplication(node_id);
if (init_res < 0) {
    // handle error here
}

// Spin it periodically:
while (true) {
    ...
    uavcanSpinOnce();
    ...
}
```

A usage example is shown below:

<img src="https://raw.githubusercontent.com/wiki/PonomarevDA/dronecan_application/assets/ubuntu_minimal.gif" alt="drawing">

## Publisher example

A CircuitStatus publisher example is avaliable in [examples/publisher/circuit_status](examples/publisher/circuit_status/) folder.

A BatteryInfo publisher example is shown below:

```c++
// Include necessary header files
#include "dronecan.h"
#include "uavcan/equipment/power/BatteryInfo.h"

// Create a message and trasnfer id
BatteryInfo_t battery_info{};
static uint8_t transfer_id = 0;

// Publish a message and increase the transfer_id:
dronecan_equipment_battery_info_publish(&battery_info, &transfer_id);
transfer_id++;
```

A CircuitStatus usage example is shown below:

<img src="https://raw.githubusercontent.com/wiki/PonomarevDA/dronecan_application/assets/ubuntu_publisher.gif" alt="drawing">

## Subscriber example

There are [Ubuntu RawCommand and ArrayCommand subscriber](examples/ubuntu/subscribers/commands) and [Ubuntu LightsCommand subscriber](examples/ubuntu/subscribers/lights_command) examples.

Let's consider a RawCommand subscriber example.

```c++
// Include necessary header files
#include "dronecan.h"
#include "uavcan/equipment/esc/RawCommand.h"

// Add a callback handler function
void callback(CanardRxTransfer* transfer) {
    RawCommand_t raw_command;
    int8_t res = dronecan_equipment_esc_raw_command_deserialize(transfer, &raw_command);
    if (res > 0) {
        // Do something very quickly, or save the command for later use
    } else {
        // Handle a real time error
    }
}

// Add the subscription:
auto sub_id = uavcanSubscribe(UAVCAN_EQUIPMENT_ESC_RAWCOMMAND, callback);
if (sub_id < 0) {
    // Handle an initialization error
}
```

## License

The software is distributed under term of MPL v2.0 license.
