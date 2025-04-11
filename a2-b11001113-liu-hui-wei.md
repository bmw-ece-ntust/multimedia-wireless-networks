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
1.  Node Composition: The simulation includes 2 access points (AP-A and AP-B) and 32 stations (STAs, divided into two groups: wifiStaNodesA and wifiStaNodesB, each with 16 STAs).
2.  Network Segmentation: AP-A and its STAs use the 10.1.1.0/24 subnet, while AP-B and its STAs use the 10.1.2.0/24 subnet.
3.  Traffic Setup: AP-A sends UDP traffic to the first STA in wifiStaNodesA, and AP-B sends UDP traffic to the first STA in wifiStaNodesB, simulating AP-to-STA communication.

**WiFi Configuration:**
1.  WiFi Standard: Uses the 802.11n standard with ConstantRateWifiManager for rate control, setting both data and control modes to HtMcs7.
2.  SSID Configuration: AP-A uses the SSID "NodeA", and AP-B uses the SSID "NodeB"; wifiStaNodesA are initially associated with AP-A, and wifiStaNodesB with AP-B.
3.  Physical Layer: Utilizes YansWifiChannelHelper to create a default WiFi channel, with no specific adjustments to physical layer parameters.

**Positioning:**
1.  Node Positioning: AP-A is fixed at (0, 0, 0), and AP-B is fixed at (100, 0, 0), simulating a spatial separation of 100 units.
2.  STA Positioning: Uses GridPositionAllocator to assign STA positions, starting at (0, 0) with a spacing of 10 units, a grid width of 3, and a row-first layout; all nodes use ConstantPositionMobilityModel, keeping positions fixed without movement.



