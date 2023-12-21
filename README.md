# DroneCAN application

This is C++ library that brings up the [libcanard](https://github.com/dronecan/libcanard), platform specific drivers and serialization together to build a minimal DroneCAN application.

A minimal application always includes the following protocol-features:

| № | type      | message  |
| - | --------- | -------- |
| 1 | broadcast | [uavcan.protocol.NodeStatus](https://legacy.uavcan.org/Specification/7._List_of_standard_data_types/#nodestatus) |
| 2 | RPC-service | [uavcan.protocol.GetNodeInfo](https://legacy.uavcan.org/Specification/7._List_of_standard_data_types/#getnodeinfo) |
| 3 | RPC-service | [uavcan.protocol.param.*](https://legacy.uavcan.org/Specification/7._List_of_standard_data_types/#uavcanprotocolparam) |
| 4 | RPC-service | [uavcan.protocol.RestartNode](https://legacy.uavcan.org/Specification/7._List_of_standard_data_types/#restartnode) |
| 5 | RPC-service | [uavcan.protocol.GetTransportStats](https://legacy.uavcan.org/Specification/7._List_of_standard_data_types/#gettransportstats) |

The following examples of auxilliary features should be provided as well:

- [ ] actuator
- [ ] airspeed
- [ ] baro
- [ ] circuit status
- [ ] esc
- [ ] rangefinder
- [ ] gnss
- [ ] mag
- [ ] etc

## Design

The source code is divided into a few folders:

- dronecan has a DroneCAN application
- serialation - set of headers for convertion data into DroneCAN format
- libcanard has a copy of libcanard
- platform_specific
- tests

It depends on libparams library.

Notes:
- it is not thread safe

## Usage example

Modify you CMakeLists.txt:

```cmake
# 1. Specify the DRONECAN_PLATFORM
# options: bxcan, fdcan or socketcan
set(DRONECAN_PLATFORM bxcan)

# 2. Include the CMakeLists.txt
include(<path_to_this_lib>/CMakeLists.txt)

# 3. Add DroneCAN related source files and headers to you target. For example:
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

In your application:

```c++
#include "dronecan/application.hpp"

// Init it somewhere
DronecanNode node(node_id);
node.init();

// Spin it periodically:
node.spin_once();
```
