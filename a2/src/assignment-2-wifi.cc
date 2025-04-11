#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/ssid.h"
#include <fstream>
#include <map>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("WifiSimulation");

uint64_t ap1Bytes = 0;
uint64_t ap2Bytes = 0;

// Callback function to log received packets AP 1
void RxCallbackAp1(Ptr<const Packet> packet, const Address &addr) {
    ap1Bytes += packet->GetSize();
}

// Callback function to log received packets AP 2
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

// Function to install traffic generator applications
ApplicationContainer InstallFullQueueTraffic(Ptr<Node> sender, Address sinkAddress, double startTime, double stopTime) {
    OnOffHelper onoff("ns3::UdpSocketFactory", sinkAddress);
    onoff.SetConstantRate(DataRate("1Mbps"));
    onoff.SetAttribute("PacketSize", UintegerValue(1472));
    onoff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1000]"));
    onoff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    ApplicationContainer app = onoff.Install(sender);
    app.Start(Seconds(startTime));
    app.Stop(Seconds(stopTime));
    return app;
}

// Function to save MAC addresses to a CSV file
void SaveMacAddresses(NodeContainer staNodesA, NodeContainer staNodesB) {
    std::ofstream macFile("sta_mac_addresses.csv");
    macFile << "STA,MAC Address" << std::endl;
    for (uint32_t i = 0; i < staNodesA.GetN(); ++i) {
        Ptr<NetDevice> device = staNodesA.Get(i)->GetDevice(0);
        Mac48Address mac = Mac48Address::ConvertFrom(device->GetAddress());
        macFile << "STA-A-" << i + 1 << "," << mac << std::endl;
    }
    for (uint32_t i = 0; i < staNodesB.GetN(); ++i) {
        Ptr<NetDevice> device = staNodesB.Get(i)->GetDevice(0);
        Mac48Address mac = Mac48Address::ConvertFrom(device->GetAddress());
        macFile << "STA-B-" << i + 1 << "," << mac << std::endl;
    }
    macFile.close();
    NS_LOG_INFO("MAC addresses saved to sta_mac_addresses.csv");
}

// Function to log handover events
void LogHandover(uint32_t time, Ptr<Node> sta, const std::string& fromAp, const std::string& toAp) {
    Ptr<NetDevice> device = sta->GetDevice(0);
    Mac48Address mac = Mac48Address::ConvertFrom(device->GetAddress());

    static std::ofstream handoverFile("handover.csv", std::ios::app);
    handoverFile << time << "," << mac << "," << fromAp << "," << toAp << std::endl;
    NS_LOG_INFO("Handover - Time: " << time << "s, STA MAC: " << mac << " From: " << fromAp << " To: " << toAp);
}

// Function to switch APs for a STA
void SwitchAp(Ptr<WifiNetDevice> staDevice, Ssid newSsid, Ptr<OnOffApplication> app, Ipv4Address newApIp)
{
    Ptr<StaWifiMac> staMac = DynamicCast<StaWifiMac>(staDevice->GetMac());
    if (staMac)
    {   
        AddressValue remoteAddress(InetSocketAddress(newApIp, 9));
        app->SetAttribute("Remote", remoteAddress);
        NS_LOG_INFO("STA " << staDevice->GetNode()->GetId() << " switched to SSID: " << newSsid);
        NS_LOG_INFO("STA " << staDevice->GetNode()->GetId() << " updated destination to " << newApIp);
    }
    else
    {
        NS_LOG_ERROR("Failed to cast to StaWifiMac for STA " << staDevice->GetNode()->GetId());
    }
}

