// M11351802 - Herranz Gancedo, Lucas.
// The following topology is built in the script:
// 200m x 300m square area.
// 2 APs at the 'x' locations
// 16 STAs randomly positioned within a 40m radius of each STA
// 60 seconds simulation time.
// At second 20, 
//    25% of STAs associated with AP1 move from AP1 to AP2
//    50% of STAs associated with AP2 move from AP2 to AP1
// At second 40,
//    25% of STAs associated with AP1 move from AP1 to AP2
//    50% of STAs associated with AP2 move from AP2 to AP1
//
//      (0,0)                   (300,0)
//        +-------------------------+
//        |                         |
//        |  (50,100)               |
//        |     x     +      x      | 
//        |              (250,100)  |
//        |                         |
//        +-------------------------+
//      (0,200)                 (300,200)
//
// Implement DCF (Distributed Coordination Function) for the APs and STAs and
// a FCFS (First Come First Serve) queue for the STAs.

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include <iostream>
#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("WifiSimulationExample");

void ScheduleStaMobilityEvents(NodeContainer staNodes, uint32_t nWifi,
                              uint32_t movingFromAp1, uint32_t movingFromAp2,
                              uint32_t ap1Offset = 0, uint32_t ap2Offset = 0)
{
  NS_LOG_INFO("Scheduling STA mobility at " << Simulator::Now().GetSeconds() << "s");

  if (staNodes.GetN() < 2 * nWifi)
  {
    NS_LOG_INFO("Not enough STA nodes for movement.");
    return;
  }

  // Move from AP1 to AP2
  for (uint32_t i = ap1Offset; i < ap1Offset + movingFromAp1; ++i)
  {
    Ptr<MobilityModel> mobility = staNodes.Get(i)->GetObject<MobilityModel>();
    Vector target = Vector(250.0 + rand() % 20 - 10, 100.0 + rand() % 20 - 10, 0.0);
    mobility->SetPosition(target);
    NS_LOG_INFO("→ STA node " << i << " (AP1 → AP2) scheduled to move at " << Simulator::Now().GetSeconds() << "s");
  }

  // Move from AP2 to AP1
  for (uint32_t i = nWifi + ap2Offset; i < nWifi + ap2Offset + movingFromAp2; ++i)
  {
    Ptr<MobilityModel> mobility = staNodes.Get(i)->GetObject<MobilityModel>();
    Vector target = Vector(50.0 + rand() % 20 - 10, 100.0 + rand() % 20 - 10, 0.0);
    mobility->SetPosition(target);
    NS_LOG_INFO("→ STA node " << i << " (AP2 → AP1) scheduled to move at " << Simulator::Now().GetSeconds() << "s");
  }
}


