#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/packet-sink.h"
#include "ns3/flow-monitor-module.h"
#include <vector>
#include <string>
#include <algorithm>
#include <random>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("WifiSimulation");

// Struct to keep track of node associations
struct NodeAssociation {
    Ptr<Node> node;
    int originalAP;  // 0 for AP-A, 1 for AP-B
    int currentAP;   // 0 for AP-A, 1 for AP-B
    uint32_t id;     // Node ID for tracking
};

// Function to move users between APs
void MoveUsers(Ptr<Node> apA, Ptr<Node> apB, 
               std::vector<NodeAssociation>& staNodesA, 
               std::vector<NodeAssociation>& staNodesB,
               double fractionAtoB, double fractionBtoA) {
    
    NS_LOG_INFO("Starting node movement at time " << Simulator::Now().GetSeconds() << "s");
    
    // Get positions of AP nodes
    Ptr<MobilityModel> mobA = apA->GetObject<MobilityModel>();
    Ptr<MobilityModel> mobB = apB->GetObject<MobilityModel>();
    
    Vector posA = mobA->GetPosition();
    Vector posB = mobB->GetPosition();
    
    NS_LOG_INFO("AP-A position: (" << posA.x << ", " << posA.y << ", " << posA.z << ")");
    NS_LOG_INFO("AP-B position: (" << posB.x << ", " << posB.y << ", " << posB.z << ")");
    
    // Calculate how many nodes to move
    uint32_t numAtoB = static_cast<uint32_t>(staNodesA.size() * fractionAtoB);
    uint32_t numBtoA = static_cast<uint32_t>(staNodesB.size() * fractionBtoA);
    
    NS_LOG_INFO("Moving " << numAtoB << " nodes from AP-A to AP-B");
    NS_LOG_INFO("Moving " << numBtoA << " nodes from AP-B to AP-A");
    
    // Randomly select nodes from A to move to B
    std::vector<uint32_t> indicesA(staNodesA.size());
    for (uint32_t i = 0; i < staNodesA.size(); ++i) {
        indicesA[i] = i;
    }
    
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(indicesA.begin(), indicesA.end(), g);
    
    // Move nodes from A to B
    for (uint32_t i = 0; i < numAtoB && i < indicesA.size(); ++i) {
        Ptr<MobilityModel> mobility = staNodesA[indicesA[i]].node->GetObject<MobilityModel>();
        
        // Set position near AP-B (with some random variation)
        double randX = posB.x + (2.0 * (double)rand() / RAND_MAX - 1.0) * 5.0;
        double randY = posB.y + (2.0 * (double)rand() / RAND_MAX - 1.0) * 5.0;
        
        mobility->SetPosition(Vector(randX, randY, 0.0));
        staNodesA[indicesA[i]].currentAP = 1;  // Now associated with AP-B
        
        NS_LOG_INFO("Node " << staNodesA[indicesA[i]].id << " moved from AP-A to AP-B");
    }
    
    // Randomly select nodes from B to move to A
    std::vector<uint32_t> indicesB(staNodesB.size());
    for (uint32_t i = 0; i < staNodesB.size(); ++i) {
        indicesB[i] = i;
    }
    
    std::shuffle(indicesB.begin(), indicesB.end(), g);
    
    // Move nodes from B to A
    for (uint32_t i = 0; i < numBtoA && i < indicesB.size(); ++i) {
        Ptr<MobilityModel> mobility = staNodesB[indicesB[i]].node->GetObject<MobilityModel>();
        
        // Set position near AP-A (with some random variation)
        double randX = posA.x + (2.0 * (double)rand() / RAND_MAX - 1.0) * 5.0;
        double randY = posA.y + (2.0 * (double)rand() / RAND_MAX - 1.0) * 5.0;
        
        mobility->SetPosition(Vector(randX, randY, 0.0));
        staNodesB[indicesB[i]].currentAP = 0;  // Now associated with AP-A
        
        NS_LOG_INFO("Node " << staNodesB[indicesB[i]].id << " moved from AP-B to AP-A");
    }
    
    NS_LOG_INFO("Node movement completed at time " << Simulator::Now().GetSeconds() << "s");
}

// Helper function to convert vector to string
std::string VectorToString(const std::vector<uint32_t>& vec) {
    std::string result = "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        result += std::to_string(vec[i]);
        if (i < vec.size() - 1) {
            result += ", ";
        }
    }
    result += "]";
    return result;
}

