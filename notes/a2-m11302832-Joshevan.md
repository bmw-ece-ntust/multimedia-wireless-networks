# **Multimedia Wireless Network A2**

- [**Multimedia Wireless Network A2**](#multimedia-wireless-network-a2)
  - [1. Introduction](#1-introduction)
  - [2. NS-3 Installation](#2-ns-3-installation)
    - [2.1 Prerequisites](#21-prerequisites)
    - [2.2 Download and Install NS-3](#22-download-and-install-ns-3)
    - [2.3 Test NS-3](#23-test-ns-3)
  - [3. WiFi Simulation](#3-wifi-simulation)
  - [4. Analysis of the Simulation Results](#4-analysis-of-the-simulation-results)
  - [References](#references)


## 1. Introduction
NS-3 is a discrete-event network simulator, primarily used for research and educational purposes. It is an open-source project that provides a flexible and extensible platform for simulating various network protocols and scenarios. NS-3 is widely used in academia and industry for network research, performance evaluation, and protocol development.

## 2. NS-3 Installation
### 2.1 Prerequisites
* The NS-3 is installed in a virtual machine running on VirtualBox with the following specifications:
  
    | Operating System | Processor | Memory | Disk Space
    |----------|----------|----------|----------|
    |   Ubuntu 20.04 LTS   |   4 vCPU   |   4 GB   | 50 GB   |

* Install prerequisites for NS-3 on Ubuntu 20.04 LTS:
```bash
sudo apt-get update
sudo apt-get install g++ python3 cmake make
```

```bash
Reading package lists... Done
Building dependency tree       
Reading state information... Done
g++ is already the newest version (4:9.3.0-1ubuntu2).
make is already the newest version (4.2.1-1.2).
python3 is already the newest version (3.8.2-0ubuntu2).
cmake is already the newest version (3.16.3-1ubuntu1.20.04.1).
The following packages were automatically installed and are no longer required:
  cabextract fuseiso
Use 'sudo apt autoremove' to remove them.
0 upgraded, 0 newly installed, 0 to remove and 408 not upgraded.
```

All of the prerequisites are already installed

### 2.2 Download and Install NS-3

* Clone the NS-3 repository from repository:
```bash
git clone https://gitlab.com/nsnam/ns-3-dev.git
```
```
Cloning into 'ns-3-dev'...
remote: Enumerating objects: 221295, done.
remote: Counting objects: 100% (3350/3350), done.
remote: Compressing objects: 100% (1256/1256), done.
remote: Total 221295 (delta 2091), reused 3279 (delta 2083), pack-reused 217945 (from 1)
Receiving objects: 100% (221295/221295), 172.66 MiB | 2.86 MiB/s, done.
Resolving deltas: 100% (185637/185637), done.
Updating files: 100% (3915/3915), done.
```

* Checkout the 3.44 version of NS-3:
```bash
cd ns-3-dev
git checkout -b ns-3.44-branch ns-3.44
```
```
Switched to a new branch 'ns-3.44-branch'
```

* Configure NS-3 using the following command:
```bash
./ns3 configure --enable-examples --enable-tests
```
```
Warn about uninitialized values.
-- The CXX compiler identification is GNU 10.5.0
-- The C compiler identification is GNU 10.5.0
-- Check for working CXX compiler: /usr/bin/g++-10
-- Check for working CXX compiler: /usr/bin/g++-10 -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Check for working C compiler: /usr/bin/gcc-10
-- Check for working C compiler: /usr/bin/gcc-10 -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Using default output directory /home/ubuntu/ns-3-dev/build

**Output omitted for brevity**

-- Configuring done
-- Generating done
-- Build files have been written to: /home/ubuntu/ns-3-dev/cmake-cache
Finished executing the following commands:
mkdir cmake-cache
/usr/bin/cmake -S /home/ubuntu/ns-3-dev -B /home/ubuntu/ns-3-dev/cmake-cache -DCMAKE_BUILD_TYPE=default -DNS3_ASSERT=ON -DNS3_LOG=ON -DNS3_WARNINGS_AS_ERRORS=OFF -DNS3_NATIVE_OPTIMIZATIONS=OFF -DNS3_EXAMPLES=ON -DNS3_TESTS=ON -G Unix Makefiles --warn-uninitialized
```

* Build NS-3 using the following command:
```
./ns3 build
```
```
Scanning dependencies of target stdlib_pch-default
[  0%] Building CXX object CMakeFiles/stdlib_pch-default.dir/cmake_pch.hxx.gch
[  0%] Building CXX object CMakeFiles/stdlib_pch-default.dir/build-support/empty.cc.o
Scanning dependencies of target stdlib_pch_exec
[  0%] Building CXX object CMakeFiles/stdlib_pch_exec.dir/cmake_pch.hxx.gch
[  0%] Building CXX object CMakeFiles/stdlib_pch_exec.dir/build-support/empty-main.cc.o
[  0%] Linking CXX executable ns3.44-stdlib_pch_exec-default
Scanning dependencies of target core
[  1%] Building CXX object src/core/CMakeFiles/core.dir/model/int64x64-128.cc.o
[  1%] Building CXX object src/core/CMakeFiles/core.dir/model/unix-fd-reader.cc.o
[  1%] Building CXX object src/core/CMakeFiles/core.dir/model/example-as-test.cc.o
[  1%] Building CXX object src/core/CMakeFiles/core.dir/helper/csv-reader.cc.o
[  1%] Building CXX object src/core/CMakeFiles/core.dir/helper/random-variable-stream-helper.cc.o
[  1%] Building CXX object src/core/CMakeFiles/core.dir/helper/event-garbage-collector.cc.o
[  1%] Building CXX object src/core/CMakeFiles/core.dir/model/time.cc.o
[  1%] Building CXX object src/core/CMakeFiles/core.dir/model/event-id.cc.o
[  1%] Building CXX object src/core/CMakeFiles/core.dir/model/scheduler.cc.o
[  1%] Building CXX object src/core/CMakeFiles/core.dir/model/list-scheduler.cc.o

**Output omitted for brevity**

Scanning dependencies of target dsr-example
Scanning dependencies of target dsr-test
Scanning dependencies of target manet-routing-compare
[ 98%] Building CXX object src/dsr/CMakeFiles/dsr-test.dir/test/dsr-test-suite.cc.o
[ 99%] Building CXX object src/dsr/examples/CMakeFiles/dsr-example.dir/dsr-example.cc.o
[ 99%] Building CXX object examples/routing/CMakeFiles/manet-routing-compare.dir/manet-routing-compare.cc.o
[ 99%] Linking CXX shared library ../../../build/lib/libns3.44-dsr-test-default.so
Scanning dependencies of target scratch_subdir_scratch-subdir
[ 99%] Building CXX object scratch/CMakeFiles/scratch_subdir_scratch-subdir.dir/subdir/scratch-subdir-additional-header.cc.o
[ 99%] Building CXX object scratch/CMakeFiles/scratch_subdir_scratch-subdir.dir/subdir/scratch-subdir.cc.o
[ 99%] Linking CXX executable ../../build/scratch/subdir/ns3.44-scratch-subdir-default
Scanning dependencies of target scratch_scratch-simulator
[ 99%] Building CXX object scratch/CMakeFiles/scratch_scratch-simulator.dir/scratch-simulator.cc.o
[ 99%] Linking CXX executable ../../../../build/src/dsr/examples/ns3.44-dsr-example-default
Scanning dependencies of target print-introspected-doxygen
[100%] Building CXX object utils/CMakeFiles/print-introspected-doxygen.dir/print-introspected-doxygen.cc.o
[100%] Linking CXX executable ../../../build/examples/routing/ns3.44-manet-routing-compare-default
[100%] Linking CXX executable ../../build/scratch/ns3.44-scratch-simulator-default
Scanning dependencies of target test-runner
[100%] Building CXX object utils/CMakeFiles/test-runner.dir/test-runner.cc.o
[100%] Linking CXX executable ../../build/utils/ns3.44-test-runner-default
[100%] Linking CXX executable ../../build/utils/ns3.44-print-introspected-doxygen-default
Finished executing the following commands:
/usr/bin/cmake --build /home/joshevan/ns-3-dev/cmake-cache -j 3
```
### 2.3 Test NS-3
* Once the build is complete, you can run the test suite to ensure that everything is working correctly:
```bash
./test.py
```
```
Finished executing the following commands:
/usr/bin/cmake --build /home/joshevan/ns-3-dev/cmake-cache -j 3
[0/790] PASS: TestSuite circular-aperture-antenna-test
[1/790] PASS: TestSuite degrees-radians
[2/790] PASS: TestSuite angles
[3/790] PASS: TestSuite cosine-antenna-model
[4/790] PASS: TestSuite isotropic-antenna-model
[5/790] PASS: TestSuite parabolic-antenna-model
[6/790] PASS: TestSuite adjacency-matrix-test
[7/790] PASS: TestSuite uniform-planar-array-test
[8/790] PASS: TestSuite aodv-routing-id-cache
[9/790] PASS: TestSuite routing-aodv
[10/790] PASS: TestSuite routing-aodv-loopback
[11/790] PASS: TestSuite routing-aodv-regression
[12/790] PASS: TestSuite applications-udp-client-server
[13/790] PASS: TestSuite applications-bulk-send
[14/790] PASS: TestSuite buildings-channel-condition-model
[15/790] PASS: TestSuite buildings-helper
[16/790] PASS: TestSuite buildings-pathloss-test
[17/790] PASS: TestSuite applications-three-gpp-http-client-server
[18/790] PASS: TestSuite buildings-penetration-losses
[19/790] PASS: TestSuite building-position-allocator
[20/790] PASS: TestSuite buildings-shadowing-test
[21/790] PASS: TestSuite outdoor-random-walk-model

** Output omitted for brevity**

[770/790] PASS: Example src/energy/examples/basic-energy-model-test
[771/790] PASS: Example src/csma/examples/csma-multicast
[772/790] PASS: Example src/csma/examples/csma-broadcast
[773/790] PASS: Example src/csma/examples/csma-one-subnet
[774/790] PASS: Example src/csma/examples/csma-ping
[775/790] PASS: Example src/csma/examples/csma-raw-ip-socket
[776/790] PASS: Example src/core/examples/main-callback
[777/790] PASS: Example src/core/examples/sample-simulator
[778/790] PASS: Example src/core/examples/main-ptr
[779/790] PASS: Example src/core/examples/sample-random-variable
[780/790] PASS: Example src/csma/examples/csma-packet-socket
[781/790] PASS: Example src/config-store/examples/config-store-save
[782/790] PASS: Example src/core/examples/test-string-value-formatting
[783/790] PASS: Example src/buildings/examples/outdoor-group-mobility-example --useHelper=0
[784/790] PASS: Example src/buildings/examples/buildings-pathloss-profiler
[785/790] PASS: Example src/buildings/examples/outdoor-group-mobility-example --useHelper=1
[786/790] PASS: Example src/bridge/examples/csma-bridge
[787/790] PASS: Example src/bridge/examples/csma-bridge-one-hop
[788/790] PASS: Example src/lr-wpan/examples/lr-wpan-error-distance-plot
[789/790] PASS: Example src/lte/examples/lena-radio-link-failure --numberOfEnbs=2 --useIdealRrc=0 --interSiteDistance=700 --simTime=17
789 of 790 tests passed (789 passed, 0 skipped, 0 failed, 1 crashed, 0 valgrind errors)
List of CRASHed tests:
    threaded-simulator
```
Only one test failed, which is the threaded-simulator test. It means that majority of the tests passed successfully. The simulator is ready to use.


## 3. WiFi Simulation

## 4. Analysis of the Simulation Results



## References
* [NS-3](https://www.nsnam.org/)
