# NS-3 LTE Simulation: Deploying 9 Nodes

This document outlines the process of setting up and running an LTE simulation in ns-3 with a network topology consisting of 9 nodes. It includes installation steps, configuration for an LTE example with 9 nodes, and basic usage instructions.

---

## 🎯 Goal
- Deploy a network topology with 9 nodes in an LTE simulation using ns-3.
- Execute and verify the simulation.

---

## Prerequisites

| Prerequisite | Package/Version |
|--------------|-----------------|
| C++ Compiler | `g++` (version 9 or greater) |
| Python       | `python3` (version >= 3.6)   |
| Build Tools  | `make`, `cmake`              |
| Git          | Any recent version           |

Install prerequisites on Debian/Ubuntu:
```bash
sudo apt-get update
sudo apt-get install git python3 g++ cmake make -y
```

---

## Install ns-3

1. **Download ns-3**:
   ```bash
   cd ~
   mkdir workspace
   cd workspace
   wget https://www.nsnam.org/release/ns-allinone-3.40.tar.bz2
   tar xjf ns-allinone-3.40.tar.bz2
   cd ns-allinone-3.40
   ```

2. **Build ns-3**:
   ```bash
   ./build.py --enable-examples --enable-tests
   ```

3. **Verify Build**:
   Navigate to `ns-allinone-3.40/ns-3.40` and run:
   ```bash
   ./test.py --no-build
   ```
   Expect a successful test output.

---

## Deploying 9 Nodes in an LTE Simulation

This section demonstrates how to configure and run an LTE simulation with 9 nodes (e.g., eNodeBs or a mix of eNodeBs and UEs).

### Step-by-Step Configuration

1. **Navigate to ns-3 Directory**:
   ```bash
   cd ~/workspace/ns-allinone-3.40/ns-3.40
   ```

2. **Use an LTE Example**:
   Copy the `lena-simple.cc` example from the LTE module as a base:
   ```bash
   cp src/lte/examples/lena-simple.cc scratch/my-lte-9-nodes.cc
   ```

3. **Modify the Script for 9 Nodes**:
   Edit `scratch/my-lte-9-nodes.cc` to deploy 9 nodes. Below is a simplified example configuration:

   ```cpp
   #include "ns3/core-module.h"
   #include "ns3/mobility-module.h"
   #include "ns3/lte-module.h"

   using namespace ns3;

   int main(int argc, char *argv[]) {
       Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();

       // Create 9 eNodeB nodes
       NodeContainer enbNodes;
       enbNodes.Create(9);

       // Set mobility for eNodeBs (fixed positions)
       MobilityHelper mobility;
       mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
       mobility.Install(enbNodes);

       // Position the 9 nodes in a grid (example layout)
       Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
       for (int i = 0; i < 3; ++i) {
           for (int j = 0; j < 3; ++j) {
               positionAlloc->Add(Vector(i * 500.0, j * 500.0, 0.0));
           }
       }
       mobility.SetPositionAllocator(positionAlloc);
       mobility.Install(enbNodes);

       // Install LTE stack on eNodeBs
       NetDeviceContainer enbDevs = lteHelper->InstallEnbDevice(enbNodes);

       // Set simulation time and run
       Simulator::Stop(Seconds(1.0));
       Simulator::Run();
       Simulator::Destroy();

       std::cout << "Simulation with 9 LTE nodes completed!" << std::endl;
       return 0;
   }
   ```

   **Explanation**:
   - Creates 9 eNodeB nodes.
   - Positions them in a 3x3 grid with 500-meter spacing.
   - Installs the LTE protocol stack on each node.

4. **Run the Simulation**:
   ```bash
   ./ns3 run scratch/my-lte-9-nodes
   ```
   Expected output: `Simulation with 9 LTE nodes completed!`

---

## Verification

- Check the console output to confirm the simulation ran successfully.
- Optionally, add logging to the script (e.g., `NS_LOG="LteEnbPhy"` before `Simulator::Run()`) to inspect node activity:
  ```cpp
  LogComponentEnable("LteEnbPhy", LOG_LEVEL_INFO);
  ```

---

## References
- [ns-3 Official Site](https://www.nsnam.org/)
- [LTE User Documentation](https://www.nsnam.org/docs/models/html/lte-user.html)
