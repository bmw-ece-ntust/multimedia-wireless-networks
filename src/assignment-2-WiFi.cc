#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/ssid.h"
#include <fstream>

using namespace ns3;

// Define the logging component
NS_LOG_COMPONENT_DEFINE("WifiSimulation");

uint64_t ap1Bytes = 0;
uint64_t ap2Bytes = 0;

// Callbacks to track AP1 received bytes
void RxCallbackAp1(Ptr<const Packet> packet, const Address &addr) {
  ap1Bytes += packet->GetSize();
}

// Callback to track AP2 received bytes
void RxCallbackAp2(Ptr<const Packet> packet, const Address &addr) {
  ap2Bytes += packet->GetSize();
}

// Function to log throughput
void LogThroughput() {
  static std::ofstream outFile("throughput.csv", std::ios::app);
  static uint32_t time = 1;

  outFile << time << "," << ap1Bytes << "," << ap2Bytes << std::endl;
  NS_LOG_INFO("Time: " << time << "s, AP1 Bytes: " << ap1Bytes << ", AP2 Bytes: " << ap2Bytes);

  time++;
  Simulator::Schedule(Seconds(1.0), &LogThroughput);
}

// Function to install full queue traffic
void InstallFullQueueTraffic(Ptr<Node> sender, Address sinkAddress, double startTime, double stopTime) {
  OnOffHelper onoff ("ns3::UdpSocketFactory", sinkAddress);
  onoff.SetConstantRate (DataRate ("1Mbps"));  // High data rate to saturate the channel
  onoff.SetAttribute ("PacketSize", UintegerValue (1472));  // Max packet size (Wi-Fi MTU)
  onoff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1000]")); // Always on
  onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]")); // No off time

  ApplicationContainer app = onoff.Install(sender);
  app.Start (Seconds (startTime));
  app.Stop (Seconds (stopTime));
}

// Function to save MAC addresses to a CSV file
void SaveMacAddresses(NodeContainer &staNodesA, NodeContainer &staNodesB) {
  std::ofstream macFile("sta_mac_addresses.csv");
  macFile << "STA,MAC Address" << std::endl;

  for (uint32_t i = 0; i < staNodesA.GetN(); ++i) {
    Ptr<NetDevice> device = staNodesA.Get(i)->GetDevice(0);
    Mac48Address mac = Mac48Address::ConvertFrom(device->GetAddress());
    macFile << "STA-A-" << i+1 << "," << mac << std::endl;
  }

  for (uint32_t i = 0; i < staNodesB.GetN(); ++i) {
    Ptr<NetDevice> device = staNodesB.Get(i)->GetDevice(0);
    Mac48Address mac = Mac48Address::ConvertFrom(device->GetAddress());
    macFile << "STA-B-" << i+1 << "," << mac << std::endl;
  }

  macFile.close();
  NS_LOG_INFO("MAC addresses saved to sta_mac_addresses.csv");
}

