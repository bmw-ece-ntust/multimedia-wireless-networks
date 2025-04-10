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
#include <map>
#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("WifiHandoverSimulation");

// Extended PacketSink to track throughput
class ThroughputSink : public PacketSink {
public:
  static TypeId GetTypeId (void);
  ThroughputSink ();
  virtual ~ThroughputSink ();

  uint64_t GetLastTotalRx() const;
  void SetLastTotalRx(uint64_t lastTotalRx);

private:
  uint64_t m_lastTotalRx;
};

NS_OBJECT_ENSURE_REGISTERED(ThroughputSink);

TypeId
ThroughputSink::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ThroughputSink")
    .SetParent<PacketSink> ()
    .SetGroupName("Applications")
    .AddConstructor<ThroughputSink> ();
  return tid;
}

ThroughputSink::ThroughputSink() : m_lastTotalRx(0)
{
}

ThroughputSink::~ThroughputSink()
{
}

uint64_t
ThroughputSink::GetLastTotalRx() const
{
  return m_lastTotalRx;
}

void
ThroughputSink::SetLastTotalRx(uint64_t lastTotalRx)
{
  m_lastTotalRx = lastTotalRx;
}

// Improved centralized handover controller
class HandoverController {
public:
    struct HandoverPlan {
        double time;
        uint32_t moveFromAToB;
        uint32_t moveFromBToA;
    };

    HandoverController(NodeContainer apNodes, NodeContainer staNodes, NetDeviceContainer staDevices)
        : m_apNodes(apNodes), m_staNodes(staNodes), m_staDevices(staDevices) {
        for (uint32_t i = 0; i < staNodes.GetN(); i++) {
            if (i < 16) {
                m_staToAp[i] = 0;
            } else {
                m_staToAp[i] = 1;
            }
        }

        for (uint32_t i = 0; i < m_apNodes.GetN(); i++) {
            Ptr<WifiNetDevice> apDev = DynamicCast<WifiNetDevice>(m_apNodes.Get(i)->GetDevice(0));
            if (apDev) {
                m_apMacs.push_back(Mac48Address::ConvertFrom(apDev->GetAddress()));
                NS_LOG_INFO("AP " << i << " MAC address: " << m_apMacs[i]);
            }
        }
    }

    void AddHandoverPlan(double time, uint32_t moveFromAToB, uint32_t moveFromBToA) {
        HandoverPlan plan;
        plan.time = time;
        plan.moveFromAToB = moveFromAToB;
        plan.moveFromBToA = moveFromBToA;

        m_handoverPlans.push_back(plan);

        Simulator::Schedule(Seconds(time), &HandoverController::ExecuteHandover, this, moveFromAToB, moveFromBToA);

        NS_LOG_INFO("Scheduled handover at " << time << "s: " << moveFromAToB << " users A->B, "
                    << moveFromBToA << " users B->A");
    }

    std::pair<uint32_t, uint32_t> GetUserDistribution() const {
        uint32_t countA = 0;
        uint32_t countB = 0;

        for (auto const& pair : m_staToAp) {
            if (pair.second == 0) countA++;
            else if (pair.second == 1) countB++;
        }

        return std::make_pair(countA, countB);
    }

    void LogUserDistribution(std::ofstream& os) {
        auto distribution = GetUserDistribution();
        double time = Simulator::Now().GetSeconds();

        os << time << "," << distribution.first << "," << distribution.second << std::endl;

        NS_LOG_INFO("Time " << time << "s: " << distribution.first << " users at AP A, "
                   << distribution.second << " users at AP B");
    }

private:
    void ExecuteHandover(uint32_t moveFromAToB, uint32_t moveFromBToA) {
        NS_LOG_INFO("Executing handover: " << moveFromAToB << " users A->B, " << moveFromBToA << " users B->A");

        std::vector<uint32_t> usersAtA;
        std::vector<uint32_t> usersAtB;

        for (auto const& pair : m_staToAp) {
            if (pair.second == 0) {
                usersAtA.push_back(pair.first);
            } else if (pair.second == 1) {
                usersAtB.push_back(pair.first);
            }
        }

        moveFromAToB = std::min(moveFromAToB, static_cast<uint32_t>(usersAtA.size()));
        moveFromBToA = std::min(moveFromBToA, static_cast<uint32_t>(usersAtB.size()));

        for (uint32_t i = 0; i < moveFromAToB; i++) {
            uint32_t staIndex = usersAtA[i];
            Ptr<WifiNetDevice> staDev = DynamicCast<WifiNetDevice>(m_staDevices.Get(staIndex));

            if (staDev && staDev->GetMac()) {
                staDev->GetMac()->SetBssid(m_apMacs[1], 0);
                m_staToAp[staIndex] = 1;
                NS_LOG_INFO("Station " << staIndex << " moved from AP A to AP B");
            }
        }

        for (uint32_t i = 0; i < moveFromBToA; i++) {
            uint32_t staIndex = usersAtB[i];
            Ptr<WifiNetDevice> staDev = DynamicCast<WifiNetDevice>(m_staDevices.Get(staIndex));

            if (staDev && staDev->GetMac()) {
                staDev->GetMac()->SetBssid(m_apMacs[0], 0);
                m_staToAp[staIndex] = 0;
                NS_LOG_INFO("Station " << staIndex << " moved from AP B to AP A");
            }
        }

        std::ofstream userDist("user_distribution.csv", std::ios_base::app);
        LogUserDistribution(userDist);
    }

