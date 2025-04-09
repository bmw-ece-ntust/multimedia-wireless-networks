// LTE Handover Simulation with Throughput Analysis for ns-3.39
// Two eNodeBs and 32 UEs. Mobility, handovers, and throughput logged in detail.

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/flow-monitor-module.h"
#include <fstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("LteHandoverSim");

// Global output file for throughput
static std::ofstream throughputFile;

// Function: Print all UEs' connection status
void PrintUeConnectionStatus(Ptr<LteHelper> lteHelper, NetDeviceContainer ueLteDevs, uint32_t numUes) {
    NS_LOG_INFO("=== UE Connection Status ===");
    for (uint32_t i = 0; i < numUes; ++i) {
        Ptr<LteUeNetDevice> ueDev = DynamicCast<LteUeNetDevice>(ueLteDevs.Get(i));
        uint16_t cellId = ueDev->GetRrc()->GetCellId();
        uint64_t imsi = ueDev->GetImsi();
        NS_LOG_INFO("UE " << i << ": IMSI " << imsi << ", Connected to Cell ID " << cellId);
    }
}

// Function: Log UE positions
void LogUePositions(NodeContainer ueNodes, uint32_t numUes, std::ofstream& posFile) {
    for (uint32_t i = 0; i < numUes; ++i) {
        Ptr<MobilityModel> mob = ueNodes.Get(i)->GetObject<MobilityModel>();
        Vector pos = mob->GetPosition();
        posFile << Simulator::Now().GetSeconds() << "," << i << "," << pos.x << "," << pos.y << "\n";
    }
}

// Function: Monitor and log throughput
void MonitorThroughput(Ptr<FlowMonitor> flowMon, Ptr<Ipv4FlowClassifier> classifier, Ptr<LteHelper> lteHelper,
                       NetDeviceContainer ueLteDevs, Ipv4InterfaceContainer ueIpIfaces, uint32_t numUes) {
    flowMon->CheckForLostPackets();
    FlowMonitor::FlowStatsContainer stats = flowMon->GetFlowStats();

    double throughputEnbA = 0.0; // Cell ID 1
    double throughputEnbB = 0.0; // Cell ID 2

    for (auto it = stats.begin(); it != stats.end(); ++it) {
        uint32_t flowId = it->first;
        Ipv4FlowClassifier::FiveTuple tuple = classifier->FindFlow(flowId);
        for (uint32_t i = 0; i < numUes; ++i) {
            if (tuple.destinationAddress == ueIpIfaces.GetAddress(i)) {
                Ptr<LteUeNetDevice> ueDev = DynamicCast<LteUeNetDevice>(ueLteDevs.Get(i));
                uint16_t cellId = ueDev->GetRrc()->GetCellId();
                double tput = 0.0;
                if (it->second.timeLastRxPacket > it->second.timeFirstTxPacket) {
                    tput = (it->second.rxBytes * 8.0) /
                           (it->second.timeLastRxPacket.GetSeconds() - it->second.timeFirstTxPacket.GetSeconds()) / 1e6;
                }
                if (cellId == 1) {
                    throughputEnbA += tput;
                } else if (cellId == 2) {
                    throughputEnbB += tput;
                }
                break;
            }
        }
    }

    NS_LOG_INFO("Time " << Simulator::Now().GetSeconds() << "s: eNodeB A Throughput = " << throughputEnbA
                        << " Mbps, eNodeB B Throughput = " << throughputEnbB << " Mbps");
    throughputFile << Simulator::Now().GetSeconds() << "," << throughputEnbA << "," << throughputEnbB << "\n";

    // Schedule next throughput check
    Simulator::Schedule(Seconds(0.1), &MonitorThroughput, flowMon, classifier, lteHelper, ueLteDevs, ueIpIfaces, numUes);
}

