# Multimedia Wireless Networks Assignment 2
---
- **Title**: ns-3 Network Simulator
- **Deadline**: 11th April 2025, 23.59 A.M.
- ## Study plan
|Time |Task |
|---------|------------------------------------------------------------------------|
|Mar.27-31|Confirmation of assignment2 content and study of ns-3 background knowledge|
|Apr.1-7  |Install ns-3 and write down the study note |
|Apr.8-11 |Simulate the network by using ns-3 |

**1. Submission Guidelines**  

- Installation guide:
    1. **Commands**: List terminal commands used for installation.
    2. **Verification**: Copy and paste the terminal logs confirming each step's success.
    3. **Final Check**: Run [`hello-simulator.cc`](https://www.nsnam.org/docs/release/3.19/doxygen/hello-simulator_8cc_source.html) to verify installation.

    References for Installation guides:
  - [NVIDIA CUDA installation guide](https://docs.nvidia.com/cuda/cuda-installation-guide-linux/index.html)
  - [ns-3 installation guide](https://www.nsnam.org/docs/installation/html/linux.html)

- Source-code documentation
    1. Upload all source code under `./a2/src/`
    2. Include comments for each function.
  
- Assignment report (`assignment-2.md`):
    1. **Section 1**: Installation guide for ns-3
    2. **Section 2**: Simulation explanation:
       1. Explain how you execute each milestone (include source code snippet/hyperlink for the `line of code`).
       2. Show figures & explanation for result analysis part.

- **Video simmulation**: Record How simulation your simulation run & attach the hyperlink in the final report on `assignment-2md`)

>[!CAUTION]
>Source code without report in `assignment-2.md` will not count as point.

**2. Objectives**  

1. **NS-3 Installation (`1-points`)**:
    <br> Installation guide report requirements:

2. **LTE / WiFi Simulation (`2-points`)**:
   - **Submission Requirements**: Upload your simulation source code under `./a2/src/` in your branch. For each milestone, provide:
      1. **Input**: Commands or source code (attach hyperlinks from `./a2/src/`).  
      2. **Output**: Figures, results, and explanations.

3. **Analysis reports** (`8-points`)

4. **Peer review** (`1-point`)

5. **Extra Credit (`up to 3-points`)**:
   - **Peer Review Bonus:** Top 3 students based on peer reviews receive extra credit:
     - **Top 1:** (`3-points`)
     - **Top 2:** (`2-points`)
     - **Top 3:** (`1-points`)


**3. Milestones & Grading**  

1. **Deploy 2 nodes and assign users** (`1-points`):
   1. Deploy 2 nodes as Node A & B;
   2. Assign 16 users for each node:
      - For LTE is called User Equipment (UE)
      - For WiFi is called Station (STA)
   3. Provide a user table (e.g., MAC addresses or STA/UE IDs).  

   This setup can be formulated as follow:
   - **Formula:**  
       $$X_0 = Y_0 = 16$$  
   - **Definitions:**
     - ($X_0$) = Initial Users in Node A
     - ($Y_0$) = Initial Users in Node B

2. **Full-Queue Model Transmission**
  
    Implement a [full-queue model](https://www.nsnam.org/doxygen/d9/db7/fcfs-wifi-queue-scheduler_8cc_source.html). Ensure each STA/UE continuously transmits to AP/BS. (`1-point`)

   - **Simulation Duration:** 1 minute.  
   - **User Movement:**  
     - **At second 20:** 25% of users move from Node A → Node B, and 50% from Node B → Node A.  
     - **At second 40:** 50% of users move from Node A → Node B, and 50% from Node B → Node A.
     - For each movement, describe which users that moved from A --> B and B --> A in your .
   - **Formulas:**  
     - **Time slot 1/3 (20 seconds):**  
            $X_1 = X_0 \times p$ , $\quad Y_1 = Y_0 \times q$  
     - **Time slot 2/3 (40 seconds):**  
            $X_2 = X_1 \times p$, $\quad Y_2 = Y_1 \times q$  
   - **Definitions:**  
     - ($p$ = 25\%) → Users moving from Node A to Node B.  
     - ($q$ = 50\% ) → Users moving from Node B to Node A.  
     - ($X_1, X_2$) → Users in Node A after each movement.  
     - ($Y_1, Y_2$) → Users in Node B after each movement.  

3. **Analysis Report**:
   1. What is the final users distribution (value of $X_2$ & $Y_2$)? Show the users table for each movement. (`2-points`)
   2. Show & analyze the figures of the aggregated throughput in Node-A & Node-B through time (`3-points`):
      - X-axis = time & Y-axis = throughput.
      - Analyze & explain your results.

4. Show the 5-minutes video simulation (`3-points`)

**4. References**  

- [ns-3](https://www.nsnam.org/)  
- [ns-3 Tutorial](https://www.nsnam.org/docs/tutorial/html/)  
- [The ns-3 Network Simulator](https://intronetworks.cs.luc.edu/current/html/ns3.html)  
- [ns-3 LTE Module](https://www.nsnam.org/docs/models/html/lte-user.html)  
- [ns-3 WiFi Module](https://www.nsnam.org/docs/models/html/wifi-design.html)  
- [3GPP NR ns-3 Module](https://github.com/QiuYukang/5G-LENA)  
---
## NS-3 Overview
NS-3 represents a modern approach to network simulation, offering researchers and educators a powerful tool for studying digital communications. Unlike traditional network testing methods that require physical hardware, this innovative simulator creates virtual network environments where users can safely experiment with various protocols and configurations.

### Key characteristics:
1. Built from the ground up using C++ for optimal performance
2. Offers Python interfaces for easier scripting
3. Designed with modular components that can be mixed and matched
4. Provides realistic simulations of common network protocols
5. Supports integration with popular analysis tools

### Stands apart from NS-2:
1. A completely redesigned architecture
2. Improved memory efficiency
3. More accurate modeling of network behaviors
4. Better support for modern networking technologies

### Practical applications:

1. Testing new network protocols before real-world implementation
2. Evaluating network performance under different conditions
3. Educational demonstrations of networking concepts
4. Researching next-generation network technologies

### System requirements:
1. Primarily runs on Linux and macOS systems
2. Windows users can access through virtual machines
3. Requires familiarity with command-line operations
4. Benefits from C++ or Python programming knowledge

## NS-3 Installation

### **Download ns-3**
**1.1 Check the version of g++ and python.**
|  tool  | version |  command   | Result            |
|:------:| ------- |:----------:| ----------------- |
| python | 3.8.10  | python3 -V | Python 3.8.10     |
|  g++   | 9.4.0   |   g++ -v   | gcc version 9.4.0 |

**1.2 Download the Latest Release (ns-3.44)**

```bash
wget https://www.nsnam.org/releases/ns-allinone-3.44.tar.bz2
tar xjf ns-allinone-3.44.tar.bz2
cd ns-allinone-3.44/ns-3.44
```

### **Build ns-3**

**2.1 Configure and Build**

```bash
./ns3 configure --enable-examples --enable-tests
./ns3 build
```
![messageImage_1744304797019](https://github.com/user-attachments/assets/e2e5d2bf-8a6b-4b46-9b00-f19995e578d4)


**2.2 Verify Installation**

Run the test suite to ensure the installation is working correctly:

```bash
./test.py
```

![messageImage_1744280781088](https://github.com/user-attachments/assets/5d7a9825-b00e-4546-9a28-06d105f5f54a)


**2.3 Run Example Simulations**

Run `hello-simulator.cc`

```bash
./ns3 run examples/tutorial/hello-simulator
```
![messageImage_1744280105849](https://github.com/user-attachments/assets/c5f03255-37c2-42bb-ae96-6f1248e7cc22)


---
## NS-3 Simulation

### **Create `setup-nodes.cc`**

```bash
nano scratch/setup-nodes.cc
```

This code sets up a WiFi network simulation with:

**Network Structure:**
1. Creates 2 Access Points (APs) named Node A and Node B
2. Connects 16 Station devices (STAs) to each AP (32 STAs total)

**WiFi Configuration:**
1. Uses 802.11n standard with high-speed MCS7 mode
2. Sets different SSIDs for each AP (NodeA/NodeB)

**Positioning:**
1. Places APs 100m apart (NodeA at 0,0 and NodeB at 100,0)
2. Arranges STAs in a grid pattern around their APs



```cpp
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SetupNodes");

// Function: Print MAC addresses of nodes
void PrintMacAddresses(NetDeviceContainer devices, std::string nodeName) {
    std::cout << "MAC Addresses for " << nodeName << " STAs:" << std::endl;
    for (uint32_t i = 0; i < devices.GetN(); ++i) {
        Ptr<NetDevice> device = devices.Get(i);
        Mac48Address mac = Mac48Address::ConvertFrom(device->GetAddress());
        std::cout << "STA " << i + 1 << ": " << mac << std::endl;
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    // 1. Create 2 nodes (Node A and Node B as APs)
    NodeContainer wifiApNodes;
    wifiApNodes.Create(2); // Node A (index 0), Node B (index 1)

    // 2. Assign 16 users (STAs) to each node
    NodeContainer wifiStaNodesA, wifiStaNodesB;
    wifiStaNodesA.Create(16); // 16 STAs for Node A
    wifiStaNodesB.Create(16); // 16 STAs for Node B

    // 3. Configure WiFi PHY and MAC layers
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211n);
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue("HtMcs7"),
                                "ControlMode", StringValue("HtMcs7"));

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    WifiMacHelper mac;
    Ssid ssidA = Ssid("NodeA");
    Ssid ssidB = Ssid("NodeB");

    // Configure AP for Node A
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssidA));
    NetDeviceContainer apDeviceA = wifi.Install(phy, mac, wifiApNodes.Get(0));

    // Configure AP for Node B
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssidB));
    NetDeviceContainer apDeviceB = wifi.Install(phy, mac, wifiApNodes.Get(1));

    // Configure STAs for Node A
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssidA));
    NetDeviceContainer staDevicesA = wifi.Install(phy, mac, wifiStaNodesA);

    // Configure STAs for Node B
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssidB));
    NetDeviceContainer staDevicesB = wifi.Install(phy, mac, wifiStaNodesB);

    // 4. Set node positions (optional but helpful for simulation)
    MobilityHelper mobility;
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue(0.0),
                                  "MinY", DoubleValue(0.0),
                                  "DeltaX", DoubleValue(10.0),
                                  "DeltaY", DoubleValue(10.0),
                                  "GridWidth", UintegerValue(3),
                                  "LayoutType", StringValue("RowFirst"));

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiApNodes);
    mobility.Install(wifiStaNodesA);
    mobility.Install(wifiStaNodesB);

    // Set AP positions
    Ptr<MobilityModel> mobilityApA = wifiApNodes.Get(0)->GetObject<MobilityModel>();
    mobilityApA->SetPosition(Vector(0.0, 0.0, 0.0));
    Ptr<MobilityModel> mobilityApB = wifiApNodes.Get(1)->GetObject<MobilityModel>();
    mobilityApB->SetPosition(Vector(100.0, 0.0, 0.0));

    // 5. Print MAC addresses of all STAs
    PrintMacAddresses(staDevicesA, "Node A");
    PrintMacAddresses(staDevicesB, "Node B");

    // 6. Run simulation (just setup, so simulation time can be short)
    Simulator::Stop(Seconds(1.0));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}

```
![messageImage_1744299872125](https://github.com/user-attachments/assets/1174d3d6-3799-4067-ba35-527f8dea5566)


**Output:**
1. Prints all STA MAC addresses for identification
2. Runs a minimal simulation (1 second) just to initialize the setup

This is typically used as a base configuration for more complex wireless network simulations in ns-3.

---
### **Create `setup-codes-3min.cc`**

```
nano scratch/setup-codes-3min.cc
```



**Basic Setup:**
1. Creates 2 eNodeBs (Cell_A at (0,0) and Cell_B at (500,0))
2. Distributes 32 UEs evenly between both cells initially

**Handover Simulation:**
1. At 20 seconds: Forces handover by physically moving UEs (4 from Cell_A→Cell_B and 8 from Cell_B→Cell_A)
2. At 40 seconds: Rebalances UEs back to original distribution

**Data Collection:**
1. Records UE count per cell every second in handover_data.csv
2. Prints real-time status showing UE distribution between cells

**Key Configurations:**
1. Uses A3-RSRP handover algorithm with 1.5dB hysteresis
2. Sets 256ms time-to-trigger for handovers


```cpp
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/internet-module.h"
#include <fstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("LteHandoverShortDemo");

std::ofstream dataFile("handover_data.csv");

void PrintUeStatus(Ptr<LteHelper> lteHelper, NetDeviceContainer ueDevices, double time) {
    std::map<uint16_t, int> cellCounts = {{1, 0}, {2, 0}}; // Cell_A:1, Cell_B:2
    
    for (uint32_t i = 0; i < ueDevices.GetN(); ++i) {
        Ptr<LteUeNetDevice> ue = DynamicCast<LteUeNetDevice>(ueDevices.Get(i));
        cellCounts[ue->GetRrc()->GetCellId()]++;
    }
    
    // Write to CSV
    dataFile << time << "," << cellCounts[1] << "," << cellCounts[2] << "\n";
    
    // Print to console
    std::cout << "Time " << time << "s: ";
    std::cout << "Cell_A=" << cellCounts[1] << " UEs, ";
    std::cout << "Cell_B=" << cellCounts[2] << " UEs\n";
}

int main(int argc, char *argv[]) {
    Time simTime = Seconds(40);  // Simulate for 40 seconds
    CommandLine cmd;
    cmd.Parse(argc, argv);

    // Initialize data file
    dataFile << "Time(s),Cell_A_UEs,Cell_B_UEs\n";

    // LTE setup
    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
    Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
    lteHelper->SetEpcHelper(epcHelper);

    // Handover configuration
    lteHelper->SetHandoverAlgorithmType("ns3::A3RsrpHandoverAlgorithm");
    lteHelper->SetHandoverAlgorithmAttribute("Hysteresis", DoubleValue(1.5));
    lteHelper->SetHandoverAlgorithmAttribute("TimeToTrigger", TimeValue(MilliSeconds(256)));

    // Create nodes
    NodeContainer enbNodes;
    enbNodes.Create(2);  // Two eNodeBs
    NodeContainer ueNodes;
    ueNodes.Create(32);  // 32 UEs

    // Position eNodeBs
    MobilityHelper enbMobility;
    enbMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    enbMobility.Install(enbNodes);
    enbNodes.Get(0)->GetObject<MobilityModel>()->SetPosition(Vector(0, 0, 0));    // Cell_A
    enbNodes.Get(1)->GetObject<MobilityModel>()->SetPosition(Vector(500, 0, 0)); // Cell_B

    // Position UEs (half near each eNodeB)
    MobilityHelper ueMobility;
    ueMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    ueMobility.Install(ueNodes);
    for (uint32_t i = 0; i < 16; ++i) {
        ueNodes.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(50 + i*5, 10, 0));
    }
    for (uint32_t i = 16; i < 32; ++i) {
        ueNodes.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(550 + (i-16)*5, -10, 0));
    }

    // Install LTE devices
    NetDeviceContainer enbDevices = lteHelper->InstallEnbDevice(enbNodes);
    NetDeviceContainer ueDevices = lteHelper->InstallUeDevice(ueNodes);

    // Install IP stack
    InternetStackHelper internet;
    internet.Install(ueNodes);
    Ipv4InterfaceContainer ueIpIfaces = epcHelper->AssignUeIpv4Address(ueDevices);

    // Initial attachment
    for (uint32_t i = 0; i < 16; ++i) {
        lteHelper->Attach(ueDevices.Get(i), enbDevices.Get(0)); // UEs 0-15 -> Cell_A
    }
    for (uint32_t i = 16; i < 32; ++i) {
        lteHelper->Attach(ueDevices.Get(i), enbDevices.Get(1)); // UEs 16-31 -> Cell_B
    }

    // === Handover Events ===
    // 20s: Move 4 UEs from Cell_A to Cell_B, 8 UEs from Cell_B to Cell_A
    Simulator::Schedule(Seconds(20), [&ueNodes]() {
        std::cout << "\n[Handover Event at 20s]\n";
        for (uint32_t i = 0; i < 4; ++i) {
            ueNodes.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(550 + i*10, 20, 0));
        }
        for (uint32_t i = 0; i < 8; ++i) {
            ueNodes.Get(16 + i)->GetObject<MobilityModel>()->SetPosition(Vector(50 + i*10, -20, 0));
        }
    });

    // 40s: Rebalance UEs
    Simulator::Schedule(Seconds(40), [&ueNodes]() {
        std::cout << "\n[Rebalance Event at 40s]\n";
        for (uint32_t i = 0; i < 4; ++i) {
            ueNodes.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(50 + i*10, 30, 0));
        }
        for (uint32_t i = 0; i < 8; ++i) {
            ueNodes.Get(16 + i)->GetObject<MobilityModel>()->SetPosition(Vector(550 + i*10, 30, 0));
        }
    });

    // Schedule status prints every second (1-40s)
    for (int t = 1; t <= 40; ++t) {
        Simulator::Schedule(Seconds(t), &PrintUeStatus, lteHelper, ueDevices, t);
    }

    Simulator::Stop(simTime);
    Simulator::Run();
    dataFile.close();
    Simulator::Destroy();
    
    std::cout << "\nSimulation completed. CSV data saved to handover_data.csv\n";
    return 0;
}
```


**20 sec status**
![messageImage_1744312867415_0](https://github.com/user-attachments/assets/945522f9-f473-4c83-b51f-8c867910bca4)


**40 sec status**
![messageImage_1744312883414_0](https://github.com/user-attachments/assets/119d3ae0-f098-42cf-b6c0-089777b059e1)