    NodeContainer m_apNodes;
    NodeContainer m_staNodes;
    NetDeviceContainer m_staDevices;
    std::map<uint32_t, int> m_staToAp;
    std::vector<Mac48Address> m_apMacs;
    std::vector<HandoverPlan> m_handoverPlans;
};

void MonitorThroughput(std::vector<Ptr<PacketSink>> sinks, 
                       std::ofstream* os,
                       double interval,
                       double simTime,
                       std::map<Ptr<PacketSink>, uint64_t>& lastBytesReceived)
{
    double timeNow = Simulator::Now().GetSeconds();
    *os << timeNow;

    for (uint32_t i = 0; i < sinks.size(); i++) {
        uint64_t currentBytes = sinks[i]->GetTotalRx();
        double throughput = (currentBytes - lastBytesReceived[sinks[i]]) * 8.0 / interval / 1000000;
        lastBytesReceived[sinks[i]] = currentBytes;
        *os << "," << throughput;
    }
    *os << std::endl;

    // Schedule the next throughput measurement
    if (timeNow < simTime) {
        Simulator::Schedule(Seconds(interval), &MonitorThroughput, sinks, os, interval, simTime, std::ref(lastBytesReceived));
    }
}

int main(int argc, char *argv[]) {
    // Enable logging
    LogComponentEnable("WifiHandoverSimulation", LOG_LEVEL_INFO);

    // Configurable simulation parameters
    int ratio = 60;
    double simTime = 3.0 * ratio;
    double firstMigration = 1.0 * ratio;
    double secondMigration = 2.0 * ratio;
    uint32_t firstMoveAtoB = 4;
    uint32_t firstMoveBtoA = 8;
    uint32_t secondMoveAtoB = 10;
    uint32_t secondMoveBtoA = 6;
    double throughputInterval = 1.0;

    // Command line arguments
    CommandLine cmd;
    cmd.AddValue("simTime", "Total simulation time in seconds", simTime);
    cmd.AddValue("firstMigration", "Time of first user migration in seconds", firstMigration);
    cmd.AddValue("secondMigration", "Time of second user migration in seconds", secondMigration);
    cmd.AddValue("firstMoveAtoB", "Users to move A->B in first migration", firstMoveAtoB);
    cmd.AddValue("firstMoveBtoA", "Users to move B->A in first migration", firstMoveBtoA);
    cmd.AddValue("secondMoveAtoB", "Users to move A->B in second migration", secondMoveAtoB);
    cmd.AddValue("secondMoveBtoA", "Users to move B->A in second migration", secondMoveBtoA);
    cmd.Parse(argc, argv);

    // Print simulation parameters
    NS_LOG_INFO("Simulation parameters:");
    NS_LOG_INFO("- Total simulation time: " << simTime << " seconds");
    NS_LOG_INFO("- First migration at " << firstMigration << "s: "
                << firstMoveAtoB << " users A->B, " << firstMoveBtoA << " users B->A");
    NS_LOG_INFO("- Second migration at " << secondMigration << "s: "
                << secondMoveAtoB << " users A->B, " << secondMoveBtoA << " users B->A");

    uint32_t nAps = 2;
    uint32_t totalStations = 32;

    NS_LOG_INFO("Creating topology with " << nAps << " APs and " << totalStations << " stations");

    // Create nodes
    NodeContainer apNodes;
    apNodes.Create(nAps);
    NodeContainer staNodes;
    staNodes.Create(totalStations);

    // Create wifi devices
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211n);

    // Set up PHY
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    // Configure AP
    WifiMacHelper mac;
    Ssid ssid = Ssid("wifi-handover-network");
    mac.SetType("ns3::ApWifiMac",
               "Ssid", SsidValue(ssid),
               "BeaconGeneration", BooleanValue(true),
               "BeaconInterval", TimeValue(MicroSeconds(102400)));

    // Install on AP nodes
    NetDeviceContainer apDevices = wifi.Install(phy, mac, apNodes);

    // STA configuration
    mac.SetType("ns3::StaWifiMac",
               "Ssid", SsidValue(ssid),
               "ActiveProbing", BooleanValue(false));

    // Install on STA nodes
    NetDeviceContainer staDevices = wifi.Install(phy, mac, staNodes);

    // Set mobility model
    MobilityHelper mobility;

    // Position APs
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    for (uint32_t i = 0; i < nAps; i++) {
        positionAlloc->Add(Vector(i * 50.0, 0.0, 0.0));
    }
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(apNodes);

    // Random walk for stations
    mobility.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
                                 "X", DoubleValue(25.0),
                                 "Y", DoubleValue(0.0),
                                 "Rho", StringValue("ns3::UniformRandomVariable[Min=0|Max=30]"));
    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue(Rectangle(-50, 100, -50, 50)),
                             "Speed", StringValue("ns3::ConstantRandomVariable[Constant=2.0]"));
    mobility.Install(staNodes);

    // Install Internet stack
    InternetStackHelper stack;
    stack.Install(apNodes);
    stack.Install(staNodes);

    // Assign IP addresses
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    Ipv4InterfaceContainer apInterfaces = address.Assign(apDevices);
    Ipv4InterfaceContainer staInterfaces = address.Assign(staDevices);

    // Setup servers on APs
    uint16_t port = 9;

    // Create packet sinks on all APs
    std::vector<Ptr<PacketSink>> sinks;
    for (uint32_t i = 0; i < nAps; i++) {
        PacketSinkHelper packetSinkHelper("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
        ApplicationContainer sinkApps = packetSinkHelper.Install(apNodes.Get(i));

        Ptr<PacketSink> sink = DynamicCast<PacketSink>(sinkApps.Get(0));
        sink->SetStartTime(Seconds(0.0));
        sink->SetStopTime(Seconds(simTime));

        sinks.push_back(sink);
    }

    // Full queue traffic generation
    OnOffHelper onoff("ns3::UdpSocketFactory", Address());
    onoff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    onoff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    onoff.SetAttribute("DataRate", DataRateValue(DataRate("1Mbps")));
    onoff.SetAttribute("PacketSize", UintegerValue(1024));

    // Install applications on all stations
    ApplicationContainer clientApps;

    // First 16 stations send to AP A
    for (uint32_t i = 0; i < 16; i++) {
        AddressValue remoteAddress(InetSocketAddress(apInterfaces.GetAddress(0), port));
        onoff.SetAttribute("Remote", remoteAddress);
        clientApps.Add(onoff.Install(staNodes.Get(i)));
    }

    // Next 16 stations send to AP B
    for (uint32_t i = 16; i < 32; i++) {
        AddressValue remoteAddress(InetSocketAddress(apInterfaces.GetAddress(1), port));
        onoff.SetAttribute("Remote", remoteAddress);
        clientApps.Add(onoff.Install(staNodes.Get(i)));
    }

    clientApps.Start(Seconds(1.0));
    clientApps.Stop(Seconds(simTime - 1));

    // Create handover controller
    HandoverController handoverController(apNodes, staNodes, staDevices);

    // Initialize output files
    std::ofstream throughputFile("throughput.csv");
    throughputFile << "Time,ThroughputA_Mbps,ThroughputB_Mbps" << std::endl;

    std::ofstream userDistFile("user_distribution.csv");
    userDistFile << "Time,UsersAtA,UsersAtB" << std::endl;
    userDistFile << "0,16,16" << std::endl;

    // Schedule handovers
    handoverController.AddHandoverPlan(firstMigration, firstMoveAtoB, firstMoveBtoA);
    handoverController.AddHandoverPlan(secondMigration, secondMoveAtoB, secondMoveBtoA);

    // Monitor throughput
    std::map<Ptr<PacketSink>, uint64_t> lastBytesReceived;
    for (auto sink : sinks) {
        lastBytesReceived[sink] = 0;
    }

    // Initial scheduling of throughput monitoring
    Simulator::Schedule(Seconds(1.0), &MonitorThroughput, sinks, &throughputFile, throughputInterval, simTime, std::ref(lastBytesReceived));

    // Run simulation
    NS_LOG_INFO("Starting simulation for " << simTime << " seconds");
    Simulator::Stop(Seconds(simTime));
    Simulator::Run();

    // Print final statistics
    for (uint32_t i = 0; i < nAps; i++) {
        NS_LOG_INFO("Total bytes received at AP " << i << ": " << sinks[i]->GetTotalRx());
    }

    NS_LOG_INFO("Simulation complete");
    Simulator::Destroy();
    return 0;
}