// Function to print the current status of nodes
void PrintNodeStatus(std::vector<NodeAssociation>& staNodesA, 
                    std::vector<NodeAssociation>& staNodesB) {
    
    uint32_t countA_atA = 0;
    uint32_t countA_atB = 0;
    uint32_t countB_atA = 0;
    uint32_t countB_atB = 0;
    
    std::vector<uint32_t> nodesA_atA;
    std::vector<uint32_t> nodesA_atB;
    std::vector<uint32_t> nodesB_atA;
    std::vector<uint32_t> nodesB_atB;
    
    // Count nodes from group A
    for (const auto& node : staNodesA) {
        if (node.currentAP == 0) {
            countA_atA++;
            nodesA_atA.push_back(node.id);
        } else {
            countA_atB++;
            nodesA_atB.push_back(node.id);
        }
    }
    
    // Count nodes from group B
    for (const auto& node : staNodesB) {
        if (node.currentAP == 0) {
            countB_atA++;
            nodesB_atA.push_back(node.id);
        } else {
            countB_atB++;
            nodesB_atB.push_back(node.id);
        }
    }
    
    NS_LOG_INFO("==== Node Status at " << Simulator::Now().GetSeconds() << "s ====");
    NS_LOG_INFO("Group A nodes at AP-A: " << countA_atA << " nodes: " << VectorToString(nodesA_atA));
    NS_LOG_INFO("Group A nodes at AP-B: " << countA_atB << " nodes: " << VectorToString(nodesA_atB));
    NS_LOG_INFO("Group B nodes at AP-A: " << countB_atA << " nodes: " << VectorToString(nodesB_atA));
    NS_LOG_INFO("Group B nodes at AP-B: " << countB_atB << " nodes: " << VectorToString(nodesB_atB));
    NS_LOG_INFO("==========================================");
}

// Function to print flow statistics
void PrintFlowStatistics(Ptr<FlowMonitor> flowMonitor, FlowMonitorHelper& flowHelper) {
    NS_LOG_INFO("=== Flow Statistics at " << Simulator::Now().GetSeconds() << "s ===");
    
    flowMonitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());
    std::map<FlowId, FlowMonitor::FlowStats> stats = flowMonitor->GetFlowStats();
    
    uint32_t totalTxPackets = 0;
    uint32_t totalRxPackets = 0;
    uint32_t totalLostPackets = 0;
    double totalThroughput = 0.0;
    
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin(); i != stats.end(); ++i) {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);
        
        NS_LOG_INFO("Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")");
        NS_LOG_INFO("  Tx Packets: " << i->second.txPackets);
        NS_LOG_INFO("  Rx Packets: " << i->second.rxPackets);
        NS_LOG_INFO("  Lost Packets: " << i->second.lostPackets);
        
        if (i->second.rxPackets > 0) {
            double throughput = i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds()) / 1000000.0;
            NS_LOG_INFO("  Throughput: " << throughput << " Mbps");
            totalThroughput += throughput;
        }
        
        totalTxPackets += i->second.txPackets;
        totalRxPackets += i->second.rxPackets;
        totalLostPackets += i->second.lostPackets;
    }
    
    NS_LOG_INFO("=== Summary Statistics ===");
    NS_LOG_INFO("Total Tx Packets: " << totalTxPackets);
    NS_LOG_INFO("Total Rx Packets: " << totalRxPackets);
    NS_LOG_INFO("Total Lost Packets: " << totalLostPackets);
    NS_LOG_INFO("Network Throughput: " << totalThroughput << " Mbps");
    NS_LOG_INFO("===============================");
}