int main(int argc, char *argv[])
{
  // Simulation parameters
  double simulationTime = 60.0; // seconds
  uint32_t nWifi = 16; // STAs per AP
  bool verbose = false;
  
  // Set up command line parameters
  CommandLine cmd;
  cmd.AddValue("simulationTime", "Total simulation time in seconds", simulationTime);
  cmd.AddValue("verbose", "Enable verbose output", verbose);
  cmd.Parse(argc, argv);


  LogComponentEnable("WifiSimulationExample", LOG_LEVEL_INFO);

  // Create nodes for APs and STAs
  NodeContainer apNodes;
  apNodes.Create(2);

  NodeContainer staNodes;
  staNodes.Create(nWifi * 2); // 2 APs, each with nWifi STAs

  // Configure mobility for APs (fixed positions)
  MobilityHelper mobilityAP;
  Ptr<ListPositionAllocator> positionAllocAP = CreateObject<ListPositionAllocator>();
  positionAllocAP->Add(Vector(50.0, 100.0, 0.0));  // AP1 position
  positionAllocAP->Add(Vector(250.0, 100.0, 0.0)); // AP2 position
  mobilityAP.SetPositionAllocator(positionAllocAP);
  mobilityAP.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobilityAP.Install(apNodes);
  
  // Configure initial mobility for STAs around AP1
  MobilityHelper mobilitySTA1;
  mobilitySTA1.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
                                    "X", DoubleValue(50.0),
                                    "Y", DoubleValue(100.0),
                                    "Rho", StringValue("ns3::UniformRandomVariable[Min=0|Max=40]"));
  mobilitySTA1.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  for (uint32_t i = 0; i < nWifi; ++i) {
    mobilitySTA1.Install(staNodes.Get(i));
  }
  
  // Configure initial mobility for STAs around AP2
  MobilityHelper mobilitySTA2;
  mobilitySTA2.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
                                    "X", DoubleValue(250.0),
                                    "Y", DoubleValue(100.0),
                                    "Rho", StringValue("ns3::UniformRandomVariable[Min=0|Max=40]"));
  mobilitySTA2.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  for (uint32_t i = nWifi; i < nWifi * 2; ++i) {
    mobilitySTA2.Install(staNodes.Get(i));
  }

  // Create and configure WiFi network
  WifiHelper wifi;
  wifi.SetStandard(WIFI_STANDARD_80211g);

  // Use DCF for medium access control
  WifiMacHelper wifiMac;
  Ssid ssid = Ssid("WIFI-Network");

  // Configure the physical layer
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();
  YansWifiPhyHelper wifiPhy;
  wifiPhy.SetChannel(wifiChannel.Create());

  // Configure and install APs
  wifiMac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
  NetDeviceContainer apDevices = wifi.Install(wifiPhy, wifiMac, apNodes);
  wifiPhy.EnablePcap("wifi-ap1", apDevices.Get(0));
  wifiPhy.EnablePcap("wifi-ap2", apDevices.Get(1));

  // Configure and install STAs
  wifiMac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));
  NetDeviceContainer staDevices = wifi.Install(wifiPhy, wifiMac, staNodes);

  // Install the internet stack
  InternetStackHelper internet;
  internet.Install(apNodes);
  internet.Install(staNodes);

  // Assign IP addresses
  Ipv4AddressHelper address;
  
  // Network addresses
  address.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer apInterfaces = address.Assign(apDevices);
  Ipv4InterfaceContainer staInterfaces = address.Assign(staDevices);
  
  // Set up UDP PacketSink on AP1
  PacketSinkHelper sinkHelper1("ns3::UdpSocketFactory", InetSocketAddress(apInterfaces.GetAddress(0), 9));
  ApplicationContainer sinkApp1 = sinkHelper1.Install(apNodes.Get(0));
  sinkApp1.Start(Seconds(1.0));
  sinkApp1.Stop(Seconds(simulationTime-1.0));

  // Set up UDP PacketSink on AP2
  PacketSinkHelper sinkHelper2("ns3::UdpSocketFactory", InetSocketAddress(apInterfaces.GetAddress(1), 9));
  ApplicationContainer sinkApp2 = sinkHelper2.Install(apNodes.Get(1));
  sinkApp2.Start(Seconds(1.0));
  sinkApp2.Stop(Seconds(simulationTime-1.0));

  // Set up UDP client on STAs
  UdpClientHelper client1(apInterfaces.GetAddress(0), 9);
  client1.SetAttribute("MaxPackets", UintegerValue(4294967295U));
  client1.SetAttribute("Interval", TimeValue(Time("0.2")));  // 5 packets per second
  client1.SetAttribute("PacketSize", UintegerValue(1472));      // Bytes

  UdpClientHelper client2(apInterfaces.GetAddress(1), 9);
  client2.SetAttribute("MaxPackets", UintegerValue(4294967295U));
  client2.SetAttribute("Interval", TimeValue(Time("0.2")));  // 5 packets per second
  client2.SetAttribute("PacketSize", UintegerValue(1472));      // Bytes

  ApplicationContainer clientApp1, clientApp2;
  for (uint32_t i = 0; i < 2*nWifi; ++i) {
    clientApp1.Add(client1.Install(staNodes.Get(i)));
    clientApp2.Add(client2.Install(staNodes.Get(i)));
  }

  clientApp1.Start(Seconds(2.0));
  clientApp1.Stop(Seconds(simulationTime-2.0)); 
  clientApp2.Start(Seconds(2.0));
  clientApp2.Stop(Seconds(simulationTime-2.0));

  NS_LOG_INFO("================= Lucas Herranz Gancedo (M11351802) ===============\n");
  NS_LOG_INFO("Starting simulation with " << nWifi * 2 << " STAs and " << simulationTime << " seconds.\n");
  NS_LOG_INFO("AP1 and AP2 are at (50,100) and (250,100) respectively.\n");
  NS_LOG_INFO("STAs are randomly positioned within a 40m radius of each AP.\n");
  NS_LOG_INFO("=======================================================================\n");

  // Initial wave at 20s
  uint32_t move1_20 = nWifi * 0.25;
  uint32_t move2_20 = nWifi * 0.5;
  Simulator::Schedule(Seconds(20.0), &ScheduleStaMobilityEvents, staNodes, nWifi, move1_20, move2_20, 0, 0);

  // Second wave at 40s (excluding previously moved STAs)
  uint32_t move1_40 = (nWifi - move1_20 + move2_20) * 0.25;
  uint32_t move2_40 = (nWifi - move2_20 + move1_20) * 0.5;
  Simulator::Schedule(Seconds(40.0), &ScheduleStaMobilityEvents, staNodes, nWifi, move1_40, move2_40, move1_20, move2_20);
  
  Ptr<PacketSink> sink1Ptr = DynamicCast<PacketSink>(sinkApp1.Get(0));
  Ptr<PacketSink> sink2Ptr = DynamicCast<PacketSink>(sinkApp2.Get(0));

  // Run simulation
  Simulator::Stop(Seconds(simulationTime));
  Simulator::Run();

  std::cout << "AP1 received: " << sink1Ptr->GetTotalRx() << " bytes\n";
  std::cout << "AP2 received: " << sink2Ptr->GetTotalRx() << " bytes\n";

  // Cleanup
  Simulator::Destroy();
  
  return 0;
}