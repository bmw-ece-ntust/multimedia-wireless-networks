```
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
  double simTime = 60.0;            // 3 minutes (60 seconds * 3)
  double firstMigrationTime = 10.0;  // 1 minute
  double secondMigrationTime = 30.0; // 2 minutes
  
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