int main(int argc, char *argv[]) {
    // Enable logging
    LogComponentEnable("WifiSimulation", LOG_LEVEL_INFO);
    // LogComponentEnable("PacketSink", LOG_LEVEL_INFO);

    CommandLine cmd;
    cmd.Parse(argc, argv);
    
    // Create nodes
    NodeContainer apNodes;
    apNodes.Create(2);
    
    NodeContainer staNodesA, staNodesB;
    staNodesA.Create(16); //initial group A STA
    staNodesB.Create(16); //initial group B STA
    
    // Set up WiFi
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211ax);
    
    // Set up channel and PHY
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());
    
    // Set up MAC
    WifiMacHelper mac;
    Ssid ssidA = Ssid("wifi-network-a");
    Ssid ssidB = Ssid("wifi-network-b");
    
    // Create AP devices
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssidA));
    NetDeviceContainer apDeviceA = wifi.Install(phy, mac, apNodes.Get(0));
    
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssidB));
    NetDeviceContainer apDeviceB = wifi.Install(phy, mac, apNodes.Get(1));
    
    // Create STA devices for group A - initially connect to AP-A
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssidA));
    NetDeviceContainer staDevicesA = wifi.Install(phy, mac, staNodesA);
    
    // Create STA devices for group B - initially connect to AP-B
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssidB));
    NetDeviceContainer staDevicesB = wifi.Install(phy, mac, staNodesB);
    
    // Set up mobility models
    MobilityHelper mobility;
    
    // Position AP-A at (0,0,0)
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));  // AP-A
    positionAlloc->Add(Vector(50.0, 0.0, 0.0)); // AP-B with 50m separation
    
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(apNodes);
    
    // Random positions for STA nodes around their respective APs
    mobility.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue("0.0"),
                                 "Y", StringValue("0.0"),
                                 "Rho", StringValue("ns3::UniformRandomVariable[Min=0|Max=10]"));
    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue(Rectangle(-25, 25, -25, 25)),
                             "Speed", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"));
    mobility.Install(staNodesA);
    
    mobility.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue("50.0"),
                                 "Y", StringValue("0.0"),
                                 "Rho", StringValue("ns3::UniformRandomVariable[Min=0|Max=10]"));
    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue(Rectangle(25, 75, -25, 25)),
                             "Speed", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"));
    mobility.Install(staNodesB);
    
    // Set up internet stack
    InternetStackHelper stack;
    stack.Install(apNodes);
    stack.Install(staNodesA);
    stack.Install(staNodesB);
    
    // Set up IP addresses
    Ipv4AddressHelper address;
    
    // Network for AP-A and its stations
    address.SetBase("192.168.1.0", "255.255.255.0");
    Ipv4InterfaceContainer apInterfaceA = address.Assign(apDeviceA);
    Ipv4InterfaceContainer staInterfacesA = address.Assign(staDevicesA);
    
    // Network for AP-B and its stations
    address.SetBase("192.168.2.0", "255.255.255.0");
    Ipv4InterfaceContainer apInterfaceB = address.Assign(apDeviceB);
    Ipv4InterfaceContainer staInterfacesB = address.Assign(staDevicesB);
    
    // Set up applications - Full-Queue Model
    // Server apps on APs
    uint16_t port = 9;
    
    // TCP sink on AP-A
    PacketSinkHelper sinkA("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkAppsA = sinkA.Install(apNodes.Get(0));
    sinkAppsA.Start(Seconds(0.0));
    sinkAppsA.Stop(Seconds(600.0));
    
    // TCP sink on AP-B
    PacketSinkHelper sinkB("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkAppsB = sinkB.Install(apNodes.Get(1));
    sinkAppsB.Start(Seconds(0.0));
    sinkAppsB.Stop(Seconds(600.0));
    
    // Client applications - continuous transmission (full-queue model)
    // For group A stations
    ApplicationContainer clientAppsA;
    for (uint32_t i = 0; i < staNodesA.GetN(); ++i) {
        OnOffHelper clientA("ns3::TcpSocketFactory", InetSocketAddress(apInterfaceA.GetAddress(0), port));
        clientA.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        clientA.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        clientA.SetAttribute("DataRate", DataRateValue(DataRate("10Mb/s")));
        clientA.SetAttribute("PacketSize", UintegerValue(1460)); // TCP MSS size

        clientAppsA.Add(clientA.Install(staNodesA.Get(i)));
    }
    
    // For group B stations
    ApplicationContainer clientAppsB;
    for (uint32_t i = 0; i < staNodesB.GetN(); ++i) {
        OnOffHelper clientB("ns3::TcpSocketFactory", InetSocketAddress(apInterfaceB.GetAddress(0), port));
        clientB.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        clientB.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        clientB.SetAttribute("DataRate", DataRateValue(DataRate("10Mb/s")));
        clientB.SetAttribute("PacketSize", UintegerValue(1460)); // TCP MSS size

        clientAppsB.Add(clientB.Install(staNodesB.Get(i)));
    }
    
    clientAppsA.Start(Seconds(1.0));
    clientAppsA.Stop(Seconds(600.0));
    clientAppsB.Start(Seconds(1.0));
    clientAppsB.Stop(Seconds(600.0));
    
    // Create node association tracking
    std::vector<NodeAssociation> nodeAssocA;
    for (uint32_t i = 0; i < staNodesA.GetN(); ++i) {
        NodeAssociation assoc;
        assoc.node = staNodesA.Get(i);
        assoc.originalAP = 0;
        assoc.currentAP = 0;
        assoc.id = i;
        nodeAssocA.push_back(assoc);
    }
    
    std::vector<NodeAssociation> nodeAssocB;
    for (uint32_t i = 0; i < staNodesB.GetN(); ++i) {
        NodeAssociation assoc;
        assoc.node = staNodesB.Get(i);
        assoc.originalAP = 1;
        assoc.currentAP = 1;
        assoc.id = i + staNodesA.GetN(); // Offset to avoid duplicate IDs
        nodeAssocB.push_back(assoc);
    }
    
    // Print initial node status
    Simulator::Schedule(Seconds(0), &PrintNodeStatus, nodeAssocA, nodeAssocB);
    
    // At minute 3: Move 4 users from A to B and 8 users from B to A (0.25 of A, 0.5 of B)
    Simulator::Schedule(Seconds(180), &MoveUsers, apNodes.Get(0), apNodes.Get(1), 
                        nodeAssocA, nodeAssocB, 0.25, 0.5);
    Simulator::Schedule(Seconds(181), &PrintNodeStatus, nodeAssocA, nodeAssocB);
    
    // At minute 6: Move 8 users from A to B and 8 users from B to A (0.5 of A, 0.5 of B)
    Simulator::Schedule(Seconds(360), &MoveUsers, apNodes.Get(0), apNodes.Get(1), 
                        nodeAssocA, nodeAssocB, 0.5, 0.5);
    Simulator::Schedule(Seconds(361), &PrintNodeStatus, nodeAssocA, nodeAssocB);
    
    // Set up flow monitor
    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> flowMonitor = flowHelper.InstallAll();

    // Run simulation
    NS_LOG_INFO("Starting simulation...");
    Simulator::Stop(Seconds(600.0));
    Simulator::Run();
    Simulator::Destroy();
    
    NS_LOG_INFO("Simulation completed successfully!");
    
    return 0;
}

