#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/mobility-module.h>
#include <ns3/wifi-module.h>
#include <ns3/internet-module.h>
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/applications-module.h" // Add applications module
#include <iostream>
#include <fstream>
#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("WiFiFullQueueSimulation");

// Helper function to handle user movement
void MoveUsers(NodeContainer& sourceNodes, NodeContainer& destNodes, 
               NetDeviceContainer& sourceDevs, NetDeviceContainer& destDevs,
               Ssid sourceSsid, Ssid destSsid, double sourceToDestPercentage, double destToSourcePercentage,
               Ptr<UniformRandomVariable> rng, std::ofstream& movementLog, 
               std::string sourceLabel, std::string destLabel) {
  
  // Calculate the number of users to move from source to destination
  uint32_t numUsersToMoveFromSource = static_cast<uint32_t>(std::round(sourceNodes.GetN() * sourceToDestPercentage));
  uint32_t numUsersToMoveFromDest = static_cast<uint32_t>(std::round(destNodes.GetN() * destToSourcePercentage));

  // Create candidate indices for source and destination
  std::vector<uint32_t> sourceCandidateIndices, destCandidateIndices;
  for (uint32_t i = 0; i < sourceNodes.GetN(); ++i) {
    sourceCandidateIndices.push_back(i);
  }
  for (uint32_t i = 0; i < destNodes.GetN(); ++i) {
    destCandidateIndices.push_back(i);
  }

  // Randomly select users to move from source to destination
  std::vector<uint32_t> usersToMoveFromSource, usersToMoveFromDest;
  while (usersToMoveFromSource.size() < numUsersToMoveFromSource && !sourceCandidateIndices.empty()) {
    uint32_t randomIndex = rng->GetInteger(0, sourceCandidateIndices.size() - 1);
    uint32_t selectedIndex = sourceCandidateIndices[randomIndex];
    usersToMoveFromSource.push_back(selectedIndex);
    sourceCandidateIndices.erase(sourceCandidateIndices.begin() + randomIndex);
  }
  while (usersToMoveFromDest.size() < numUsersToMoveFromDest && !destCandidateIndices.empty()) {
    uint32_t randomIndex = rng->GetInteger(0, destCandidateIndices.size() - 1);
    uint32_t selectedIndex = destCandidateIndices[randomIndex];
    usersToMoveFromDest.push_back(selectedIndex);
    destCandidateIndices.erase(destCandidateIndices.begin() + randomIndex);
  }

  // Create new containers for remaining nodes and devices
  NodeContainer newSourceNodes, newDestNodes;
  NetDeviceContainer newSourceDevs, newDestDevs;

  // Move users from source to destination
  for (uint32_t i = 0; i < sourceNodes.GetN(); ++i) {
    if (std::find(usersToMoveFromSource.begin(), usersToMoveFromSource.end(), i) != usersToMoveFromSource.end()) {
      Ptr<StaWifiMac> mac = DynamicCast<StaWifiMac>(sourceDevs.Get(i)->GetObject<WifiNetDevice>()->GetMac());
      if (mac) {
        mac->SetSsid(destSsid);
        Ptr<NetDevice> device = sourceDevs.Get(i);
        Mac48Address macAddr = Mac48Address::ConvertFrom(device->GetAddress());
        movementLog << "Time: " << Simulator::Now().GetSeconds() 
                   << "s - User " << sourceLabel << i 
                   << " (MAC: " << macAddr << ") moved from " 
                   << sourceLabel << " to " << destLabel << "\n";
        destNodes.Add(sourceNodes.Get(i));
        destDevs.Add(sourceDevs.Get(i));
      }
    } else {
      newSourceNodes.Add(sourceNodes.Get(i));
      newSourceDevs.Add(sourceDevs.Get(i));
    }
  }

  // Move users from destination to source
  for (uint32_t i = 0; i < destNodes.GetN(); ++i) {
    if (std::find(usersToMoveFromDest.begin(), usersToMoveFromDest.end(), i) != usersToMoveFromDest.end()) {
      Ptr<StaWifiMac> mac = DynamicCast<StaWifiMac>(destDevs.Get(i)->GetObject<WifiNetDevice>()->GetMac());
      if (mac) {
        mac->SetSsid(sourceSsid);
        Ptr<NetDevice> device = destDevs.Get(i);
        Mac48Address macAddr = Mac48Address::ConvertFrom(device->GetAddress());
        movementLog << "Time: " << Simulator::Now().GetSeconds() 
                   << "s - User " << destLabel << i 
                   << " (MAC: " << macAddr << ") moved from " 
                   << destLabel << " to " << sourceLabel << "\n";
        newSourceNodes.Add(destNodes.Get(i));
        newSourceDevs.Add(destDevs.Get(i));
      }
    } else {
      newDestNodes.Add(destNodes.Get(i));
      newDestDevs.Add(destDevs.Get(i));
    }
  }

  // Replace the source and destination containers with the new ones
  sourceNodes = newSourceNodes;
  sourceDevs = newSourceDevs;
  destNodes = newDestNodes;
  destDevs = newDestDevs;
}

