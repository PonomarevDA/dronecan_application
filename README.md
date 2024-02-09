# DroneCAN application [![build](https://github.com/PonomarevDA/dronecan_application/actions/workflows/build.yml/badge.svg)](https://github.com/PonomarevDA/dronecan_application/actions/workflows/build.yml)  [![check_crlf](https://github.com/PonomarevDA/dronecan_application/actions/workflows/check_crlf.yml/badge.svg)](https://github.com/PonomarevDA/dronecan_application/actions/workflows/check_crlf.yml)

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
- [ ] esc
- [ ] ice
- [x] indication
- [x] power
- [ ] rangefinder
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
- It depends on libparams library.
- It is not thread safe.

## How to integrate the library into a project

Add the following lines into CMakeLists.txt of your project:

```cmake
# 1. Specify the CAN_PLATFORM. Options: bxcan, fdcan or socketcan.
set(CAN_PLATFORM ubuntu)

# 2. Specify path to libparams and platform. Options: stm32f103, stm32g0b1, ubuntu.
set(LIBPARAMS_PATH        ../../build/libparams)
set(LIBPARAMS_PLATFORM    ubuntu)

# 3. Include the CMakeLists.txt
include(../../CMakeLists.txt)

# 4. Add DroneCAN related source files and headers to you target.
add_executable(${EXECUTABLE}
    ...
    ${DRONECAN_SOURCES}
    ...
)
target_include_directories(${EXECUTABLE} PRIVATE
    ...
    ${DRONECAN_HEADERS}
    ...
)
```

The minimal required application looks is shown below:

```c++
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

## Custom publisher example

1. Include a header file, for example:

```c++
#include "uavcan/equipment/power/BatteryInfo.h"
```

2. Create a message and trasnfer id, for example:

```c++
BatteryInfo_t battery_info{};
static uint8_t transfer_id = 0;
```

3. Publish a message and increase the `transfer_id`, for example:

```c++
dronecan_equipment_battery_info_publish(&battery_info, &transfer_id);
transfer_id++;
```

## Custom subscriber example

Let's consider a RawCommand example.

1. Include a header file:

```c++
#include "uavcan/equipment/esc/RawCommand.h"
```

2. Add a callback handler function:

```c++
void callback(CanardRxTransfer* transfer) {
    RawCommand_t raw_command;
    int8_t res = dronecan_equipment_esc_raw_command_deserialize(transfer, &raw_command);
    if (res > 0) {
        // Do something very quickly, or save the command for later use
    } else {
        // Handle a real time error
    }
}
```

3. Add the subscription:

```c++
auto sub_id = uavcanSubscribe(UAVCAN_EQUIPMENT_ESC_RAWCOMMAND, callback);
if (sub_id < 0) {
    // Handle an initialization error
}
```

## More examples

More examples in [examples](examples) folder:
- [Ubuntu minimal](examples/ubuntu/minimal/) example
- [Ubuntu CircuitStatus publisher](examples/ubuntu/publisher/circuit_status/) example
- [Ubuntu RawCommand and ArrayCommand subscriber](examples/ubuntu/subscribers/commands) example
- [Ubuntu LightsCommand subscriber](examples/ubuntu/subscribers/lights_command) example


## License

The software is distributed under term of MPL v2.0 license.
