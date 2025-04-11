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
