#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/wifi-net-device.h"
#include "ns3/ap-wifi-mac.h"
#include "ns3/sta-wifi-mac.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/packet-sink.h"
#include "ns3/ipv4-header.h"
#include "ns3/udp-header.h"
#include "ns3/log.h"
#include "ns3/inet-socket-address.h"
// Need YansWifiPhyHelper and WifiMacHelper globally or passed
#include "ns3/yans-wifi-helper.h"


#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <set> // Keep for separated movement logic

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TaskMobilitySim");


// --- Global Variables & Setup ---
double throughputIntervalSeconds = 1.0; std::ofstream throughputLogFile;
std::map<uint32_t, uint64_t> bytesReceivedInInterval;
// *** Use a SINGLE SSID ***
Ssid sharedSsid = Ssid("MyWiFiNetwork"); // Single SSID for both APs


struct UserInfo {
    Ptr<Node> node; uint32_t id; int initialAp; // Store initial AP (0 or 1)
    Ptr<NetDevice> wifiDevice;
    // No longer need clientApp pointer here, installed directly
    Ptr<MobilityModel> mobility; Ipv4Address ipAddr;
};
std::map<uint32_t, UserInfo> userDatabase; // Used mainly to store IP and initial state
uint32_t initialUsersPerNode = 16;
Vector apPositionA = Vector(0.0, 0.0, 0.0); Vector apPositionB = Vector(100.0, 0.0, 0.0);

// --- Callbacks and Helpers ---
// ApSinkRxCallback (unchanged)
static void ApSinkRxCallback (std::string c, Ptr<const Packet> p, const Address &a) {
    uint32_t ps=p->GetSize(); Ipv4Address sip=Ipv4Address::GetAny(); Ptr<Packet> pCopy=p->Copy(); Ipv4Header ipH;
    if(pCopy->PeekHeader(ipH)){sip=ipH.GetSource();} if(sip==Ipv4Address::GetAny()||sip.IsBroadcast()||sip.IsMulticast()||sip.IsLocalMulticast()){InetSocketAddress tAddr=InetSocketAddress::ConvertFrom(a);sip=tAddr.GetIpv4();}
    uint32_t rxId=(uint32_t)-1; if(!c.empty()){size_t p1=c.find("/NodeList/");size_t p2=c.find('/',p1+10);if(p1!=std::string::npos&&p2!=std::string::npos){p1+=10;std::string nIdStr=c.substr(p1,p2-p1);try{rxId=std::stoul(nIdStr);}catch(...){rxId=(uint32_t)-1;}}}
    if(rxId<=1){NS_LOG_INFO("[AP_RxTrace] Time="<<Simulator::Now().GetSeconds()<<"s, AP_NodeID="<<rxId<<", RxFromSTA_IP="<<sip<<", Size="<<ps);bytesReceivedInInterval[rxId]+=ps;}
    else if(rxId!=(uint32_t)-1){NS_LOG_WARN("ApSinkRxCallback: Rx@unexpected NodeID "<<rxId<<" ctx: "<<c);}
}
// CalculateThroughput (unchanged)
void CalculateThroughput () { Time now=Simulator::Now();double tS=now.GetSeconds();double tA=(bytesReceivedInInterval.count(0)?(bytesReceivedInInterval[0]*8.0)/throughputIntervalSeconds:0.0);bytesReceivedInInterval[0]=0;double tB=(bytesReceivedInInterval.count(1)?(bytesReceivedInInterval[1]*8.0)/throughputIntervalSeconds:0.0);bytesReceivedInInterval[1]=0;if(throughputLogFile.is_open()){throughputLogFile<<tS<<","<<tA/1e6<<","<<tB/1e6<<std::endl;}Simulator::Schedule(Seconds(throughputIntervalSeconds),&CalculateThroughput); }

// Simplified PrintUserTable - only shows initial assignment
void PrintUserTable(Time now) {
     std::cout << "--- User Initial Assignment Table (at " << now.GetSeconds() << "s) ---\n";
     std::cout << "ID|Initial AP|IP\n";
     std::cout << "-----------------------------------------\n";
     std::vector<uint32_t> ids; for(auto const& [id, i]:userDatabase){ids.push_back(id);}
     std::sort(ids.begin(), ids.end());
     for(const auto& id : ids) {
         const UserInfo& i = userDatabase[id];
         std::cout << std::setw(2) << i.id << "|" << std::setw(10) << (i.initialAp == 0 ? 'A' : 'B') << "|" << i.ipAddr << std::endl;
     }
     std::cout << "-----------------------------------------\n";
}

