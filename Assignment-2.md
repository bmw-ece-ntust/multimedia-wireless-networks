# Multimedia HW2
## A. Install the NS3 in docker
[Install Docker Here](https://www.docker.com)
Add this configure to your local docker. at `setting -> Docker Engine`


```=
{
  "insecure-registries" : ["quay.bmw.lab"]
}
```
![截圖 2025-04-11 下午5.26.12](https://hackmd.io/_uploads/rkcjgP8Ayg.png)
```
docker pull quay.bmw.lab/infidel/ns3:3.38
```
**Result:**
![截圖 2025-04-11 下午5.29.20](https://hackmd.io/_uploads/HkJ-WPU01e.png)
### 1 Testing on ns3
**Run at local bash**
```
docker run -it -v ./task2:/usr/ns3/ns-3.38/contrib/mwn-task2:Z quay.bmw.lab/infidel/ns3:3.38 bash
```
![截圖 2025-04-11 下午5.35.09](https://hackmd.io/_uploads/r1cIzPLC1g.png)

Create your file and compose the path to the container
```
kc@liguanjindeMacBook-Pro ns3-docker % docker compose up -d
```
![截圖 2025-04-11 下午5.33.35](https://hackmd.io/_uploads/BJsxMPUCye.png)

**Run on container**
try `examples/wireless/wifi-80211e-txop.cc`
```
root@879e342b5a47:/usr/ns3/ns-3.38# ./ns3 run examples/wireless/wifi-80211e-txop.cc
```
![截圖 2025-04-11 下午5.41.02](https://hackmd.io/_uploads/Bkh2mvLCye.png)
### 2. simulate wifi-.cc
```
./ns3 run examples/wireless/wifi-tcp.cc 
```

![截圖 2025-04-11 下午5.45.33](https://hackmd.io/_uploads/H1F6NvI01e.png)
## B. Source code
### Settings
**Node Setup:**
* 2 Access Points (AP-A and AP-B) positioned 40m apart
* 16 stations initially associated with AP-A (Group A)
* 16 stations initially associated with AP-B (Group B)

**Mobility Model:**
* APs are stationary (ConstantPositionMobilityModel)
* The simulation includes scheduled node movements between APs

**Network Configuration:**
* IEEE 802.11ax (Wi-Fi 6) standard
* Two separate IP subnets (192.168.1.0/24 for AP-A, 192.168.2.0/24 for AP-B)
* Full UDP traffic from stations to their associated APs

**Traffic Model:**
* Continuous UDP traffic (full-queue model)
* 10 Mbps data rate from each station to its AP
* UDP packet size of 1472 bytes (standard MSS)

**WifiLoadBalancer Class Declaration (Lines 16-81)**
- CreateNodes (Lines 75-91): Creates AP and station nodes, assigning stations to APs
- ConfigureWifi (Lines 94-146): Sets up WiFi devices with 802.11g standard, configures APs and stations
- ConfigureMobility (Lines 149-174): Positions APs statically and stations with random mobility
- SetupInternetStack (Lines 177-187): Configures IP addressing and network stack
- ConfigureApplications (Lines 190-235): Sets up UDP traffic generators and packet sinks
- MigrateUsers (Lines 238-293): Handles station reassignment between APs
- MeasureThroughput (Lines 296-329): Calculates and logs throughput metrics periodically

**Main Function (Lines 345-399)**
- Defines simulation parameters [(Lines 350-361)](https://github.com/bmw-ece-ntust/multimedia-wireless-networks/blob/9ab23e022343a783fc0506f52ff18ce697063810/a2/src/wifi.cc#L79
)
- Processes command-line arguments (Lines 364-375)
- Creates the load balancer with specified parameters (Line 387)
- Schedules migrations at designated times (Lines 390-391)
- Configures throughput monitoring (Line 394)
- Runs the simulation (Line 397)
```cpp
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("WifiLoadBalancer");

// Class to manage WiFi network setup and handover operations
class WifiLoadBalancer {
public:
  WifiLoadBalancer(uint32_t numAps, uint32_t numStations, double simTime) 
    : m_numAps(numAps), 
      m_numStations(numStations), 
      m_simTime(simTime),
      m_throughputFile("ap_throughput.csv") {
    
    // Initialize the throughput file with header
    m_throughputFile << "Time,AP_A_Mbps,AP_B_Mbps" << std::endl;
    
    // Create topology
    CreateNodes();
    ConfigureWifi();
    ConfigureMobility();
    SetupInternetStack();
    ConfigureApplications();
    
    // Initialize m_lastTotalBytes
    for (uint32_t i = 0; i < m_sinks.size(); i++) {
      m_lastTotalBytes.push_back(0);
    }
  }
  
  ~WifiLoadBalancer() {
    m_throughputFile.close();
  }
  
  // Schedule user migration between APs
  void ScheduleMigration(double time, uint32_t moveFromAToB, uint32_t moveFromBToA) {
    Simulator::Schedule(Seconds(time), &WifiLoadBalancer::MigrateUsers, this, 
                       moveFromAToB, moveFromBToA);
    
    NS_LOG_INFO("Scheduled migration at " << time << "s: " 
               << moveFromAToB << " users A->B, " 
               << moveFromBToA << " users B->A");
  }
  
  // Start monitoring throughput at specified intervals
  void StartThroughputMonitoring(double interval) {
    Simulator::Schedule(Seconds(1.0), &WifiLoadBalancer::MeasureThroughput, this, interval);
  }
  
  // Run the simulation
  void Run() {
    NS_LOG_INFO("Starting simulation for " << m_simTime << " seconds");
    Simulator::Stop(Seconds(m_simTime));
    Simulator::Run();
    
    // Print final statistics
    NS_LOG_INFO("Simulation completed. Final statistics:");
    for (uint32_t i = 0; i < m_numAps; i++) {
      NS_LOG_INFO("Total bytes received at AP " << i << ": " << m_sinks[i]->GetTotalRx());
    }
    
    Simulator::Destroy();
  }

private:
  // Create AP and STA nodes
  void CreateNodes() {
    m_apNodes.Create(m_numAps);
    m_staNodes.Create(m_numStations);
    
    // Initially assign half stations to each AP
    uint32_t halfStations = m_numStations / 2;
    
    for (uint32_t i = 0; i < m_numStations; i++) {
      if (i < halfStations) {
        m_stationToAp[i] = 0; // First half to AP A
      } else {
        m_stationToAp[i] = 1; // Second half to AP B
      }
    }
    
    NS_LOG_INFO("Created " << m_numAps << " APs and " << m_numStations << " stations");
  }
  
  // Configure WiFi devices
  void ConfigureWifi() {
    // Setup WiFi standard and manager
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211g);
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                               "DataMode", StringValue("ErpOfdmRate54Mbps"),
                               "ControlMode", StringValue("ErpOfdmRate24Mbps"));
    
    // Setup channel
    YansWifiChannelHelper channel;
    channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    channel.AddPropagationLoss("ns3::LogDistancePropagationLossModel",
                              "Exponent", DoubleValue(3.0));
    
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());
    
    // Configure APs
    WifiMacHelper mac;
    Ssid ssid = Ssid("load-balancer-network");
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    
    // Install WiFi on APs
    m_apDevices = wifi.Install(phy, mac, m_apNodes);
    
    // Save AP MAC addresses for handovers
    for (uint32_t i = 0; i < m_numAps; i++) {
      Ptr<WifiNetDevice> apDev = DynamicCast<WifiNetDevice>(m_apDevices.Get(i));
      Mac48Address apMac = Mac48Address::ConvertFrom(apDev->GetAddress());
      m_apMacs.push_back(apMac);
      NS_LOG_INFO("AP " << i << " MAC: " << apMac);
    }
    
    // Configure stations
    mac.SetType("ns3::StaWifiMac",
               "Ssid", SsidValue(ssid),
               "ActiveProbing", BooleanValue(false));
    
    // Install WiFi on stations
    m_staDevices = wifi.Install(phy, mac, m_staNodes);
    
    // Set initial BSSID for each station
    uint32_t halfStations = m_numStations / 2;
    
    for (uint32_t i = 0; i < m_numStations; i++) {
      Ptr<WifiNetDevice> staDev = DynamicCast<WifiNetDevice>(m_staDevices.Get(i));
      if (i < halfStations) {
        staDev->GetMac()->SetBssid(m_apMacs[0], 0);
      } else {
        staDev->GetMac()->SetBssid(m_apMacs[1], 0);
      }
    }
  }
  
  // Configure mobility model
  void ConfigureMobility() {
    MobilityHelper mobility;
    
    // Position APs
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));   // AP A at origin
    positionAlloc->Add(Vector(40.0, 0.0, 0.0));  // AP B at 40m distance
    
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(m_apNodes);
    
    // Position stations in a disc around middle point
    mobility.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
                                 "X", DoubleValue(20.0),
                                 "Y", DoubleValue(0.0),
                                 "Rho", StringValue("ns3::UniformRandomVariable[Min=0|Max=20]"));
    
    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue(Rectangle(-10, 50, -25, 25)),
                             "Speed", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"));
    
    mobility.Install(m_staNodes);
  }
  
  // Setup IP addressing and Internet stack
  void SetupInternetStack() {
    InternetStackHelper stack;
    stack.Install(m_apNodes);
    stack.Install(m_staNodes);
    
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    
    m_apInterfaces = address.Assign(m_apDevices);
    m_staInterfaces = address.Assign(m_staDevices);
  }
  
  // Configure applications (traffic generators and sinks)
  void ConfigureApplications() {
    uint16_t port = 9;
    
    // Create packet sinks on APs
    PacketSinkHelper sinkHelper("ns3::UdpSocketFactory", 
                               InetSocketAddress(Ipv4Address::GetAny(), port));
    
    for (uint32_t i = 0; i < m_numAps; i++) {
      ApplicationContainer sinkApp = sinkHelper.Install(m_apNodes.Get(i));
      Ptr<PacketSink> sink = DynamicCast<PacketSink>(sinkApp.Get(0));
      sink->SetStartTime(Seconds(0.0));
      sink->SetStopTime(Seconds(m_simTime));
      m_sinks.push_back(sink);
    }
    
    // Setup traffic generators on stations
    OnOffHelper onoff("ns3::UdpSocketFactory", Address());
    onoff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    onoff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    onoff.SetAttribute("DataRate", DataRateValue(DataRate("24Mbps")));
    onoff.SetAttribute("PacketSize", UintegerValue(1472));
    
    ApplicationContainer clientApps;
    
    // Half of stations send to AP A
    uint32_t halfStations = m_numStations / 2;
    
    for (uint32_t i = 0; i < halfStations; i++) {
      AddressValue remoteAddress(InetSocketAddress(m_apInterfaces.GetAddress(0), port));
      onoff.SetAttribute("Remote", remoteAddress);
      clientApps.Add(onoff.Install(m_staNodes.Get(i)));
    }
    
    // Half of stations send to AP B
    for (uint32_t i = halfStations; i < m_numStations; i++) {
      AddressValue remoteAddress(InetSocketAddress(m_apInterfaces.GetAddress(1), port));
      onoff.SetAttribute("Remote", remoteAddress);
      clientApps.Add(onoff.Install(m_staNodes.Get(i)));
    }
    
    clientApps.Start(Seconds(1.0));
    clientApps.Stop(Seconds(m_simTime - 1.0));
  }
  
  // Migrate users between APs
  void MigrateUsers(uint32_t moveFromAToB, uint32_t moveFromBToA) {
    NS_LOG_INFO("Executing migration: " << moveFromAToB << " users A->B, " << moveFromBToA << " users B->A");
    
    std::vector<uint32_t> usersAtA;
    std::vector<uint32_t> usersAtB;
    
    // Find users at each AP
    for (const auto& pair : m_stationToAp) {
      if (pair.second == 0) {
        usersAtA.push_back(pair.first);
      } else {
        usersAtB.push_back(pair.first);
      }
    }
    
    // Adjust migration counts if needed
    moveFromAToB = std::min(moveFromAToB, static_cast<uint32_t>(usersAtA.size()));
    moveFromBToA = std::min(moveFromBToA, static_cast<uint32_t>(usersAtB.size()));
    
    // Move users from A to B
    for (uint32_t i = 0; i < moveFromAToB; i++) {
      uint32_t staIndex = usersAtA[i];
      Ptr<WifiNetDevice> staDev = DynamicCast<WifiNetDevice>(m_staDevices.Get(staIndex));
      
      if (staDev && staDev->GetMac()) {
        staDev->GetMac()->SetBssid(m_apMacs[1], 0);
        m_stationToAp[staIndex] = 1;
        NS_LOG_INFO("Station " << staIndex << " moved from AP A to AP B");
      }
    }
    
    // Move users from B to A
    for (uint32_t i = 0; i < moveFromBToA; i++) {
      uint32_t staIndex = usersAtB[i];
      Ptr<WifiNetDevice> staDev = DynamicCast<WifiNetDevice>(m_staDevices.Get(staIndex));
      
      if (staDev && staDev->GetMac()) {
        staDev->GetMac()->SetBssid(m_apMacs[0], 0);
        m_stationToAp[staIndex] = 0;
        NS_LOG_INFO("Station " << staIndex << " moved from AP B to AP A");
      }
    }
    
    // Log user distribution after migration
    uint32_t usersOnA = 0;
    uint32_t usersOnB = 0;
    
    for (const auto& pair : m_stationToAp) {
      if (pair.second == 0) usersOnA++;
      else usersOnB++;
    }
    
    NS_LOG_INFO("Current distribution: " << usersOnA << " users at AP A, " << usersOnB << " users at AP B");
  }
  
  // Measure throughput on each AP
  void MeasureThroughput(double interval) {
    double currentTime = Simulator::Now().GetSeconds();
    
    if (currentTime < m_simTime) {
      // Calculate throughput for each AP
      m_throughputFile << currentTime;
      
      std::stringstream logMsg;
      logMsg << "Time " << currentTime << "s - Throughput: ";

      for (uint32_t i = 0; i < m_sinks.size(); i++) {
        uint64_t totalBytes = m_sinks[i]->GetTotalRx();
        double throughput = (totalBytes - m_lastTotalBytes[i]) * 8.0 / interval / 1000000; // Mbps
        m_lastTotalBytes[i] = totalBytes;

        m_throughputFile << "," << throughput;

        logMsg << "AP " << i << ": " << throughput << " Mbps";
        if (i < m_sinks.size() - 1) {
            logMsg << ", ";
        }
      }
      
      // Add to log message
      
      

      m_throughputFile << std::endl;

      NS_LOG_INFO(logMsg.str());
      
      // Schedule next measurement
      Simulator::Schedule(Seconds(interval), &WifiLoadBalancer::MeasureThroughput, this, interval);
    }
  }
  
  // Member variables
  uint32_t m_numAps;
  uint32_t m_numStations;
  double m_simTime;
  
  NodeContainer m_apNodes;
  NodeContainer m_staNodes;
  NetDeviceContainer m_apDevices;
  NetDeviceContainer m_staDevices;
  Ipv4InterfaceContainer m_apInterfaces;
  Ipv4InterfaceContainer m_staInterfaces;
  
  std::vector<Ptr<PacketSink>> m_sinks;
  std::vector<uint64_t> m_lastTotalBytes;
  std::vector<Mac48Address> m_apMacs;
  std::map<uint32_t, int> m_stationToAp;
  
  std::ofstream m_throughputFile;
};

int main(int argc, char *argv[]) {
  // Enable logging
  LogComponentEnable("WifiLoadBalancer", LOG_LEVEL_INFO);
  
  // Simulation parameters
  uint32_t nAps = 2;
  uint32_t nStations = 32;
  double simTime = 180.0;            // 3 minutes (60 seconds * 3)
  double firstMigrationTime = 60.0;  // 1 minute
  double secondMigrationTime = 120.0; // 2 minutes
  
  // Migration parameters
  uint32_t firstMoveAtoB = 4;
  uint32_t firstMoveBtoA = 8;
  uint32_t secondMoveAtoB = 10;
  uint32_t secondMoveBtoA = 6;
  
  // Parse command line arguments
  CommandLine cmd;
  cmd.AddValue("simTime", "Simulation time in seconds", simTime);
  cmd.AddValue("firstMigration", "Time of first migration in seconds", firstMigrationTime);
  cmd.AddValue("secondMigration", "Time of second migration in seconds", secondMigrationTime);
  cmd.AddValue("firstMoveAtoB", "Users to move A->B in first migration", firstMoveAtoB);
  cmd.AddValue("firstMoveBtoA", "Users to move B->A in first migration", firstMoveBtoA);
  cmd.AddValue("secondMoveAtoB", "Users to move A->B in second migration", secondMoveAtoB);
  cmd.AddValue("secondMoveBtoA", "Users to move B->A in second migration", secondMoveBtoA);
  cmd.Parse(argc, argv);
  
  // Log simulation parameters
  NS_LOG_INFO("Simulation parameters:");
  NS_LOG_INFO("- Total simulation time: " << simTime << " seconds");
  NS_LOG_INFO("- First migration at " << firstMigrationTime << "s: "
              << firstMoveAtoB << " users A->B, " << firstMoveBtoA << " users B->A");
  NS_LOG_INFO("- Second migration at " << secondMigrationTime << "s: "
              << secondMoveAtoB << " users A->B, " << secondMoveBtoA << " users B->A");
  
  // Create and configure the load balancer
  WifiLoadBalancer loadBalancer(nAps, nStations, simTime);
  
  // Schedule migrations
  loadBalancer.ScheduleMigration(firstMigrationTime, firstMoveAtoB, firstMoveBtoA);
  loadBalancer.ScheduleMigration(secondMigrationTime, secondMoveAtoB, secondMoveBtoA);
  
  // Start throughput monitoring (every 1 second)
  loadBalancer.StartThroughputMonitoring(1.0);
  
  // Run the simulation
  loadBalancer.Run();
  
  return 0;
}
```
## C. Result
- Due to computer capability, i run 50 seconds for simulation
- Stations number on AP

| | Initial  | t = 15 seconds | t = 30 seconds|
| -------- | -------- | -------- |-|
| AP A    | 16     | 20     |16|
|AP B| 16| 12|16




```
root@e467cf78053c:/usr/ns3/ns-3.38# ./ns3 run mira
[0/2] Re-checking globbed directories...
ninja: no work to do.
Time 1s: AP A Throughput : 0 Mbps, AP B Throughput : 0 Mbps
Time 2s: AP A Throughput : 13.5734 Mbps, AP B Throughput : 15.7038 Mbps
Time 3s: AP A Throughput : 13.6262 Mbps, AP B Throughput : 15.7441 Mbps
Time 4s: AP A Throughput : 13.6461 Mbps, AP B Throughput : 15.7525 Mbps
Time 5s: AP A Throughput : 13.6526 Mbps, AP B Throughput : 15.7594 Mbps
Time 6s: AP A Throughput : 13.6513 Mbps, AP B Throughput : 15.765 Mbps
Time 7s: AP A Throughput : 13.6561 Mbps, AP B Throughput : 15.7663 Mbps
Time 8s: AP A Throughput : 13.659 Mbps, AP B Throughput : 15.7689 Mbps
Time 9s: AP A Throughput : 13.662 Mbps, AP B Throughput : 15.7707 Mbps
Time 10s: AP A Throughput : 13.665 Mbps, AP B Throughput : 15.7725 Mbps
Time 11s: AP A Throughput : 13.6629 Mbps, AP B Throughput : 15.7737 Mbps
Time 12s: AP A Throughput : 13.6637 Mbps, AP B Throughput : 15.7733 Mbps
Time 13s: AP A Throughput : 13.6665 Mbps, AP B Throughput : 15.7751 Mbps
Time 14s: AP A Throughput : 13.6669 Mbps, AP B Throughput : 15.7749 Mbps
 4 UEs from move from  A to B; 8 UEs move from B to A
Time 15s: AP A Throughput : 13.6682 Mbps, AP B Throughput : 15.7752 Mbps
Time 16s: AP A Throughput : 13.8749 Mbps, AP B Throughput : 16.2413 Mbps
Time 17s: AP A Throughput : 14.0592 Mbps, AP B Throughput : 16.6554 Mbps
Time 18s: AP A Throughput : 17.0953 Mbps, AP B Throughput : 11.8992 Mbps
Time 19s: AP A Throughput : 16.9274 Mbps, AP B Throughput : 12.0806 Mbps
Time 20s: AP A Throughput : 16.777 Mbps, AP B Throughput : 12.2458 Mbps
Time 21s: AP A Throughput : 16.6424 Mbps, AP B Throughput : 12.3939 Mbps
Time 22s: AP A Throughput : 16.5193 Mbps, AP B Throughput : 12.5257 Mbps
Time 23s: AP A Throughput : 16.4071 Mbps, AP B Throughput : 12.6479 Mbps
Time 24s: AP A Throughput : 16.3053 Mbps, AP B Throughput : 12.7574 Mbps
Time 25s: AP A Throughput : 16.214 Mbps, AP B Throughput : 12.8601 Mbps
Time 26s: AP A Throughput : 16.127 Mbps, AP B Throughput : 12.954 Mbps
Time 27s: AP A Throughput : 16.0477 Mbps, AP B Throughput : 13.0399 Mbps
Time 28s: AP A Throughput : 15.9751 Mbps, AP B Throughput : 13.1201 Mbps
Time 29s: AP A Throughput : 15.9074 Mbps, AP B Throughput : 13.1943 Mbps
10 STAs from move from  A to B; 6 STAs move from B to A 
Time 30s: AP A Throughput : 15.8436 Mbps, AP B Throughput : 13.2638 Mbps
Time 31s: AP A Throughput : 15.8933 Mbps, AP B Throughput : 13.3286 Mbps
Time 32s: AP A Throughput : 15.9389 Mbps, AP B Throughput : 13.3882 Mbps
Time 33s: AP A Throughput : 12.1926 Mbps, AP B Throughput : 16.8893 Mbps
Time 34s: AP A Throughput : 12.2326 Mbps, AP B Throughput : 16.8554 Mbps
Time 35s: AP A Throughput : 12.2705 Mbps, AP B Throughput : 16.8235 Mbps
Time 36s: AP A Throughput : 12.307 Mbps, AP B Throughput : 16.7925 Mbps
Time 37s: AP A Throughput : 12.3391 Mbps, AP B Throughput : 16.7638 Mbps
Time 38s: AP A Throughput : 12.3722 Mbps, AP B Throughput : 16.7368 Mbps
Time 39s: AP A Throughput : 12.4023 Mbps, AP B Throughput : 16.7114 Mbps
Time 40s: AP A Throughput : 12.432 Mbps, AP B Throughput : 16.6876 Mbps
Time 41s: AP A Throughput : 12.4586 Mbps, AP B Throughput : 16.6648 Mbps
Time 42s: AP A Throughput : 12.485 Mbps, AP B Throughput : 16.6437 Mbps
Time 43s: AP A Throughput : 12.5087 Mbps, AP B Throughput : 16.6231 Mbps
Time 44s: AP A Throughput : 12.5328 Mbps, AP B Throughput : 16.603 Mbps
Time 45s: AP A Throughput : 12.5561 Mbps, AP B Throughput : 16.5844 Mbps
Time 46s: AP A Throughput : 12.5774 Mbps, AP B Throughput : 16.5666 Mbps
Time 47s: AP A Throughput : 12.5984 Mbps, AP B Throughput : 16.5498 Mbps
Time 48s: AP A Throughput : 12.618 Mbps, AP B Throughput : 16.5338 Mbps
Time 49s: AP A Throughput : 12.6368 Mbps, AP B Throughput : 16.5183 Mbps
Simulation completed
```
### Analysis of Access Point Throughput Figure

#### Figure Description

The figure shows the throughput performance (in Mbps) of two access points (AP A in blue and AP B in red) over simulation time (in seconds). The simulation runs for 50 seconds and is divided into three distinct phases separated by two handover events at 15 seconds and 30 seconds (marked by vertical dashed lines). The background is shaded in different gray intensities to visually distinguish these phases.

#### Phase-by-Phase Analysis

##### Phase 1 (t=0-15s):
- **Initial Setup**: Both APs start at 0 Mbps at t=1s and rapidly increase to their baseline performance by t=2s.
- **Stable Performance**: From t=2s to t=15s, AP A maintains a steady throughput of ~13.66 Mbps while AP B operates at a higher throughput of ~15.77 Mbps.
- **Average Performance**: AP A averages 12.74 Mbps and AP B averages 14.71 Mbps during this phase (likely lower due to the initial 0 value).

#### Phase 2 (t=15-30s):
- **First Handover Event (15s)**: At this point, there's a dramatic shift in performance.
- **AP A Increase**: AP A's throughput jumps significantly from ~13.67 Mbps to ~17.10 Mbps (a ~25% increase).
- **AP B Decrease**: Simultaneously, AP B's throughput drops sharply from ~15.78 Mbps to ~11.90 Mbps (a ~25% decrease).
- **Gradual Adjustment**: Over this phase, AP A's throughput gradually decreases while AP B's gradually increases.
- **Average Performance**: AP A averages 15.90 Mbps and AP B averages 13.35 Mbps.

#### Phase 3 (t=30-49s):
- **Second Handover Event (30s)**: Another dramatic shift occurs.
- **AP A Decrease**: AP A's throughput drops from ~15.84 Mbps to ~12.19 Mbps (a ~23% decrease).
- **AP B Increase**: AP B's throughput rises from ~13.26 Mbps to ~16.89 Mbps (a ~27% increase).
- **Stabilization**: Both APs stabilize in this new pattern with minimal variation.
- **Average Performance**: AP A averages 12.56 Mbps and AP B averages 16.18 Mbps.

#### Magnitude of Changes

1. **First Handover (15s)**:
   - AP A: +3.43 Mbps (+25.1%)
   - AP B: -3.88 Mbps (-24.6%)
   - Total system throughput remains relatively stable

2. **Second Handover (30s)**:
   - AP A: -3.65 Mbps (-23.0%)
   - AP B: +3.63 Mbps (+27.4%)
   - Total system throughput again remains stable

##### Why These Changes Occur

The pattern strongly suggests a **load-balancing handover mechanism** between access points:

1. The dramatic throughput changes at t=17s and t=32s represent **scheduled handover events** where user equipment (UE) connections shift between access points.

2. The first handover event (15s) represents a scenario where more UEs were transferred from AP B to AP A, causing AP A's throughput to increase and AP B's to decrease.

3. The second handover event (30s) appears to be a reversal, with UEs shifting back from AP A to AP B.

4. The gradual throughput changes between handover events (especially in Phase 2) suggest ongoing network optimization as connections stabilize after handovers.

5. The total network throughput (sum of both APs) remains relatively consistent throughout the simulation (around 29 Mbps), indicating that the handovers are redistributing load rather than adding or removing total capacity.

This simulation effectively demonstrates how handover events in a network can be used to dynamically balance load between access points while maintaining overall network performance.


![截圖 2025-04-11 下午6.24.02](https://hackmd.io/_uploads/BkC66PLRye.png)
## D. Demo Video
https://youtu.be/61KQkyrLbU4?si=97vS30oHLKoymubG