// Add throughput logging and user distribution logging
void LogThroughputAndUsers(std::ofstream& throughputLog, std::ofstream& userDistributionLog, 
                           NodeContainer& apNodes, NetDeviceContainer& staDevsA, NetDeviceContainer& staDevsB) {
  // Log throughput for Node A and Node B
  for (uint32_t i = 0; i < apNodes.GetN(); ++i) {
    Ptr<PacketSink> sink = DynamicCast<PacketSink>(apNodes.Get(i)->GetApplication(0));
    if (sink) {
      double throughput = sink->GetTotalRx() * 8.0 / Simulator::Now().GetSeconds() / 1000000; // Mbps
      throughputLog << Simulator::Now().GetSeconds() << ",Node-" << (i == 0 ? "A" : "B") << "," << throughput << "\n";
    }
  }

  // Log user distribution
  userDistributionLog << "Time: " << Simulator::Now().GetSeconds() << "s\n";
  userDistributionLog << "Node A Users:\n";
  for (uint32_t i = 0; i < staDevsA.GetN(); ++i) {
    Ptr<NetDevice> device = staDevsA.Get(i);
    Mac48Address mac = Mac48Address::ConvertFrom(device->GetAddress());
    userDistributionLog << "STA-A" << i << " (MAC: " << mac << ")\n";
  }
  userDistributionLog << "Node B Users:\n";
  for (uint32_t i = 0; i < staDevsB.GetN(); ++i) {
    Ptr<NetDevice> device = staDevsB.Get(i);
    Mac48Address mac = Mac48Address::ConvertFrom(device->GetAddress());
    userDistributionLog << "STA-B" << i << " (MAC: " << mac << ")\n";
  }
  userDistributionLog << "================\n";
}

// Function to log user connections every second
void LogUserConnections(std::ofstream& logFile, NodeContainer& staNodesA, NodeContainer& staNodesB, 
                        NetDeviceContainer& staDevsA, NetDeviceContainer& staDevsB) {
  logFile << "Time: " << Simulator::Now().GetSeconds() << "s\n";
  logFile << "================\n";

  logFile << "Node A Users:\n";
  for (uint32_t i = 0; i < staDevsA.GetN(); ++i) {
    Ptr<NetDevice> device = staDevsA.Get(i);
    Mac48Address mac = Mac48Address::ConvertFrom(device->GetAddress());
    logFile << "STA-A" << i << " (MAC: " << mac << ")\n";
  }

  logFile << "Node B Users:\n";
  for (uint32_t i = 0; i < staDevsB.GetN(); ++i) {
    Ptr<NetDevice> device = staDevsB.Get(i);
    Mac48Address mac = Mac48Address::ConvertFrom(device->GetAddress());
    logFile << "STA-B" << i << " (MAC: " << mac << ")\n";
  }

  logFile << "================\n\n";
}

// Add a new function to log AP user counts
void LogUserCounts(NodeContainer& staNodesA, NodeContainer& staNodesB) {
  std::cout << "Time: " << Simulator::Now().GetSeconds() << "s - "
            << "AP-A has " << staNodesA.GetN() << " users, "
            << "AP-B has " << staNodesB.GetN() << " users." << std::endl;
}