// Generates random position
// *** RESTORED IMPLEMENTATION ***
Vector GetRandomPositionNear(Vector center, double range)
{
    Ptr<UniformRandomVariable> rX=CreateObject<UniformRandomVariable>();rX->SetAttribute("Min",DoubleValue(center.x-range));rX->SetAttribute("Max",DoubleValue(center.x+range));
    Ptr<UniformRandomVariable> rY=CreateObject<UniformRandomVariable>();rY->SetAttribute("Min",DoubleValue(center.y-range));rY->SetAttribute("Max",DoubleValue(center.y+range));
    return Vector(rX->GetValue(),rY->GetValue(),center.z); // Added return
}


// ADAPTED Mobility Scheduling Function from Example
// Only moves nodes physically, no MAC/App changes
void ScheduleStaMobilityEvents(NodeContainer staNodes, uint32_t nWifi,
                              uint32_t movingFromAp1, uint32_t movingFromAp2,
                              uint32_t ap1Offset = 0, uint32_t ap2Offset = 0)
{
  NS_LOG_INFO("== Scheduling STA physical moves at " << Simulator::Now().GetSeconds() << "s ==");
  std::cout << "\n== Moving STAs physically at " << Simulator::Now().GetSeconds() << "s ==\n";

  if (staNodes.GetN() < (ap1Offset + movingFromAp1) || staNodes.GetN() < (nWifi + ap2Offset + movingFromAp2) )
  {
    NS_LOG_WARN("Not enough STA nodes (" << staNodes.GetN() << ") for requested movement index ranges.");
    return;
  }

  // Move from AP1 area to AP2 area
  std::cout << "Moving " << movingFromAp1 << " STAs physically from AP1 area -> AP2 area:\n";
  for (uint32_t i = ap1Offset; i < ap1Offset + movingFromAp1; ++i)
  {
    if (i >= staNodes.GetN()) continue; // Boundary check
    Ptr<MobilityModel> mobility = staNodes.Get(i)->GetObject<MobilityModel>();
    Vector target = GetRandomPositionNear(apPositionB, 5.0); // Move near AP B
    mobility->SetPosition(target);
    NS_LOG_INFO("  -> STA node " << i << " moved near AP B. Pos: " << target);
    std::cout << "  - STA node " << i << " moved near AP B.\n";
  }

  // Move from AP2 area to AP1 area
  std::cout << "Moving " << movingFromAp2 << " STAs physically from AP2 area -> AP1 area:\n";
  for (uint32_t i = nWifi + ap2Offset; i < nWifi + ap2Offset + movingFromAp2; ++i)
  {
     if (i >= staNodes.GetN()) continue; // Boundary check
    Ptr<MobilityModel> mobility = staNodes.Get(i)->GetObject<MobilityModel>();
    Vector target = GetRandomPositionNear(apPositionA, 5.0); // Move near AP A
    mobility->SetPosition(target);
    NS_LOG_INFO("  -> STA node " << i << " moved near AP A. Pos: " << target);
     std::cout << "  - STA node " << i << " moved near AP A.\n";
  }
}