int main(int argc, char *argv[]) {
    Time simTime = Seconds(600.0); // Full simulation period
    CommandLine cmd;
    cmd.Parse(argc, argv);

    // Enable logging
    LogComponentEnable("LteHandoverSim", LOG_LEVEL_INFO);

    // Open throughput file globally
    throughputFile.open("throughput.csv");
    throughputFile << "Time,Throughput_EnbA,Throughput_EnbB\n";

    // Create LTE and EPC helpers
    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
    Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
    lteHelper->SetEpcHelper(epcHelper);

    // Configure handover algorithm
    lteHelper->SetHandoverAlgorithmType("ns3::A3RsrpHandoverAlgorithm");
    lteHelper->SetHandoverAlgorithmAttribute("Hysteresis", DoubleValue(0.5));
    lteHelper->SetHandoverAlgorithmAttribute("TimeToTrigger", TimeValue(MilliSeconds(200)));

    // Configure path loss model
    lteHelper->SetAttribute("PathlossModel", StringValue("ns3::LogDistancePropagationLossModel"));
    lteHelper->SetPathlossModelAttribute("Exponent", DoubleValue(3.0));

    // Create nodes
    NodeContainer enbNodes;
    enbNodes.Create(2);
    NodeContainer ueNodes;
    ueNodes.Create(32);
    NodeContainer remoteHost;
    remoteHost.Create(1);

    // Set eNodeB positions
    MobilityHelper enbMobility;
    enbMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    enbMobility.Install(enbNodes);
    enbNodes.Get(0)->GetObject<MobilityModel>()->SetPosition(Vector(0, 0, 0));    // eNodeB A
    enbNodes.Get(1)->GetObject<MobilityModel>()->SetPosition(Vector(500, 0, 0));  // eNodeB B

    // Set UE positions
    MobilityHelper ueMobility;
    ueMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    ueMobility.Install(ueNodes);
    for (uint32_t i = 0; i < 16; ++i) {
        ueNodes.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(50 + i * 5, 10, 0));
    }
    for (uint32_t i = 16; i < 32; ++i) {
        ueNodes.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(550 + (i - 16) * 5, -10, 0));
    }

    // Set remote host position
    MobilityHelper remoteHostMobility;
    remoteHostMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    remoteHostMobility.Install(remoteHost);

    // Install LTE devices
    NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice(enbNodes);
    NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice(ueNodes);

    // Install internet stack
    InternetStackHelper internet;
    internet.Install(ueNodes);
    internet.Install(remoteHost);

    // Connect remote host to EPC
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("10Gbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));
    NetDeviceContainer p2pDevs = p2p.Install(remoteHost.Get(0), epcHelper->GetPgwNode());

    // Assign IP addresses
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces = ipv4.Assign(p2pDevs);
    Ipv4InterfaceContainer ueIpIfaces = epcHelper->AssignUeIpv4Address(ueLteDevs);

    // Set up routing
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> remoteHostRouting = ipv4RoutingHelper.GetStaticRouting(remoteHost.Get(0)->GetObject<Ipv4>());
    remoteHostRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);

    // Attach UEs to eNodeBs
    for (uint32_t i = 0; i < 16; ++i) {
        lteHelper->Attach(ueLteDevs.Get(i), enbLteDevs.Get(0));
    }
    for (uint32_t i = 16; i < 32; ++i) {
        lteHelper->Attach(ueLteDevs.Get(i), enbLteDevs.Get(1));
    }

    // Set up UDP traffic
    uint16_t basePort = 49153;
    ApplicationContainer serverApps, clientApps;
    for (uint32_t i = 0; i < 32; ++i) {
        UdpServerHelper udpServer(basePort + i);
        serverApps.Add(udpServer.Install(ueNodes.Get(i)));
        serverApps.Start(Seconds(0.5));
        serverApps.Stop(simTime);

        UdpClientHelper udpClient(ueIpIfaces.GetAddress(i), basePort + i);
        udpClient.SetAttribute("MaxPackets", UintegerValue(1000000));
        udpClient.SetAttribute("Interval", TimeValue(MilliSeconds(10))); // 100 packets/s
        udpClient.SetAttribute("PacketSize", UintegerValue(1024)); // 1024 bytes
        clientApps.Add(udpClient.Install(remoteHost.Get(0)));
        clientApps.Start(Seconds(1.0));
        clientApps.Stop(simTime);
    }

    // Set up FlowMonitor
    FlowMonitorHelper flowMonHelper;
    flowMonHelper.SetMonitorAttribute("StartTime", TimeValue(Seconds(1.0)));
    Ptr<FlowMonitor> flowMon = flowMonHelper.InstallAll();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowMonHelper.GetClassifier());

    // Open position file
    std::ofstream posFile("ue_positions.csv");
    posFile << "Time,UE_Index,X,Y\n";

    // Schedule initial tasks
    Simulator::Schedule(Seconds(1.0), &PrintUeConnectionStatus, lteHelper, ueLteDevs, 32);
    Simulator::Schedule(Seconds(1.0), &MonitorThroughput, flowMon, classifier, lteHelper, ueLteDevs, ueIpIfaces, 32);
    Simulator::Schedule(Seconds(1.0), &LogUePositions, ueNodes, 32, std::ref(posFile));

    // First handover event at 3 minutes
    Simulator::Schedule(Seconds(180.0), [&ueNodes, &posFile]() {
        NS_LOG_INFO("[3min] 4 UEs from eNB A → B; 8 UEs from B → A");
        for (uint32_t i = 0; i < 4; ++i) {
            ueNodes.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(550 + i * 5, 20, 0));
        }
        for (uint32_t i = 0; i < 8; ++i) {
            ueNodes.Get(16 + i)->GetObject<MobilityModel>()->SetPosition(Vector(50 + i * 5, -20, 0));
        }
        LogUePositions(ueNodes, 32, posFile);
    });

    // Print connection status after first handover
    Simulator::Schedule(Seconds(185.0), &PrintUeConnectionStatus, lteHelper, ueLteDevs, 32);

    // Second handover event at 6 minutes
    Simulator::Schedule(Seconds(360.0), [&ueNodes, &posFile]() {
        NS_LOG_INFO("[6min] Half of UEs on A & B swap positions");
        for (uint32_t i = 0; i < 8; ++i) {
            ueNodes.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(550 + i * 5, 30, 0));
        }
        for (uint32_t i = 0; i < 8; ++i) {
            ueNodes.Get(16 + i)->GetObject<MobilityModel>()->SetPosition(Vector(50 + i * 5, 30, 0));
        }
        LogUePositions(ueNodes, 32, posFile);
    });

    // Print connection status after second handover
    Simulator::Schedule(Seconds(365.0), &PrintUeConnectionStatus, lteHelper, ueLteDevs, 32);

    // Final position logging
    Simulator::Schedule(simTime, &LogUePositions, ueNodes, 32, std::ref(posFile));

    Simulator::Stop(simTime);
    Simulator::Run();

    // Close files
    throughputFile.close();
    posFile.close();

    Simulator::Destroy();
    NS_LOG_INFO("Simulation completed successfully!");
    return 0;
}