int main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse(argc, argv);

  // Create a random variable generator for movement selection
  Ptr<UniformRandomVariable> rng = CreateObject<UniformRandomVariable>();

  // Declare unifiedLog as a local variable inside the main function
  std::ofstream unifiedLog;
  unifiedLog.open("user_log.txt");
  unifiedLog << "Time\tUE ID\tConnected AP\n";

  // Set up WiFi
  WifiHelper wifi;
  wifi.SetStandard(WIFI_STANDARD_80211ac);
  wifi.SetRemoteStationManager("ns3::MinstrelHtWifiManager");

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
  YansWifiPhyHelper phy;
  phy.SetChannel(channel.Create());

  // Configure UDP client
  Config::SetDefault ("ns3::UdpClient::Interval", TimeValue (MilliSeconds (1000)));
  Config::SetDefault ("ns3::UdpClient::MaxPackets", UintegerValue (1000000));

  // Create Node objects for the APs (formerly eNBs)
  NodeContainer apNodes;
  apNodes.Create (2); // 2 APs (Node A and Node B)

  // Create Node objects for stations (formerly UEs) - 16 stations per AP, total 32 stations
  NodeContainer staNodesA; // Stations for Node A
  staNodesA.Create (16);
  
  NodeContainer staNodesB; // Stations for Node B
  staNodesB.Create (16);

  //Configure the Mobility model for all the nodes
  MobilityHelper mobility;
  
  // Set Mobility for APs
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  Ptr<ListPositionAllocator> apPositionAlloc = CreateObject<ListPositionAllocator> ();
  // Node A at position (0,0,0)
  apPositionAlloc->Add (Vector (0.0, 0.0, 0.0));
  // Node B at position (500,0,0) - 500m apart on x-axis
  apPositionAlloc->Add (Vector (500.0, 0.0, 0.0));
  
  mobility.SetPositionAllocator (apPositionAlloc);
  mobility.Install (apNodes);

  // Set Mobility for stations of Node A
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
    "MinX", DoubleValue (-100.0),
    "MinY", DoubleValue (-100.0),
    "DeltaX", DoubleValue (50.0),
    "DeltaY", DoubleValue (50.0),
    "GridWidth", UintegerValue (4),
    "LayoutType", StringValue ("RowFirst"));
  
  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
    "Mode", StringValue ("Time"),
    "Time", StringValue ("2s"),
    "Bounds", RectangleValue (Rectangle (-200, 200, -200, 200)));
  
  mobility.Install (staNodesA);

  // Set Mobility for stations of Node B
  Ptr<ListPositionAllocator> staPositionAllocB = CreateObject<ListPositionAllocator> ();
  for (uint32_t i = 0; i < 16; ++i) {
    double x = 500.0 + ((i % 4) * 50.0) - 100.0; // Centered around Node B (500,0,0)
    double y = ((i / 4) * 50.0) - 100.0;
    staPositionAllocB->Add (Vector (x, y, 0.0));
  }
  
  mobility.SetPositionAllocator (staPositionAllocB);
  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
    "Mode", StringValue ("Time"),
    "Time", StringValue ("2s"),
    "Bounds", RectangleValue (Rectangle (300, 700, -200, 200))); // Bounds around Node B
  
  mobility.Install (staNodesB);

  // Install WiFi devices (replacing LTE devices)
  WifiMacHelper macAp;
  Ssid ssid1 = Ssid ("network-A");
  Ssid ssid2 = Ssid ("network-B");
  
  // AP A configuration
  macAp.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid1),
               "EnableBeaconJitter", BooleanValue (false));
  NetDeviceContainer apDevsA = wifi.Install (phy, macAp, apNodes.Get(0));
  
  // AP B configuration
  macAp.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid2),
               "EnableBeaconJitter", BooleanValue (false));
  NetDeviceContainer apDevsB = wifi.Install (phy, macAp, apNodes.Get(1));
  
  // Configure stations for AP A
  WifiMacHelper macSta;
  macSta.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid1),
               "ActiveProbing", BooleanValue (false));
  NetDeviceContainer staDevsA = wifi.Install (phy, macSta, staNodesA);
  
  // Configure stations for AP B
  macSta.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid2),
               "ActiveProbing", BooleanValue (false));
  NetDeviceContainer staDevsB = wifi.Install (phy, macSta, staNodesB);

  // Install internet stack
  InternetStackHelper internet;
  internet.Install (apNodes);
  internet.Install (staNodesA);
  internet.Install (staNodesB);

  // Assign IP addresses
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfacesA = ipv4.Assign (staDevsA);
  Ipv4InterfaceContainer apInterfaceA = ipv4.Assign (apDevsA);
  
  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer interfacesB = ipv4.Assign (staDevsB);
  Ipv4InterfaceContainer apInterfaceB = ipv4.Assign (apDevsB);

  // Setup applications for full-queue model (continuous traffic)
  uint16_t port = 9;
  
  // Create a sink (receiver) application on both APs
  PacketSinkHelper packetSinkHelper("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
  ApplicationContainer sinkApps;
  sinkApps.Add(packetSinkHelper.Install(apNodes.Get(0))); // AP A
  sinkApps.Add(packetSinkHelper.Install(apNodes.Get(1))); // AP B
  sinkApps.Start(Seconds(1.0));
  sinkApps.Stop(Seconds(30.0)); // Changed to 30s for the scenario
  
  // Create OnOff Application for stations connected to AP A
  OnOffHelper onOffHelperA("ns3::UdpSocketFactory", InetSocketAddress(apInterfaceA.GetAddress(0), port));
  onOffHelperA.SetConstantRate(DataRate("20Mb/s")); // High data rate for full queue
  onOffHelperA.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"));
  onOffHelperA.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0.0]"));
  
  ApplicationContainer clientAppsA;
  for (uint32_t i = 0; i < staNodesA.GetN(); ++i) {
    clientAppsA.Add(onOffHelperA.Install(staNodesA.Get(i)));
  }
  clientAppsA.Start(Seconds(2.0));
  clientAppsA.Stop(Seconds(30.0)); // Changed to 30s for the scenario
  
  // Create OnOff Application for stations connected to AP B
  OnOffHelper onOffHelperB("ns3::UdpSocketFactory", InetSocketAddress(apInterfaceB.GetAddress(0), port));
  onOffHelperB.SetConstantRate(DataRate("20Mb/s")); // High data rate for full queue
  onOffHelperB.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"));
  onOffHelperB.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0.0]"));
  
  ApplicationContainer clientAppsB;
  for (uint32_t i = 0; i < staNodesB.GetN(); ++i) {
    clientAppsB.Add(onOffHelperB.Install(staNodesB.Get(i)));
  }
  clientAppsB.Start(Seconds(2.0));
  clientAppsB.Stop(Seconds(30.0)); // Changed to 30s for the scenario

  // Generate user table
  std::ofstream userTable;
  userTable.open ("user_table.txt");
  userTable << "Node ID\tSTA ID\tMAC Address\n";
  
  // Log Node A's stations
  for (uint32_t i = 0; i < staDevsA.GetN(); i++) {
    Ptr<NetDevice> device = staDevsA.Get(i);
    Mac48Address mac = Mac48Address::ConvertFrom(device->GetAddress());
    userTable << "Node A\t" << (i + 1) << "\t" << mac << "\n";
  }
  
  // Log Node B's stations
  for (uint32_t i = 0; i < staDevsB.GetN(); i++) {
    Ptr<NetDevice> device = staDevsB.Get(i);
    Mac48Address mac = Mac48Address::ConvertFrom(device->GetAddress());
    userTable << "Node B\t" << (i + 17) << "\t" << mac << "\n";
  }
  
  userTable.close();

  // Create movement log file
  std::ofstream movementLog;
  movementLog.open("user_movement_log.txt");
  movementLog << "User Movement Log\n";
  movementLog << "================\n\n";

  // Create throughput log file
  std::ofstream throughputLog;
  throughputLog.open("throughput_log.txt");
  throughputLog << "Time,Node,Throughput (Mbps)\n";

  // Create user distribution log file
  std::ofstream userDistributionLog;
  userDistributionLog.open("user_distribution_log.txt");

  // First movement at 10s - updated percentages to match scenario
  // Move 25% from AP-A to AP-B and 50% from AP-B to AP-A
  Simulator::Schedule(Seconds(10), &MoveUsers, 
                     std::ref(staNodesA), std::ref(staNodesB),
                     std::ref(staDevsA), std::ref(staDevsB), 
                     ssid1, ssid2, 0.25, 0.50, rng, std::ref(movementLog), "STA-A", "STA-B");
  
  // Log after first movement
  Simulator::Schedule(Seconds(10.1), &LogUserCounts, std::ref(staNodesA), std::ref(staNodesB));
  
  // Print explanation of expected movement results
  Simulator::Schedule(Seconds(10.2), []() {
    std::cout << "\n*** Expected after first movement at 10s ***" << std::endl;
    std::cout << "AP-A should have ~20 users (16 - 25% + 50% of AP-B's 16)" << std::endl;
    std::cout << "AP-B should have ~12 users (16 - 50% + 25% of AP-A's 16)" << std::endl;
  });

  // CORRECTED: Second movement at 20s to restore exact balance of 16 users per AP
  // Using more aggressive ratios based on actual user counts after first movement
  Simulator::Schedule(Seconds(20), [&staNodesA, &staNodesB, &staDevsA, &staDevsB, ssid1, ssid2, &rng, &movementLog]() {
    // Calculate exact percentages needed to get 16 users at each AP
    uint32_t usersAtA = staNodesA.GetN();
    uint32_t usersAtB = staNodesB.GetN();
    
    // Calculate how many users to move
    int moveFromAtoB = std::max(0, (int)usersAtA - 16); // Move excess from A to B
    int moveFromBtoA = std::max(0, 16 - (int)usersAtA); // Move shortage from B to A
    
    // Calculate percentages
    double percentFromA = (usersAtA > 0) ? (double)moveFromAtoB / usersAtA : 0;
    double percentFromB = (usersAtB > 0) ? (double)moveFromBtoA / usersAtB : 0;
    
    std::cout << "Moving " << moveFromAtoB << " users from A to B and " 
              << moveFromBtoA << " users from B to A to achieve balance" << std::endl;
              
    // Call MoveUsers with calculated percentages
    MoveUsers(staNodesA, staNodesB, staDevsA, staDevsB, ssid1, ssid2, 
             percentFromA, percentFromB, rng, movementLog, "STA-A", "STA-B");
  });
  
  // Log after second movement
  Simulator::Schedule(Seconds(20.1), &LogUserCounts, std::ref(staNodesA), std::ref(staNodesB));
  
  // Print explanation of expected movement results
  Simulator::Schedule(Seconds(20.2), []() {
    std::cout << "\n*** Expected after second movement at 20s ***" << std::endl;
    std::cout << "AP-A and AP-B should each have exactly 16 users" << std::endl;
    std::cout << "Using calculated percentages to ensure proper balance" << std::endl;
  });

  // Add debugging to track user counts specifically at 20.5s
  Simulator::Schedule(Seconds(20.5), [&staNodesA, &staNodesB]() {
    std::cout << "\n*** Verifying user counts after second movement ***" << std::endl;
    std::cout << "AP-A current user count: " << staNodesA.GetN() << std::endl;
    std::cout << "AP-B current user count: " << staNodesB.GetN() << std::endl;
    std::cout << "Total users in system: " << (staNodesA.GetN() + staNodesB.GetN()) << std::endl;
    
    // Print result assessment
    if (staNodesA.GetN() == 16 && staNodesB.GetN() == 16) {
      std::cout << "✓ PERFECT BALANCE ACHIEVED!" << std::endl;
    } else {
      std::cout << "Balance not quite achieved. Adjusting percentages may be needed." << std::endl;
    }
  });

  // Schedule periodic logging of throughput and user distribution - adjust to 30s
  for (double t = 1.0; t <= 30.0; t += 1.0) {
    Simulator::Schedule(Seconds(t), &LogThroughputAndUsers, 
                        std::ref(throughputLog), std::ref(userDistributionLog), 
                        std::ref(apNodes), std::ref(staDevsA), std::ref(staDevsB));

    // Log user counts every second
    Simulator::Schedule(Seconds(t), &LogUserCounts, std::ref(staNodesA), std::ref(staNodesB));
  }

  // Schedule periodic logging of user connections - adjust to 30s
  for (double t = 1.0; t <= 30.0; t += 1.0) {
    Simulator::Schedule(Seconds(t), &LogUserConnections, 
                        std::ref(unifiedLog), std::ref(staNodesA), std::ref(staNodesB), 
                        std::ref(staDevsA), std::ref(staDevsB));
  }

  // Close the unified log file at the end of the simulation
  Simulator::Schedule(Seconds(30.1), [&unifiedLog]() {
    unifiedLog.close();
  });

  // Enable WiFi traces
  AsciiTraceHelper ascii;
  phy.EnableAsciiAll (ascii.CreateFileStream ("wifi-tracing.tr"));
  phy.EnablePcap ("wifi-pcap", apNodes);

  // Set simulation duration to 30 seconds
  Simulator::Stop (Seconds (30));
  
  // Add initial user count logging
  Simulator::Schedule(Seconds(1.0), &LogUserCounts, std::ref(staNodesA), std::ref(staNodesB));
  
  // Print the scenario timeline
  std::cout << "\n*** Simulation Scenario ***" << std::endl;
  std::cout << "1-10s: AP-A and AP-B each have 16 users" << std::endl;
  std::cout << "10-20s: Movement at 10s - AP-A should have 20 users, AP-B should have 12 users" << std::endl;
  std::cout << "20-30s: Movement at 20s - AP-A and AP-B each return to 16 users" << std::endl;

  // Set up animation
  AnimationInterface anim ("wifi_32sta_2ap.xml");
  
  // Set node representation for APs - use red color
  for (uint32_t i = 0; i < apNodes.GetN(); i++) {
    uint32_t nodeId = apNodes.Get(i)->GetId();
    anim.UpdateNodeColor (nodeId, 255, 0, 0); // Red color for APs
    anim.UpdateNodeSize (nodeId, 10, 10); // Small circle
    anim.UpdateNodeDescription (nodeId, "AP-" + std::to_string(i));
  }
  
  // Set node representation for stations of Node A - use blue color
  for (uint32_t i = 0; i < staNodesA.GetN(); i++) {
    uint32_t nodeId = staNodesA.Get(i)->GetId();
    anim.UpdateNodeColor (nodeId, 0, 0, 255); // Blue color for stations of AP A
    anim.UpdateNodeSize (nodeId, 5, 5); // Smaller circle
    anim.UpdateNodeDescription (nodeId, "STA-A" + std::to_string(i));
  }
  
  // Set node representation for stations of Node B - use green color
  for (uint32_t i = 0; i < staNodesB.GetN(); i++) {
    uint32_t nodeId = staNodesB.Get(i)->GetId();
    anim.UpdateNodeColor (nodeId, 0, 255, 0); // Green color for stations of AP B
    anim.UpdateNodeSize (nodeId, 5, 5); // Smaller circle
    anim.UpdateNodeDescription (nodeId, "STA-B" + std::to_string(i));
  }
  
  // Setup animation settings
  anim.SetMobilityPollInterval(Seconds(1.00));
  anim.SetMaxPktsPerTraceFile (100000000000);
  
  // Add Flow Monitor
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();
  
  // Run the simulation
  Simulator::Run ();
  
  // Print flow statistics
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
  
  std::cout << std::endl << "*** Flow monitor statistics ***" << std::endl;
  std::cout << "  Tx Packets:   " << stats[1].txPackets << std::endl;
  std::cout << "  Rx Packets:   " << stats[1].rxPackets << std::endl;
  std::cout << "  Throughput:   " << stats[1].rxBytes * 8.0 / (stats[1].timeLastRxPacket.GetSeconds() - stats[1].timeFirstTxPacket.GetSeconds()) / 1000000 << " Mbps" << std::endl;
  
  // Add final user count summary
  std::cout << std::endl << "*** Final User Distribution ***" << std::endl;
  std::cout << "AP-A final user count: " << staNodesA.GetN() << std::endl;
  std::cout << "AP-B final user count: " << staNodesB.GetN() << std::endl;
  std::cout << "Total users in system: " << (staNodesA.GetN() + staNodesB.GetN()) << std::endl;
  
  // Close the movement log file
  movementLog.close();
  
  // Close the throughput log file
  throughputLog.close();

  // Close the user distribution log file
  userDistributionLog.close();
  
  Simulator::Destroy ();
  
  std::cout << "Simulation completed. User movement log has been generated in user_movement_log.txt" << std::endl;
  
  return 0;
}