// --- Main Function ---
int main(int argc, char *argv[]) {
    // --- Parameters ---
    double simulationTime = 180; double moveTime1 = 60; double moveTime2 = 120;
    double percentAtoB_M1=0.25; double percentBtoA_M1=0.50; double percentAtoB_M2=0.50; double percentBtoA_M2=0.50;
    double initialPositionRange=10.0; /* movementPositionRange unused */ std::string throughputLogFilename="aggregated_throughput.txt";
    bool enableFlowMonitor=true; bool enablePcap=false; bool enableAscii=false; bool enableNetAnim=false; double clientStartTime=3.0;

    CommandLine cmd(__FILE__); cmd.AddValue("logFile", "File name", throughputLogFilename);
    cmd.AddValue("pcap", "Enable PCAP", enablePcap); cmd.AddValue("ascii", "Enable ASCII", enableAscii);
    cmd.AddValue("flowmon", "Enable FlowMon", enableFlowMonitor); cmd.AddValue("netanim", "Enable NetAnim", enableNetAnim);
    cmd.Parse(argc, argv);

    Time::SetResolution(Time::NS);
    LogComponentEnable("TaskMobilitySim", LOG_LEVEL_INFO);
    LogComponentEnable("StaWifiMac", LOG_LEVEL_INFO); LogComponentEnable("ApWifiMac", LOG_LEVEL_INFO); LogComponentEnable("UdpClient", LOG_LEVEL_INFO);

    throughputLogFile.open(throughputLogFilename);
    if(!throughputLogFile.is_open()){NS_LOG_ERROR("Could not open file: "<<throughputLogFilename);return 1;}
    throughputLogFile<<"Time_s,Throughput_AP_A_Mbps,Throughput_AP_B_Mbps"<<std::endl;

    // --- Node Creation ---
    NodeContainer apNodes; apNodes.Create(2); NodeContainer staNodes; staNodes.Create(initialUsersPerNode*2);

    // --- WiFi Setup ---
    WifiHelper wifi; wifi.SetStandard(WIFI_STANDARD_80211n); wifi.SetRemoteStationManager("ns3::IdealWifiManager");
    YansWifiChannelHelper channel=YansWifiChannelHelper::Default(); YansWifiPhyHelper phy; phy.SetChannel(channel.Create());
    WifiMacHelper mac; NetDeviceContainer apDevices, staDevices;

    // APs use SHARED SSID, QoS Disabled
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(sharedSsid), "QosSupported", BooleanValue(false));
    apDevices.Add(wifi.Install(phy, mac, apNodes.Get(0)));
    apDevices.Add(wifi.Install(phy, mac, apNodes.Get(1)));

    // --- Mobility Setup ---
    MobilityHelper mobility; Ptr<ListPositionAllocator> apPosAlloc = CreateObject<ListPositionAllocator>();
    apPosAlloc->Add(Vector(0.0, 0.0, 0.0)); apPosAlloc->Add(Vector(100.0, 0.0, 0.0)); mobility.SetPositionAllocator(apPosAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel"); mobility.Install(apNodes);
    apPositionA = apNodes.Get(0)->GetObject<MobilityModel>()->GetPosition(); apPositionB = apNodes.Get(1)->GetObject<MobilityModel>()->GetPosition();

    // --- Install STA MAC Layers and Mobility ---
    MobilityHelper staMobility; staMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    for (uint32_t i = 0; i < staNodes.GetN(); ++i) {
        Ptr<Node> staNode=staNodes.Get(i); UserInfo info; info.node=staNode; info.id=i;
        // STAs use SHARED SSID, QoS Disabled, Active Probing
        mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(sharedSsid), "ActiveProbing", BooleanValue(true), "QosSupported", BooleanValue(false));
        info.initialAp=(i<initialUsersPerNode)?0:1;
        info.wifiDevice=wifi.Install(phy, mac, staNode).Get(0); staDevices.Add(info.wifiDevice);
        Vector initialPos=(info.initialAp==0)?GetRandomPositionNear(apPositionA, initialPositionRange):GetRandomPositionNear(apPositionB, initialPositionRange);
        Ptr<ListPositionAllocator> singlePosAlloc=CreateObject<ListPositionAllocator>(); singlePosAlloc->Add(initialPos); staMobility.SetPositionAllocator(singlePosAlloc); staMobility.Install(info.node);
        info.mobility=info.node->GetObject<MobilityModel>();
        userDatabase[info.id]=info;
    }

    // --- Internet Stack & IP Addressing ---
    InternetStackHelper stack; stack.Install(apNodes); stack.Install(staNodes);
    Ipv4AddressHelper address; address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer apInterfaces=address.Assign(apDevices); Ipv4InterfaceContainer staInterfaces=address.Assign(staDevices);
    for(uint32_t i=0; i<staNodes.GetN(); ++i){userDatabase[i].ipAddr = staInterfaces.GetAddress(i);}
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // Print initial table once
    Simulator::Schedule(Seconds(0.1), &PrintUserTable, Simulator::Now());

    // --- Applications ---
    uint16_t sinkPort=9; ApplicationContainer serverApps;
    PacketSinkHelper sinkHelper("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), sinkPort));
    Ptr<PacketSink> sinkA=StaticCast<PacketSink>(sinkHelper.Install(apNodes.Get(0)).Get(0)); sinkA->TraceConnect("Rx", "/NodeList/0/ApplicationList/0/$ns3::PacketSink/Rx", MakeCallback(&ApSinkRxCallback)); serverApps.Add(sinkA);
    Ptr<PacketSink> sinkB=StaticCast<PacketSink>(sinkHelper.Install(apNodes.Get(1)).Get(0)); sinkB->TraceConnect("Rx", "/NodeList/1/ApplicationList/0/$ns3::PacketSink/Rx", MakeCallback(&ApSinkRxCallback)); serverApps.Add(sinkB);

    // Install TWO clients per STA
    ApplicationContainer clientAppsToA, clientAppsToB;
    DataRate dataRate("1Mbps"); uint32_t packetSize=1400; Time interPacketInterval=Seconds(packetSize*8.0/dataRate.GetBitRate());
    UdpClientHelper clientHelperToA(apInterfaces.GetAddress(0), sinkPort); clientHelperToA.SetAttribute("MaxPackets", UintegerValue(0)); clientHelperToA.SetAttribute("Interval", TimeValue(interPacketInterval)); clientHelperToA.SetAttribute("PacketSize", UintegerValue(packetSize));
    UdpClientHelper clientHelperToB(apInterfaces.GetAddress(1), sinkPort); clientHelperToB.SetAttribute("MaxPackets", UintegerValue(0)); clientHelperToB.SetAttribute("Interval", TimeValue(interPacketInterval)); clientHelperToB.SetAttribute("PacketSize", UintegerValue(packetSize));
    for(uint32_t i = 0; i < staNodes.GetN(); ++i) { clientAppsToA.Add(clientHelperToA.Install(staNodes.Get(i))); clientAppsToB.Add(clientHelperToB.Install(staNodes.Get(i))); }

    // Start/Stop applications
    serverApps.Start(Seconds(1.0)); serverApps.Stop(Seconds(simulationTime + 1.0));
    clientAppsToA.Start(Seconds(clientStartTime)); clientAppsToA.Stop(Seconds(simulationTime));
    clientAppsToB.Start(Seconds(clientStartTime)); clientAppsToB.Stop(Seconds(simulationTime));

    // --- Scheduling Mobility Events ---
    uint32_t move1_10s = initialUsersPerNode * percentAtoB_M1; uint32_t move2_10s = initialUsersPerNode * percentBtoA_M1;
    Simulator::Schedule(Seconds(moveTime1), &ScheduleStaMobilityEvents, staNodes, initialUsersPerNode, move1_10s, move2_10s, 0, 0);
    uint32_t move1_20s = (initialUsersPerNode-initialUsersPerNode* percentAtoB_M1+initialUsersPerNode * percentBtoA_M1) * percentAtoB_M2; uint32_t move2_20s = (initialUsersPerNode-initialUsersPerNode* percentBtoA_M1+initialUsersPerNode* percentAtoB_M1) * percentBtoA_M2;
    Simulator::Schedule(Seconds(moveTime2), &ScheduleStaMobilityEvents, staNodes, initialUsersPerNode, move1_20s, move2_20s, move1_10s, move2_10s);

    Simulator::Schedule(Seconds(1.0 + throughputIntervalSeconds), &CalculateThroughput);

    // --- Tracing / Monitoring ---
    Ptr<FlowMonitor> monitor; FlowMonitorHelper flowmon; AnimationInterface* anim = nullptr;
    if(enablePcap){phy.EnablePcapAll("task-mobility-sim", true);} if(enableAscii){AsciiTraceHelper ascii; phy.EnableAsciiAll(ascii.CreateFileStream("task-mobility-sim.tr"));}
    if(enableFlowMonitor){monitor=flowmon.InstallAll();} if(enableNetAnim){anim=new AnimationInterface("task-mobility-sim.xml");/*...NetAnim setup...*/ }

    // --- Run Simulation ---
    NS_LOG_INFO("Starting Simulation for " << simulationTime << " seconds...");
    Simulator::Stop(Seconds(simulationTime + 1.0)); Simulator::Run(); NS_LOG_INFO("Simulation Finished.");

    // --- Final Analysis & Cleanup ---
    std::cout << "\n===== Simulation Ended =====" << std::endl;
    Ptr<PacketSink> sink1Ptr = DynamicCast<PacketSink>(serverApps.Get(0)); Ptr<PacketSink> sink2Ptr = DynamicCast<PacketSink>(serverApps.Get(1));
    std::cout << "AP A (Node 0) Total Received: " << (sink1Ptr ? sink1Ptr->GetTotalRx() : 0) << " bytes\n"; std::cout << "AP B (Node 1) Total Received: " << (sink2Ptr ? sink2Ptr->GetTotalRx() : 0) << " bytes\n";
    if(enableFlowMonitor && monitor){monitor->SerializeToXmlFile("task-mobility-sim-flowmon.xml", true, true);}
    if(throughputLogFile.is_open()){throughputLogFile.close(); NS_LOG_INFO("Aggregated throughput data saved to " << throughputLogFilename);}
    if(anim){delete anim;}
    Simulator::Destroy();
    NS_LOG_INFO("Done.");
    return 0;
}