int main() {
  // Set default values for the simulation
  uint32_t nStaPerAp = 16; //number of stations per AP
  double simTime = 60.0;  // Simulation time in seconds
  LogComponentEnable("WifiSimulation", LOG_LEVEL_INFO);

  // Create nodes
  NS_LOG_INFO("Create Nodes...");
  NodeContainer wifiStaNodesA, wifiStaNodesB, wifiApNodes;
  wifiStaNodesA.Create(nStaPerAp);  
  wifiStaNodesB.Create(nStaPerAp);  
  wifiApNodes.Create(2);
  
  // Save MAC addresses to a CSV file
  SaveMacAddresses(wifiStaNodesA, wifiStaNodesB);
  // Set up the Wi-Fi channel
  NS_LOG_INFO("Create Wifi Channel...");
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
  YansWifiPhyHelper phy;
  phy.SetChannel(channel.Create());

  // Set up the Wi-Fi PHY layer
  WifiHelper wifi;
  wifi.SetStandard(WIFI_STANDARD_80211n);
  wifi.SetRemoteStationManager("ns3::MinstrelHtWifiManager");

  // Set up the Wi-Fi Mac Layer and SSID
  WifiMacHelper mac;
  Ssid ssidA = Ssid("network-A");
  Ssid ssidB = Ssid("network-B");

  mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssidA), "ActiveProbing", BooleanValue(false));
  NetDeviceContainer staDevicesA = wifi.Install(phy, mac, wifiStaNodesA);

  mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssidB), "ActiveProbing", BooleanValue(false));
  NetDeviceContainer staDevicesB = wifi.Install(phy, mac, wifiStaNodesB);

  mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssidA));
  NetDeviceContainer apDeviceA = wifi.Install(phy, mac, wifiApNodes.Get(0));

  mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssidB));
  NetDeviceContainer apDeviceB = wifi.Install(phy, mac, wifiApNodes.Get(1));

  // Set up the mobility model
  NS_LOG_INFO("Set Mobility...");
  MobilityHelper mobility;
  mobility.SetPositionAllocator("ns3::GridPositionAllocator", "MinX", DoubleValue(25.0), "MinY", DoubleValue(0.0),
                                "DeltaX", DoubleValue(0.0), "DeltaY", DoubleValue(0.0), "GridWidth", UintegerValue(1),
                                "LayoutType", StringValue("RowFirst"));
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

  mobility.Install(wifiStaNodesA);
  mobility.Install(wifiStaNodesB);
  mobility.Install(wifiApNodes);

  // Set the position of the APs
  NS_LOG_INFO("Install Internet Stack...");
  InternetStackHelper stack;
  stack.Install(wifiApNodes);
  stack.Install(wifiStaNodesA);
  stack.Install(wifiStaNodesB);

  // Set the IP addresses of the APs and STAs
  Ipv4AddressHelper address;

  address.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer apInterfaceA = address.Assign(apDeviceA);
  address.Assign(staDevicesA);

  address.SetBase("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer apInterfaceB = address.Assign(apDeviceB);
  address.Assign(staDevicesB);

  NS_LOG_INFO("Install Applications...");

  uint16_t port = 9;

  // Install packet sink on AP A
  PacketSinkHelper sinkA("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
  ApplicationContainer sinkAppA = sinkA.Install(wifiApNodes.Get(0));
  sinkAppA.Start(Seconds(0.0));
  sinkAppA.Stop(Seconds(simTime));

  sinkAppA.Get(0)->GetObject<PacketSink>()->TraceConnectWithoutContext("Rx", MakeCallback(&RxCallbackAp1));

  // Install packet sink on AP2
  PacketSinkHelper sinkB("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
  ApplicationContainer sinkAppB = sinkB.Install(wifiApNodes.Get(1));
  sinkAppB.Start(Seconds(0.0));
  sinkAppB.Stop(Seconds(simTime));

  sinkAppB.Get(0)->GetObject<PacketSink>()->TraceConnectWithoutContext("Rx", MakeCallback(&RxCallbackAp2));

  NS_LOG_INFO("Install Full Queue Traffic...");

  // Install full queue traffic for each STA
  for (uint32_t i = 0; i < wifiStaNodesA.GetN(); ++i) {
    Address remoteAddress(InetSocketAddress(apInterfaceA.GetAddress(0), port));
    InstallFullQueueTraffic(wifiStaNodesA.Get(i), remoteAddress, 1.0, simTime);
  }

  for (uint32_t i = 0; i < wifiStaNodesB.GetN(); ++i) {
    Address remoteAddress(InetSocketAddress(apInterfaceB.GetAddress(0), port));
    InstallFullQueueTraffic(wifiStaNodesB.Get(i), remoteAddress, 1.0, simTime);
  }

  NS_LOG_INFO("Enable Tracing and Run Simulation...");

  // Enable tracing for the APs
  Simulator::Schedule(Seconds(1.0), &LogThroughput);

  // Start the simulation
  Simulator::Stop(Seconds(simTime));
  Simulator::Run();
  Simulator::Destroy();

  NS_LOG_INFO("Simulation Completed");

  return 0;
}