// Function to perform the switch of APs for a group of STAs
void PerformSwitchAp(NodeContainer &staNodesA, NodeContainer &staNodesB, double percentageA, double percentageB, Ssid ssidA, Ssid ssidB, uint32_t time
, std::vector<Ptr<OnOffApplication>> &appsA, std::vector<Ptr<OnOffApplication>> &appsB, Ipv4Address apAIp, Ipv4Address apBIp)
{
    uint32_t totalStaA = staNodesA.GetN();
    uint32_t totalStaB = staNodesB.GetN();
    
    uint32_t numToSwitchA = static_cast<uint32_t>(totalStaA * percentageA);
    uint32_t numToSwitchB = static_cast<uint32_t>(totalStaB * percentageB);
    NS_LOG_INFO("Switching " << numToSwitchA << " STAs from SSID " << ssidA << " to " << ssidB);

    NodeContainer movingStaA;

    // Move STAs from AP-A to AP-B
    for (uint32_t i = 0; i < numToSwitchA; ++i) {
        movingStaA.Add(staNodesA.Get(i));
    }
    staNodesB.Add(movingStaA);

    // Create temporary containers for the remaining STAs
    NodeContainer remainingStaA;
    for (uint32_t i = numToSwitchA; i < staNodesA.GetN(); ++i) {
        remainingStaA.Add(staNodesA.Get(i));
    }
    staNodesA = remainingStaA;

    // Move STAs from AP-B to AP-A
    NodeContainer movingStaB;
    for (uint32_t i = 0; i < numToSwitchB; ++i) {
        movingStaB.Add(staNodesB.Get(i));
    }
    staNodesA.Add(movingStaB);

    // Create temporary containers for the remaining STAs
    NodeContainer remainingStaB;
    for (uint32_t i = numToSwitchB; i < staNodesB.GetN(); ++i) {
        remainingStaB.Add(staNodesB.Get(i));
    }
    staNodesB = remainingStaB;

    // Switch APs for the selected STAs
    for (uint32_t i = 0; i < numToSwitchA; ++i)
    {
        Ptr<Node> staNode = movingStaA.Get(i);
        // Set new SSID for the STA
        Ptr<MobilityModel> staA = staNode->GetObject<MobilityModel>();
        if (staA)
        {
            Ptr<UniformRandomVariable> randX = CreateObject<UniformRandomVariable>();
            randX->SetAttribute("Min", DoubleValue(5.0));
            randX->SetAttribute("Max", DoubleValue(10.0));
            
            Ptr<UniformRandomVariable> randY = CreateObject<UniformRandomVariable>();
            randY->SetAttribute("Min", DoubleValue(-10.0));
            randY->SetAttribute("Max", DoubleValue(10.0));

            double x = randX->GetValue();
            double y = randY->GetValue();
            double z = 0.0;
            staA->SetPosition(Vector(x, y, z));
        }
        else
        {
            NS_LOG_ERROR("Failed to get MobilityModel for STA " << staNode->GetId());
        }
        Ptr<WifiNetDevice> staDevice = DynamicCast<WifiNetDevice>(staNode->GetDevice(0));
        if (staDevice)
        {
            OnOffHelper onoff2("ns3::UdpSocketFactory", InetSocketAddress(apBIp, 9));
            onoff2.SetConstantRate(DataRate("1Mbps"));
            onoff2.SetAttribute("PacketSize", UintegerValue(1472));
            onoff2.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1000]"));
            onoff2.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
            ApplicationContainer app = onoff2.Install(staNode);
            app.Stop(Seconds(time));
            app.Start(Seconds(time));
            app.Stop(Seconds(time+100));
        }
        else
        {
            NS_LOG_ERROR("Failed to get WifiNetDevice for STA " << staNode->GetId());
        }
        LogHandover(time, staNode, "network-A", "network-B");
    }

    NS_LOG_INFO("Switching " << numToSwitchB << " STAs from SSID " << ssidB << " to " << ssidA);
    // Switch APs for the selected STAs
    for (uint32_t i = 0; i < numToSwitchB; ++i)
    {
        Ptr<Node> staNode = movingStaB.Get(i);
        Ptr<MobilityModel> staB = staNode->GetObject<MobilityModel>();
        if (staB)
        {
            Ptr<UniformRandomVariable> randX = CreateObject<UniformRandomVariable>();
            randX->SetAttribute("Min", DoubleValue(-5.0));
            randX->SetAttribute("Max", DoubleValue(5.0));
            
            Ptr<UniformRandomVariable> randY = CreateObject<UniformRandomVariable>();
            randY->SetAttribute("Min", DoubleValue(-10.0));
            randY->SetAttribute("Max", DoubleValue(10.0));

            double x = randX->GetValue();
            double y = randY->GetValue();
            double z = 0.0;
            staB->SetPosition(Vector(x, y, z));
        }
        else
        {
            NS_LOG_ERROR("Failed to get MobilityModel for STA " << staNode->GetId());
        }
        Ptr<WifiNetDevice> staDevice = DynamicCast<WifiNetDevice>(staNode->GetDevice(0));
        if (staDevice)
        {
            OnOffHelper onoff1("ns3::UdpSocketFactory", InetSocketAddress(apAIp, 9));
            onoff1.SetConstantRate(DataRate("1Mbps"));
            onoff1.SetAttribute("PacketSize", UintegerValue(1472));
            onoff1.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1000]"));
            onoff1.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
            ApplicationContainer app = onoff1.Install(staNode);
            app.Stop(Seconds(time));
            app.Start(Seconds(time));
            app.Stop(Seconds(time+100));
        }
        else
        {
            NS_LOG_ERROR("Failed to get WifiNetDevice for STA " << staNode->GetId());
        }
        LogHandover(time, staNode, "network-B", "network-A");
    }
}

