# Ubuntu RawCommand and ArrayCommand subscriber example

Basides such protocol functions as NodeStatus and respond and a few RPC-services, the application subscribes on RawCommand and ArrayCommand and print the number of received commands.

Running the example is as simple as possible:

```bash
./scripts/vcan.sh slcan0
make ubuntu_subscriber
```

The first command will create virtual CAN interface. If you type `ifconfig`,
you should see something like that:

```bash
slcan0: flags=193<UP,RUNNING,NOARP>  mtu 72
        unspec 00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00  txqueuelen 1000  (UNSPEC)
        RX packets 21196  bytes 130494 (130.4 KB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 21196  bytes 130494 (130.4 KB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
```

The second command will create a build directory inside `build` folder of the repositor, build the example with CMake and execute the application.