```cpp
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/ipv4-flow-classifier.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SetupNodesWithThroughput");

void PrintMacAddresses(NetDeviceContainer devices, std::string nodeName) {
    std::cout << "MAC Addresses for " << nodeName << " STAs:" << std::endl;
    for (uint32_t i = 0; i < devices.GetN(); ++i) {
        Ptr<NetDevice> device = devices.Get(i);
        Mac48Address mac = Mac48Address::ConvertFrom(device->GetAddress());
        std::cout << "STA " << i + 1 << ": " << mac << std::endl;
    }
    std::cout << std::endl;
}

void CalculateThroughput(Ptr<FlowMonitor> monitor, Ptr<Ipv4FlowClassifier> classifier, double duration) {
    monitor->CheckForLostPackets();
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();
    double totalThroughput = 0.0;

    for (const auto& stat : stats) {
        double throughput = stat.second.rxBytes * 8.0 / duration / 1e6; // Mbps
        totalThroughput += throughput;
        std::cout << "Flow " << stat.first << ": Throughput = " << throughput << " Mbps" << std::endl;
    }
    std::cout << "Total Throughput: " << totalThroughput << " Mbps" << std::endl;
}

int main(int argc, char* argv[]) {
    double simTime = 10.0; // Simulation time in seconds

    // 1. Create nodes
    NodeContainer wifiApNodes;
    wifiApNodes.Create(2);
    NodeContainer wifiStaNodesA, wifiStaNodesB;
    wifiStaNodesA.Create(16);
    wifiStaNodesB.Create(16);

    // 2. Setup WiFi
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

    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssidA));
    NetDeviceContainer apDeviceA = wifi.Install(phy, mac, wifiApNodes.Get(0));

    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssidB));
    NetDeviceContainer apDeviceB = wifi.Install(phy, mac, wifiApNodes.Get(1));

    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssidA));
    NetDeviceContainer staDevicesA = wifi.Install(phy, mac, wifiStaNodesA);

    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssidB));
    NetDeviceContainer staDevicesB = wifi.Install(phy, mac, wifiStaNodesB);

    // 3. Mobility
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

    Ptr<MobilityModel> mobilityApA = wifiApNodes.Get(0)->GetObject<MobilityModel>();
    mobilityApA->SetPosition(Vector(0.0, 0.0, 0.0));
    Ptr<MobilityModel> mobilityApB = wifiApNodes.Get(1)->GetObject<MobilityModel>();
    mobilityApB->SetPosition(Vector(100.0, 0.0, 0.0));

    // 4. Install Internet stack
    InternetStackHelper stack;
    stack.Install(wifiApNodes);
    stack.Install(wifiStaNodesA);
    stack.Install(wifiStaNodesB);

    // 5. Assign IP addresses
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer apInterfaceA = address.Assign(apDeviceA);
    Ipv4InterfaceContainer staInterfacesA = address.Assign(staDevicesA);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer apInterfaceB = address.Assign(apDeviceB);
    Ipv4InterfaceContainer staInterfacesB = address.Assign(staDevicesB);

    // 6. Setup UDP traffic (AP to one STA per network)
    uint16_t port = 9;
    UdpServerHelper server(port);
    ApplicationContainer serverAppA = server.Install(wifiStaNodesA.Get(0)); // STA 1 of Node A
    ApplicationContainer serverAppB = server.Install(wifiStaNodesB.Get(0)); // STA 1 of Node B
    serverAppA.Start(Seconds(1.0));
    serverAppB.Start(Seconds(1.0));
    serverAppA.Stop(Seconds(simTime));
    serverAppB.Stop(Seconds(simTime));

    UdpClientHelper clientA(staInterfacesA.GetAddress(0), port);
    clientA.SetAttribute("MaxPackets", UintegerValue(1000000));
    clientA.SetAttribute("Interval", TimeValue(MilliSeconds(1)));
    clientA.SetAttribute("PacketSize", UintegerValue(1024));
    ApplicationContainer clientAppA = clientA.Install(wifiApNodes.Get(0)); // AP A
    clientAppA.Start(Seconds(2.0));
    clientAppA.Stop(Seconds(simTime));

    UdpClientHelper clientB(staInterfacesB.GetAddress(0), port);
    clientB.SetAttribute("MaxPackets", UintegerValue(1000000));
    clientB.SetAttribute("Interval", TimeValue(MilliSeconds(1)));
    clientB.SetAttribute("PacketSize", UintegerValue(1024));
    ApplicationContainer clientAppB = clientB.Install(wifiApNodes.Get(1)); // AP B
    clientAppB.Start(Seconds(2.0));
    clientAppB.Stop(Seconds(simTime));

    // 7. FlowMonitor for throughput
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());

    // 8. Print MAC addresses
    PrintMacAddresses(staDevicesA, "Node A");
    PrintMacAddresses(staDevicesB, "Node B");

    // 9. Run simulation
    Simulator::Stop(Seconds(simTime));
    Simulator::Run();

    // 10. Calculate throughput
    CalculateThroughput(monitor, classifier, simTime - 2.0); // Exclude startup time

    Simulator::Destroy();
    return 0;
}

```
![messageImage_1744372125251](https://github.com/user-attachments/assets/ca9c412d-0b6d-499f-b76a-2b49752ec219)



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
1. Environment Setup: The program uses ns-3 to simulate a WiFi network, consisting of 2 access points (AP-A and AP-B), 32 stations (STAs, divided into two groups: staNodesA and staNodesB, each with 16 STAs), and 1 remote host (remoteHost).
2. Node Positioning: AP-A is located at (0, 0, 0), AP-B at (500, 0, 0); staNodesA are initially positioned around x=50, and staNodesB around x=450, simulating spatial distribution.
3. Network Stack: An Internet protocol stack is installed on all nodes, and a point-to-point (P2P) link connects AP-A to the remoteHost with a data rate of 100Mbps and a delay of 2ms.
4. IP Assignment: APs and STAs use the 10.1.1.0/24 subnet, while the remoteHost uses the 7.0.0.0/8 subnet. Static routing is configured to ensure the remoteHost can reach all STAs.

**Handover Simulation:**
1. Simulation Scenario: The simulation runs for 40 seconds with two mobility events to achieve load balancing:
- At 10 seconds: Move 4 STAs from AP-A to AP-B’s coverage area, expecting AP-A to have 20 users and AP-B to have 12 users.
- At 20 seconds: Move 4 STAs from AP-A back to AP-A, and move 8 STAs from AP-B to AP-A, expecting AP-A and AP-B to each have 16 users.
2. Reassociation: The ReassociateStaToNearestAp function dynamically adjusts STA associations based on their distance to APs, with STAs connecting to the nearest AP (AP-A or AP-B).
3. Connection Status Monitoring: The PrintStaConnectionStatus function prints the number of users on AP-A and AP-B every second, verifying load balancing at 10, 20, and 40 seconds.

**Data Collection:**
1. Throughput Logging: The MonitorThroughput function uses FlowMonitor to calculate the throughput of AP-A and AP-B every 0.1 seconds, logging the results to throughput.csv.
2. Position Logging: The LogStaPositions function records the positions (x, y coordinates) of all STAs, saving them to sta_positions.csv.
3. Final Statistics: At the end of the simulation, the CalculateFinalThroughput function computes the total throughput, outputting the number of transmitted packets (TX Packets), received packets (RX Packets), and overall throughput (Throughput).

**Key Configurations:**
1. WiFi Settings: Uses the 802.11n standard with MinstrelHtWifiManager as the rate control algorithm, and sets the transmit power to 20 dBm.
2. UDP Traffic: The remoteHost sends UDP traffic to each STA with a packet size of 512 bytes, a sending interval of 10 milliseconds, and a maximum of 100,000 packets, running from 1 second to 40 seconds.
3. FlowMonitor: Starts monitoring traffic from 1 second, capturing statistics for all nodes.
4. Logging: Enables logging for WifiHandoverSim (INFO level) and disables tracing logs for UdpClient and UdpServer (set to ERROR level) to reduce output interference.


```cpp
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/point-to-point-module.h"
#include <fstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("WifiHandoverSim");

// 全局變數，用於存儲 AP 和 STA 設備
NetDeviceContainer apDevices;
NetDeviceContainer staDevices;
NodeContainer apNodes;
NodeContainer staNodesA, staNodesB;

// 全局輸出文件
static std::ofstream throughputFile;
static std::ofstream posFile;

// 啟用日誌以調試流量
static void EnableLogging() {
    LogComponentEnable("WifiHandoverSim", LOG_LEVEL_INFO);
    // 禁用 UdpClient 和 UdpServer 的預設追蹤日誌
    LogComponentEnable("UdpClient", LOG_LEVEL_ERROR);
    LogComponentEnable("UdpServer", LOG_LEVEL_ERROR);
}

// 檢查 STA 與 AP 的距離，模擬重新關聯
void ReassociateStaToNearestAp() {
    NodeContainer allStaNodes;
    allStaNodes.Add(staNodesA);
    allStaNodes.Add(staNodesB);

    for (uint32_t i = 0; i < allStaNodes.GetN(); ++i) {
        Ptr<MobilityModel> staMobility = allStaNodes.Get(i)->GetObject<MobilityModel>();
        Vector staPos = staMobility->GetPosition();

        // 計算與 AP-A 和 AP-B 的距離
        Ptr<MobilityModel> apAMobility = apNodes.Get(0)->GetObject<MobilityModel>();
        Ptr<MobilityModel> apBMobility = apNodes.Get(1)->GetObject<MobilityModel>();
        double distToApA = CalculateDistance(staPos, apAMobility->GetPosition());
        double distToApB = CalculateDistance(staPos, apBMobility->GetPosition());

        // 獲取 STA 的 MAC 層
        Ptr<NetDevice> staDevice = staDevices.Get(i);
        Ptr<WifiNetDevice> wifiStaDevice = DynamicCast<WifiNetDevice>(staDevice);
        Ptr<StaWifiMac> staMac = DynamicCast<StaWifiMac>(wifiStaDevice->GetMac());

        // 根據距離選擇最近的 AP
        if (distToApA < distToApB) {
            staMac->SetSsid(Ssid("wifi-network-1"));
        } else {
            staMac->SetSsid(Ssid("wifi-network-2"));
        }
    }
}

// 打印所有 STA 的連線狀態
void PrintStaConnectionStatus(NetDeviceContainer staDevices, uint32_t numStas) {
    int apACount = 0, apBCount = 0;
    for (uint32_t i = 0; i < numStas; ++i) {
        Ptr<NetDevice> staDevice = staDevices.Get(i);
        Ptr<WifiNetDevice> wifiStaDevice = DynamicCast<WifiNetDevice>(staDevice);
        Ptr<StaWifiMac> staMac = DynamicCast<StaWifiMac>(wifiStaDevice->GetMac());
        std::string ssid = staMac->GetSsid().PeekString();
        if (ssid == "wifi-network-1") {
            apACount++;
        } else if (ssid == "wifi-network-2") {
            apBCount++;
        }
    }
    std::cout << "TIME: " << Simulator::Now().GetSeconds() << "s AP-A has " << apACount << " users, AP-B has " << apBCount << " users." << std::endl;

    // 驗證負載平衡
    if (Simulator::Now().GetSeconds() == 10) {
        std::cout << "AP-A should have 20 users (16 + 25% * 16), AP-B should have 12 users (16 - 25% of AP-B's 16)" << std::endl;
        double apAPercent = (apACount / 32.0) * 100.0;
        double apBPercent = (apBCount / 32.0) * 100.0;
        std::cout << "AP-A has " << apACount << " users (" << apAPercent << "% of users), "
                  << "AP-B has " << apBCount << " users (" << apBPercent << "% of users)." << std::endl;
    } else if (Simulator::Now().GetSeconds() == 20) {
        std::cout << "Using calculated percentages to ensure proper balance" << std::endl;
        std::cout << "AP-A current user count: " << apACount << std::endl;
        std::cout << "AP-B current user count: " << apBCount << std::endl;
        if (apACount == 16 && apBCount == 16) {
            std::cout << "PERFECT BALANCE ACHIEVED!" << std::endl;
        }
    } else if (Simulator::Now().GetSeconds() == 40) {
        std::cout << "AP-A should have exactly 16 users to ensure proper balance" << std::endl;
        std::cout << "AP-B should have exactly 16 users to ensure proper balance" << std::endl;
        double apAPercent = (apACount / 32.0) * 100.0;
        double apBPercent = (apBCount / 32.0) * 100.0;
        std::cout << "AP-A has " << apACount << " users (" << apAPercent << "% of users), "
                  << "AP-B has " << apBCount << " users (" << apBPercent << "% of users)." << std::endl;
    }
}

// 記錄 STA 位置
void LogStaPositions(NodeContainer staNodesA, NodeContainer staNodesB, uint32_t numStas, std::ofstream& posFile) {
    NodeContainer allStaNodes;
    allStaNodes.Add(staNodesA);
    allStaNodes.Add(staNodesB);

    for (uint32_t i = 0; i < numStas; ++i) {
        Ptr<MobilityModel> mob = allStaNodes.Get(i)->GetObject<MobilityModel>();
        Vector pos = mob->GetPosition();
        posFile << Simulator::Now().GetSeconds() << "," << i << "," << pos.x << "," << pos.y << "\n";
    }
}

// 監控並記錄吞吐量
void MonitorThroughput(Ptr<FlowMonitor> flowMon, Ptr<Ipv4FlowClassifier> classifier, NetDeviceContainer staDevices, Ipv4InterfaceContainer staIpIfaces, uint32_t numStas) {
    flowMon->CheckForLostPackets();
    FlowMonitor::FlowStatsContainer stats = flowMon->GetFlowStats();

    double throughputApA = 0.0; // AP-A (SSID: wifi-network-1)
    double throughputApB = 0.0; // AP-B (SSID: wifi-network-2)

    for (auto it = stats.begin(); it != stats.end(); ++it) {
        uint32_t flowId = it->first;
        Ipv4FlowClassifier::FiveTuple tuple = classifier->FindFlow(flowId);
        for (uint32_t i = 0; i < numStas; ++i) {
            if (tuple.destinationAddress == staIpIfaces.GetAddress(i)) {
                Ptr<NetDevice> staDevice = staDevices.Get(i);
                Ptr<WifiNetDevice> wifiStaDevice = DynamicCast<WifiNetDevice>(staDevice);
                Ptr<StaWifiMac> staMac = DynamicCast<StaWifiMac>(wifiStaDevice->GetMac());
                std::string ssid = staMac->GetSsid().PeekString();
                double tput = 0.0;
                if (it->second.timeLastRxPacket > it->second.timeFirstTxPacket) {
                    tput = (it->second.rxBytes * 8.0) /
                           (it->second.timeLastRxPacket.GetSeconds() - it->second.timeFirstTxPacket.GetSeconds()) / 1e6;
                }
                if (ssid == "wifi-network-1") {
                    throughputApA += tput;
                } else if (ssid == "wifi-network-2") {
                    throughputApB += tput;
                }
                break;
            }
        }
    }

    throughputFile << Simulator::Now().GetSeconds() << "," << throughputApA << "," << throughputApB << "\n";

    // 每 0.1 秒檢查一次吞吐量
    if (Simulator::Now().GetSeconds() < 40.0) {
        Simulator::Schedule(Seconds(0.1), &MonitorThroughput, flowMon, classifier, staDevices, staIpIfaces, numStas);
    }
}

// 計算最終吞吐量
void CalculateFinalThroughput(Ptr<FlowMonitor> flowMon, Ptr<Ipv4FlowClassifier> classifier, double duration) {
    flowMon->CheckForLostPackets();
    std::map<FlowId, FlowMonitor::FlowStats> stats = flowMon->GetFlowStats();
    double totalThroughput = 0.0;
    uint64_t totalTxPackets = 0, totalRxPackets = 0;

    for (const auto& stat : stats) {
        totalThroughput += stat.second.rxBytes * 8.0 / duration / 1e6; // Mbps
        totalTxPackets += stat.second.txPackets;
        totalRxPackets += stat.second.rxPackets;
    }

    std::cout << "*** Flow monitor statistics ***" << std::endl;
    std::cout << "TX Packets: " << totalTxPackets << std::endl;
    std::cout << "RX Packets: " << totalRxPackets << std::endl;
    std::cout << "Throughput: " << totalThroughput << " Mbps" << std::endl;
}

int main(int argc, char *argv[]) {
    Time simTime = Seconds(40.0);

    // 啟用日誌
    EnableLogging();

    // 打開輸出文件
    throughputFile.open("throughput.csv");
    throughputFile << "Time,Throughput_ApA,Throughput_ApB\n";
    posFile.open("sta_positions.csv");
    posFile << "Time,STA_Index,X,Y\n";

    // 模擬場景描述
    std::cout << "*** SIMULATION SCENARIO ***" << std::endl;
    std::cout << "10-20s: Movement at 10s - AP-A should have 20 users, AP-B should have 12 users." << std::endl;
    std::cout << "20-30s: Movement at 20s - AP-A and AP-B each return to 16 users." << std::endl;

    // 創建節點
    apNodes.Create(2);
    staNodesA.Create(16); // 第一組 STA
    staNodesB.Create(16); // 第二組 STA
    NodeContainer remoteHost;
    remoteHost.Create(1);

    // 設置 AP 位置
    MobilityHelper apMobility;
    apMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    apMobility.Install(apNodes);
    apNodes.Get(0)->GetObject<MobilityModel>()->SetPosition(Vector(0, 0, 0));
    apNodes.Get(1)->GetObject<MobilityModel>()->SetPosition(Vector(500, 0, 0));

    // 設置 STA 位置
    MobilityHelper staMobility;
    staMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    staMobility.Install(staNodesA);
    staMobility.Install(staNodesB);
    for (uint32_t i = 0; i < 16; ++i) {
        staNodesA.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(50 + i*5, 10, 0));
        staNodesB.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(450 + i*5, -10, 0));
    }

    // 設置遠程主機位置
    MobilityHelper remoteMobility;
    remoteMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    remoteMobility.Install(remoteHost);

    // WiFi 設置
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211n);
    wifi.SetRemoteStationManager("ns3::MinstrelHtWifiManager");

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());
    phy.Set("TxPowerStart", DoubleValue(20.0)); // 增加發射功率
    phy.Set("TxPowerEnd", DoubleValue(20.0));

    WifiMacHelper mac;
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(Ssid("wifi-network-1")));
    NetDeviceContainer apDevicesA = wifi.Install(phy, mac, apNodes.Get(0));

    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(Ssid("wifi-network-2")));
    NetDeviceContainer apDevicesB = wifi.Install(phy, mac, apNodes.Get(1));

    apDevices.Add(apDevicesA);
    apDevices.Add(apDevicesB);

    // 為 STA 設置初始 SSID
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(Ssid("wifi-network-1")));
    NetDeviceContainer staDevicesA = wifi.Install(phy, mac, staNodesA);
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(Ssid("wifi-network-2")));
    NetDeviceContainer staDevicesB = wifi.Install(phy, mac, staNodesB);

    staDevices.Add(staDevicesA);
    staDevices.Add(staDevicesB);

    // 安裝網路堆疊
    InternetStackHelper internet;
    internet.Install(staNodesA);
    internet.Install(staNodesB);
    internet.Install(apNodes);
    internet.Install(remoteHost);

    // 設置點對點鏈路（連接到遠程主機）
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));
    NetDeviceContainer hostDevices = p2p.Install(apNodes.Get(0), remoteHost.Get(0));

    // 分配 IP 地址
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer apInterfaces = address.Assign(apDevices);
    Ipv4InterfaceContainer staInterfaces = address.Assign(staDevices);
    address.SetBase("7.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer hostInterfaces = address.Assign(hostDevices);

    // 添加靜態路由，確保 remoteHost 可以到達所有 STA
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4> remoteHostIpv4 = remoteHost.Get(0)->GetObject<Ipv4>();
    Ptr<Ipv4StaticRouting> remoteHostRouting = ipv4RoutingHelper.GetStaticRouting(remoteHostIpv4);
    remoteHostRouting->AddNetworkRouteTo(Ipv4Address("10.1.1.0"), Ipv4Mask("255.255.255.0"), 1);

    // 設置 UDP 流量（從遠程主機到 STA）
    uint16_t basePort = 49153;
    ApplicationContainer serverApps, clientApps;
    for (uint32_t i = 0; i < 32; ++i) {
        UdpServerHelper udpServer(basePort + i);
        NodeContainer allStaNodes;
        allStaNodes.Add(staNodesA);
        allStaNodes.Add(staNodesB);
        serverApps.Add(udpServer.Install(allStaNodes.Get(i)));
        serverApps.Start(Seconds(0.5));
        serverApps.Stop(simTime);

        UdpClientHelper udpClient(staInterfaces.GetAddress(i), basePort + i);
        udpClient.SetAttribute("MaxPackets", UintegerValue(100000));
        udpClient.SetAttribute("Interval", TimeValue(MilliSeconds(10)));
        udpClient.SetAttribute("PacketSize", UintegerValue(512));
        clientApps.Add(udpClient.Install(remoteHost.Get(0)));
        clientApps.Start(Seconds(1.0));
        clientApps.Stop(simTime);
    }

    // 設置 FlowMonitor
    FlowMonitorHelper flowMonHelper;
    flowMonHelper.SetMonitorAttribute("StartTime", TimeValue(Seconds(1.0)));
    Ptr<FlowMonitor> flowMon = flowMonHelper.InstallAll();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowMonHelper.GetClassifier());

    // 安排初始任務
    Simulator::Schedule(Seconds(1.0), &PrintStaConnectionStatus, staDevices, 32);
    Simulator::Schedule(Seconds(1.0), &MonitorThroughput, flowMon, classifier, staDevices, staInterfaces, 32);
    Simulator::Schedule(Seconds(1.0), &LogStaPositions, staNodesA, staNodesB, 32, std::ref(posFile));

    // 第一次移動事件（10 秒）
    Simulator::Schedule(Seconds(10.0), []() {
        std::cout << "\n*** Expected after first movement at 10s ***\n";
        for (uint32_t i = 0; i < 4; ++i) {
            staNodesA.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(450 + i*10, 20, 0));
        }
        ReassociateStaToNearestAp();
    });

    // 第一次移動後打印連線狀態
    Simulator::Schedule(Seconds(10.0), &PrintStaConnectionStatus, staDevices, 32);

    // 第二次移動事件（20 秒）
    Simulator::Schedule(Seconds(20.0), []() {
        std::cout << "\n*** Expected after second movement at 20s ***\n";
        std::cout << "Moving 4 users from A to B and 8 users from B to A to achieve balance" << std::endl;
        for (uint32_t i = 0; i < 4; ++i) {
            staNodesA.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(50 + i*10, 30, 0));
        }
        for (uint32_t i = 0; i < 8; ++i) {
            staNodesB.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(50 + i*10, -20, 0));
        }
        ReassociateStaToNearestAp();
    });

    // 第二次移動後打印連線狀態
    Simulator::Schedule(Seconds(20.0), &PrintStaConnectionStatus, staDevices, 32);

    // 定期打印連線狀態（每秒）
    for (int t = 1; t <= 40; ++t) {
        Simulator::Schedule(Seconds(t), &PrintStaConnectionStatus, staDevices, 32);
    }

    // 最終位置記錄
    Simulator::Schedule(simTime, &LogStaPositions, staNodesA, staNodesB, 32, std::ref(posFile));

    // 最終用戶分佈和吞吐量
    Simulator::Schedule(Seconds(40.0), [flowMon, classifier]() {
        std::cout << "\n*** Final user distribution ***\n";
        int apACount = 0, apBCount = 0;
        for (uint32_t i = 0; i < staDevices.GetN(); ++i) {
            Ptr<NetDevice> staDevice = staDevices.Get(i);
            Ptr<WifiNetDevice> wifiStaDevice = DynamicCast<WifiNetDevice>(staDevice);
            Ptr<StaWifiMac> staMac = DynamicCast<StaWifiMac>(wifiStaDevice->GetMac());
            std::string ssid = staMac->GetSsid().PeekString();
            if (ssid == "wifi-network-1") apACount++;
            else if (ssid == "wifi-network-2") apBCount++;
        }
        std::cout << "AP-A final user count: " << apACount << std::endl;
        std::cout << "AP-B final user count: " << apBCount << std::endl;
        std::cout << "Total users in system: " << (apACount + apBCount) << std::endl;

        // 計算最終吞吐量
        CalculateFinalThroughput(flowMon, classifier, 38.0); // 排除啟動時間
    });

    // 運行模擬
    Simulator::Stop(simTime);
    Simulator::Run();

    // 關閉文件
    throughputFile.close();
    posFile.close();

    Simulator::Destroy();
    std::cout << "\nSimulation completed successfully!\n";
    return 0;
}
```


**Output**
![messageImage_1744371857794_0](https://github.com/user-attachments/assets/9560cb2e-f5be-4116-8b89-d43d8de04679)
![messageImage_1744371873966_0](https://github.com/user-attachments/assets/a8fa2764-1199-4a4d-81d3-94c19b16f535)
![messageImage_1744371887511_0](https://github.com/user-attachments/assets/61da1955-1dea-4c53-8760-29a45966ccfd)


### Analysis reports
**1. User Distribution Issues**

Possible Causes:
- Reassociation Logic Error:
   - The simulation uses the ReassociateStaToNearestAp function to reassign STAs based on their distance to APs. However, there may be a logic error causing STAs to associate with the wrong AP. For example, at 10 seconds, 4 STAs are moved from AP-A to AP-B's coverage area, but after reassociation, they may still associate with AP-A.
     
- Distance Calculation Issue:
   - The distance calculation may be incorrect (e.g., due to inconsistent coordinate systems), leading STAs to associate with the wrong AP.
     
- Reassociation Not Triggered:
   - After a mobility event, the STA's MAC layer may not update its SSID in time, causing the user distribution to not change as expected.
     
- Incorrect STA Selection for Mobility Events:
   - The number or direction of moved STAs may be incorrect. For example, at 10 seconds, 4 STAs should move from AP-A to AP-B, but the actual movement direction may be reversed.

**Impact:**
Incorrect user distribution leads to load imbalance, with AP-A and AP-B user counts not meeting expectations, affecting the simulation's load balancing goal.

**2. Throughput Issues**

Possible Causes:
- High Packet Loss Rate:
  - The packet loss rate is approximately 70.5%, with potential causes including:
    - STAs Too Far from APs: After mobility events, some STAs may move beyond the AP's coverage range, causing packets to fail transmission. For example, STAs moved to x = 450 may be outside AP-B's coverage.
    - WiFi Interference and Collisions: With 32 STAs transmitting UDP traffic simultaneously, channel contention and collisions may increase packet loss.
    - AP Overload: After the first mobility event at 10 seconds, AP-B has 20 users, potentially overloading AP-B and preventing it from handling all traffic.
      
- Throughput Calculation Issue:
  - FlowMonitor may not correctly capture all traffic, or there may be errors in the calculation logic. For example, after a mobility event, STAs may disconnect from the AP, interrupting traffic and affecting throughput statistics.
    
- Inappropriate UDP Traffic Parameters:
  - Current settings:
    - Packet size: 512 bytes
    - Sending interval: 10 milliseconds (100 packets/second)
    - Theoretical throughput per STA: 100 * 512 * 8 = 409,600 bits/second = 0.4096 Mbps
    - Total theoretical throughput for 32 STAs: 32 * 0.4096 = 13.1072 Mbps
  - The actual throughput is only 0.409263 Mbps, far below the theoretical value, primarily due to the high packet loss rate.
    
- Poor Receiver Performance:
  - The poor performance of the receiver (STAs) may be due to the following reasons:
    - WiFi Physical Layer Parameters: The transmit power (TxPower) may be insufficient, leading to low signal strength and packet loss. The current setting is 20 dBm, which may not be enough to cover STAs after mobility.
    - STA Processing Capability: The STA's MAC layer may not be able to handle high-frequency packets, resulting in packet loss.
    - Connection Interruption After Mobility: STAs may not reassociate with a new AP in time after moving, causing traffic interruption.
    - Channel Interference: The WiFi channel may experience interference (e.g., multiple STAs sharing the same channel), leading to packet loss.
      
**Impact:**
- A high packet loss rate results in throughput far below expectations, failing to reflect the simulation's true performance.
- Poor receiver performance affects the simulation's load balancing effectiveness, as some STAs cannot receive traffic properly.

**Solutions**
1. Fix User Distribution Issues
- Adjust Reassociation Logic:
  - Ensure the ReassociateStaToNearestAp function correctly calculates distances and reassigns STAs immediately after mobility events.
  - Add logging to verify the reassociation result for each STA, ensuring they associate with the correct AP.
    
- Correct STA Selection for Mobility Events:
  - Ensure the number and direction of moved STAs are correct:
     - At 10 seconds: Move 4 STAs from AP-A to AP-B.
     - At 20 seconds: Move 4 STAs from AP-A back to AP-A, and move 8 STAs from AP-B to AP-A.
       
- Delay Reassociation:
  - Introduce a slight delay (e.g., 0.1 seconds) after mobility events before performing reassociation, ensuring STA position updates are complete.
    
2. Fix Throughput Issues
- Reduce Packet Loss:
  - Increase WiFi transmit power (TxPower), e.g., from 20 dBm to 30 dBm, to ensure STAs remain connected to APs after mobility.
  - Adjust the initial positions of STAs to ensure they remain within the AP's coverage range before and after mobility. For example, adjust staNodesB initial positions from x = 450 to a position closer to AP-B.
  - Reduce the UDP traffic sending frequency, e.g., increase the Interval from 10ms to 20ms, to reduce channel contention.
    
- Ensure Connection Stability:
  - After mobility events, check the connection status of STAs to ensure they are correctly associated with the new AP.
  - If an STA disconnects, force reassociation.
    
- Adjust Throughput Calculation:
  - Ensure FlowMonitor correctly captures all traffic, and verify the logic in MonitorThroughput and CalculateFinalThroughput.
  - Extend the time range for traffic statistics to exclude the impact of startup and mobility events.
    
- Optimize Receiver Performance:
  - Increase the transmit power of APs and STAs to improve signal strength.
  - Reduce the number of STAs transmitting simultaneously, or use different WiFi channels to reduce interference.
  - Adjust the UDP packet size (e.g., increase to 1024 bytes) to improve throughput.
    
**Expected Improvements**
- User Distribution:
  - At 10 seconds: AP-A has 20 users, AP-B has 12 users.
  - At 20 seconds: AP-A and AP-B each have 16 users, achieving load balancing.
    
- Throughput:
  - Reduce packet loss rate (target below 10%).
  - Increase throughput, expected to reach 5-10 Mbps, closer to the theoretical value.
    
**Conclusion :**
The user distribution issues are primarily caused by errors in reassociation logic and mobility event design, while the throughput issues stem from high packet loss and poor receiver performance. By adjusting the reassociation logic, optimizing mobility events, increasing transmit power, and tuning traffic parameters, these issues can be effectively resolved, improving the accuracy and performance of the simulation.