int main(int argc, char *argv[]) {
    uint32_t staA = 16;
    uint32_t staB = 16;
    double simTime = 60.0;
    LogComponentEnable("WifiSimulation", LOG_LEVEL_INFO);

    NS_LOG_INFO("Create Nodes...");

    // Create nodes for STA-A and STA-B
    NodeContainer wifiStaNodesA, wifiStaNodesB, wifiApNodes;
    wifiStaNodesA.Create(staA);
    wifiStaNodesB.Create(staB);
    wifiApNodes.Create(2);

    NS_LOG_INFO("Create Wifi Channel...");
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    // Set Wifi PHY parameters
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211n);
    wifi.SetRemoteStationManager("ns3::MinstrelHtWifiManager");

    // Set Wifi PHY parameters
    WifiMacHelper mac;
    Ssid ssidA = Ssid("network-A");
    Ssid ssidB = Ssid("network-B");

    // Set Wifi MAC parameters
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssidA), "ActiveProbing", BooleanValue(false));
    NetDeviceContainer staDevicesA = wifi.Install(phy, mac, wifiStaNodesA);

    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssidB), "ActiveProbing", BooleanValue(false));
    NetDeviceContainer staDevicesB = wifi.Install(phy, mac, wifiStaNodesB);

    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssidA));
    NetDeviceContainer apDeviceA = wifi.Install(phy, mac, wifiApNodes.Get(0));

    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssidB));
    NetDeviceContainer apDeviceB = wifi.Install(phy, mac, wifiApNodes.Get(1));

    MobilityHelper mobility;
    // Set Mobility for STA-A Nodes (near AP-A)
    MobilityHelper mobilityStaA;
    mobilityStaA.SetPositionAllocator("ns3::RandomRectanglePositionAllocator",
                                             "X", StringValue("ns3::UniformRandomVariable[Min=-5.0|Max=5.0]"),
                                             "Y", StringValue("ns3::UniformRandomVariable[Min=-10.0|Max=10.0]"));
    mobilityStaA.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                                         "Bounds", RectangleValue(Rectangle(-10, 10, -10, 10)),
                                         "Speed", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"),
                                         "Distance", DoubleValue(5.0));
    mobilityStaA.Install(wifiStaNodesA);

    // Set Mobility for STA-B Nodes (near AP-B at (10,0,0))
    MobilityHelper mobilityStaB;
    mobilityStaB.SetPositionAllocator("ns3::RandomRectanglePositionAllocator",
                                             "X", StringValue("ns3::UniformRandomVariable[Min=5.0|Max=10.0]"),
                                             "Y", StringValue("ns3::UniformRandomVariable[Min=-10.0|Max=10.0]"));
    mobilityStaB.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                                         "Bounds", RectangleValue(Rectangle(-10, 10, -10, 10)),
                                         "Speed", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"),
                                         "Distance", DoubleValue(5.0));
    mobilityStaB.Install(wifiStaNodesB);

    // Set Mobility for AP Nodes
    MobilityHelper mobilityAp;
    mobilityAp.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobilityAp.Install(wifiApNodes);

    // Set AP A Position at (0,0,0)
    Ptr<MobilityModel> mobApA = wifiApNodes.Get(0)->GetObject<MobilityModel>();
    mobApA->SetPosition(Vector(0.0, 0.0, 0.0));

    // Set AP B Position at (10,0,0)
    Ptr<MobilityModel> mobApB = wifiApNodes.Get(1)->GetObject<MobilityModel>();
    mobApB->SetPosition(Vector(10.0, 0.0, 0.0));


    InternetStackHelper stack;
    stack.Install(wifiStaNodesA);
    stack.Install(wifiStaNodesB);
    stack.Install(wifiApNodes);

    // Assign IP addresses to the nodes
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer apInterfaceA = address.Assign(apDeviceA);
    Ipv4InterfaceContainer apInterfaceB = address.Assign(apDeviceB);
    address.Assign(staDevicesA);
    address.Assign(staDevicesB);

    // Set up packet sink to receive packets at APs
    PacketSinkHelper sink("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), 9));v
    ApplicationContainer sinkAppsA = sink.Install(wifiApNodes.Get(0));
    sinkAppsA.Start(Seconds(0.0));
    sinkAppsA.Stop(Seconds(simTime));
    sinkAppsA.Get(0)->GetObject<PacketSink>()->TraceConnectWithoutContext("Rx", MakeCallback(&RxCallbackAp1));
60
    ApplicationContainer sinkAppsB = sink.Install(wifiApNodes.Get(1));
    sinkAppsB.Start(Seconds(0.0));
    sinkAppsB.Stop(Seconds(simTime));
    sinkAppsB.Get(0)->GetObject<PacketSink>()->TraceConnectWithoutContext("Rx", MakeCallback(&RxCallbackAp2));
    std::vector<Ptr<OnOffApplication>> appsA, appsB;
    
    // Install traffic generator applications on STAs
    for (uint32_t i = 0; i < wifiStaNodesA.GetN(); ++i) {
        ApplicationContainer tempApp;
        tempApp = InstallFullQueueTraffic(wifiStaNodesA.Get(i), InetSocketAddress(apInterfaceA.GetAddress(0), 9), 1.0, simTime);
        appsA.push_back(DynamicCast<OnOffApplication>(tempApp.Get(0)));
    }

    // Install traffic generator applications on STAs
    for (uint32_t i = 0; i < wifiStaNodesB.GetN(); ++i) {
        ApplicationContainer tempApp;
        tempApp = InstallFullQueueTraffic(wifiStaNodesB.Get(i), InetSocketAddress(apInterfaceB.GetAddress(0), 9), 1.0, simTime);
        appsB.push_back(DynamicCast<OnOffApplication>(tempApp.Get(0)));
    }
    // First Handover at 20 seconds
    Simulator::Schedule(Seconds(20.0), &PerformSwitchAp, std::ref(wifiStaNodesA), std::ref(wifiStaNodesB), 0.25, 0.5, ssidA, ssidB, 20.0,
    std::ref(appsA), std::ref(appsB), apInterfaceA.GetAddress(0), apInterfaceB.GetAddress(0));

    // Second Handover at 40 seconds
    Simulator::Schedule(Seconds(40.0), &PerformSwitchAp, std::ref(wifiStaNodesA), std::ref(wifiStaNodesB), 0.5, 0.5, ssidA, ssidB, 40.0,
    std::ref(appsA), std::ref(appsB), apInterfaceA.GetAddress(0), apInterfaceB.GetAddress(0));
    

    Simulator::Schedule(Seconds(1.0), &LogThroughput);
    Simulator::Schedule(Seconds(0.5), &SaveMacAddresses, wifiStaNodesA, wifiStaNodesB);

    NS_LOG_INFO("Run Simulation...");
    Simulator::Stop(Seconds(simTime));
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("Simulation Done.");
}
