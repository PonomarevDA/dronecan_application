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

- [ ] actuator
- [ ] airspeed
- [ ] baro
- [x] circuit status
- [ ] esc
- [ ] rangefinder
- [ ] gnss
- [ ] mag
- [ ] etc

The library should support the following platforms:
- [x] Ubuntu: socketcan
- [ ] stm32f103: BXCAN based on platform specific
- [ ] stm32g0: FDCAN based on STM32 HAL
- [ ] stm32f103: DroneCAN/Serial based on STM32 HAL

## Design

The source code is divided into a few folders:

- `application` has the main source code. It brings up all uavcan/protocol features such as NodeStatus, GetNodeInfo, Params, RestartNode, GetTransportStats.
- `examples` provides examples with this library on different platforms
- `serialation` has a set of headers for data serialization from C-structures to DroneCAN and vice versa
- `Libs` - dependencies
- `platform_specific` has a platform specific drivers
- `tests` - unit tests


Notes:
- It depends on libparams library.
- It is not thread safe.

## How to integrate the library into a project

Add the following lines into CMakeLists.txt of your project:

```cmake
# 1. Specify the DRONECAN_PLATFORM. Options: bxcan, fdcan or socketcan.
set(DRONECAN_PLATFORM ubuntu)

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
#include "uavcan_application.h"

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

More examples in [examples](examples) folder:
- [Ubuntu minimal](examples/ubuntu_minimal/) example
- [Ubuntu CircuitStatus publisher](examples/ubuntu_publisher_circuit_status/) example


## License

The software is distributed under term of MPL v2.0 license.
