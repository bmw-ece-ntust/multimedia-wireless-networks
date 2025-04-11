#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include <fstream>
#include <map>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("FullQueueMobility");

// 儲存使用者群組
std::vector<Ptr<Node>> staGroupA;
std::vector<Ptr<Node>> staGroupB;
std::map<uint32_t, std::string> nodeIdToGroup;
std::map<uint32_t, Ptr<Node>> nodeMap;
std::ofstream out("throughput.csv");
std::map<uint32_t, int> StaToApMap;

std::vector<double> throughputA_record, throughputB_record;

// 使用者在 A/B 間交換
void SwapUsers(std::vector<Ptr<Node>> &A, std::vector<Ptr<Node>> &B, double p, double q) {
  uint32_t moveAtoB = std::round(A.size() * p);
  uint32_t moveBtoA = std::round(B.size() * q);
  std::vector<Ptr<Node>> fromA(A.begin(), A.begin() + moveAtoB);
  std::vector<Ptr<Node>> fromB(B.begin(), B.begin() + moveBtoA);
  A.erase(A.begin(), A.begin() + moveAtoB);
  B.erase(B.begin(), B.begin() + moveBtoA);
  A.insert(A.end(), fromB.begin(), fromB.end());
  B.insert(B.end(), fromA.begin(), fromA.end());
}

// 印出目前 A/B 人數
void PrintUserTable() {
  NS_LOG_UNCOND("Time: " << Simulator::Now().GetSeconds() << "s");
  NS_LOG_UNCOND("Node A: " << staGroupA.size() << " users");
  NS_LOG_UNCOND("Node B: " << staGroupB.size() << " users");
}

// 建立使用者對應表
void TagNodes() {
  for (auto node : staGroupA) {
    nodeIdToGroup[node->GetId()] = "A";
    nodeMap[node->GetId()] = node;
  }
  for (auto node : staGroupB) {
    nodeIdToGroup[node->GetId()] = "B";
    nodeMap[node->GetId()] = node;
  }
}

// 每秒記錄 throughput + UE 數量
void CheckThroughput(Ptr<FlowMonitor> monitor, Ptr<Ipv4FlowClassifier> classifier) {
  monitor->CheckForLostPackets();
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();

  double throughputA = 0, throughputB = 0;
  double now = Simulator::Now().GetSeconds();

  for (auto& flow : stats) {
    Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(flow.first);
  
    uint64_t bits = flow.second.rxBytes * 8;
    double duration = flow.second.timeLastRxPacket.GetSeconds() - flow.second.timeFirstTxPacket.GetSeconds();
    double tp = (duration > 0) ? bits / duration / 1e6 : 0;

    for (auto& flow : stats) {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(flow.first);
  
      uint64_t bits = flow.second.rxBytes * 8;
      double duration = flow.second.timeLastRxPacket.GetSeconds() - flow.second.timeFirstTxPacket.GetSeconds();
      double tp = (duration > 0) ? bits / duration / 1e6 : 0;
  
      if (t.destinationAddress == Ipv4Address("10.1.1.1")){
        throughputA += tp;
      }
      else if (t.destinationAddress == Ipv4Address("10.1.1.2")){
        throughputB += tp;
      }
    }
  }

  throughputA_record.push_back(throughputA);
  throughputB_record.push_back(throughputB);

  out << now << "," << throughputA << "," << throughputB << "," << staGroupA.size() << "," << staGroupB.size() << "\n";

  if (now < 60.0) {
    Simulator::Schedule(Seconds(1.0), &CheckThroughput, monitor, classifier);
  } else {
    double avgA = 0, avgB = 0;
    for (double val : throughputA_record) avgA += val;
    for (double val : throughputB_record) avgB += val;
    avgA /= throughputA_record.size();
    avgB /= throughputB_record.size();

    std::cout << "[Result] Avg Throughput A = " << avgA << " Mbps" << std::endl;
    std::cout << "[Result] Avg Throughput B = " << avgB << " Mbps" << std::endl;
  }
}
void StopAllClients() {
    for (auto node : staGroupA) {
      for (uint32_t i = 0; i < node->GetNApplications(); ++i) {
        node->GetApplication(i)->SetStopTime(Seconds(Simulator::Now().GetSeconds() + 0.001));
      }
    }
    for (auto node : staGroupB) {
      for (uint32_t i = 0; i < node->GetNApplications(); ++i) {
        node->GetApplication(i)->SetStopTime(Seconds(Simulator::Now().GetSeconds() + 0.001));
      }
    }
  }
  
// Uplink to AP  
void ReconnectClients(Ipv4Address apA, Ipv4Address apB) {
  UdpEchoClientHelper clientA(apA, 9);
  clientA.SetAttribute("MaxPackets", UintegerValue(1000000));
  clientA.SetAttribute("Interval", TimeValue(MilliSeconds(10)));
  clientA.SetAttribute("PacketSize", UintegerValue(1024));

  UdpEchoClientHelper clientB(apB, 9);
  clientB.SetAttribute("MaxPackets", UintegerValue(1000000));
  clientB.SetAttribute("Interval", TimeValue(MilliSeconds(10)));
  clientB.SetAttribute("PacketSize", UintegerValue(1024));

  for (auto node : staGroupA) {
    uint32_t staId = node->GetId();
    if (StaToApMap.find(staId) == StaToApMap.end() || StaToApMap[staId] == -1) {
      std::cout << "Initial STA " << staId << " to AP" << (32) << " at " << Simulator::Now().GetSeconds() << "s\n";
    } else if (StaToApMap[staId] != 0) {
      std::cout << "STA " << staId << " handover from AP" << StaToApMap[staId] + 32 << " to AP" << 32 << " at " << Simulator::Now().GetSeconds() << "s\n";
    }
    StaToApMap[staId] = 0;
    ApplicationContainer app = clientA.Install(node);
    app.Start(Simulator::Now());
    
  }

  for (auto node : staGroupB) {
    uint32_t staId = node->GetId();
    if (StaToApMap.find(staId) == StaToApMap.end() || StaToApMap[staId] == -1) {
      std::cout << "Initial STA " << staId << " to AP" << (33) << " at " << Simulator::Now().GetSeconds() << "s\n";
    } else if (StaToApMap[staId] != 1) {
      std::cout << "STA " << staId << " handover from AP" << StaToApMap[staId] + 32 << " to AP" << 33 << " at " << Simulator::Now().GetSeconds() << "s\n";
    }
    StaToApMap[staId] = 1;
    ApplicationContainer app = clientB.Install(node);
    app.Start(Simulator::Now());

  }
}

void PrintUserTableAtStart(NetDeviceContainer staDevs, NetDeviceContainer apDevs) {
    std::cout << "\n=== [Initial STA-to-AP Assignment Table] ===\n";
    std::cout << "STA ID\tSTA MAC\t\t\t→ AP MAC\n";
  
    for (uint32_t i = 0; i < staDevs.GetN(); ++i) {
      Ptr<NetDevice> staDev = staDevs.Get(i);
      Mac48Address staMac = Mac48Address::ConvertFrom(staDev->GetAddress());
  
      std::ostringstream ossSta;
      ossSta << staMac;
      std::string staMacStr = ossSta.str();
  
      Ptr<Node> staNode = staDev->GetNode();
      uint32_t staId = staNode->GetId();
  
      std::string apMacStr = "Unknown";
      if (StaToApMap.find(staId) != StaToApMap.end()) {
        int apIndex = StaToApMap[staId];
        if (apIndex >= 0 && apIndex < (int)apDevs.GetN()) {
          Mac48Address apMac = Mac48Address::ConvertFrom(apDevs.Get(apIndex)->GetAddress());
          std::ostringstream ossAp;
          ossAp << apMac;
          apMacStr = ossAp.str();
        }
      }
  
      std::cout << staId << "\t" << staMacStr << " → " << apMacStr << "\n";
    }
  
    std::cout << "===========================================\n";
  }
    
int main(int argc, char *argv[]) {
  NodeContainer aps;
  aps.Create(2);

  NodeContainer allStas;
  allStas.Create(32);
  for (uint32_t i = 0; i < 16; ++i) staGroupA.push_back(allStas.Get(i));
  for (uint32_t i = 16; i < 32; ++i) staGroupB.push_back(allStas.Get(i));

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
  YansWifiPhyHelper phy;
  phy.SetChannel(channel.Create());

  WifiHelper wifi;
  wifi.SetStandard(WIFI_STANDARD_80211g);
  WifiMacHelper mac;
  NetDeviceContainer staDevs;

  for (auto node : staGroupA) {
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(Ssid("A")));
    staDevs.Add(wifi.Install(phy, mac, node));
  }
  for (auto node : staGroupB) {
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(Ssid("B")));
    staDevs.Add(wifi.Install(phy, mac, node));
  }

  mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(Ssid("A")));
  NetDeviceContainer apDevs;
  apDevs.Add(wifi.Install(phy, mac, aps.Get(0)));

  mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(Ssid("B")));
  apDevs.Add(wifi.Install(phy, mac, aps.Get(1)));

  MobilityHelper mobility;
  //Set Position for Ap
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel"); 
  mobility.Install(aps);
  // Set random position for Sta
  mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel"); 
  mobility.Install(allStas);

  InternetStackHelper stack;
  stack.Install(aps);
  stack.Install(allStas);

  Ipv4AddressHelper addr;
  addr.SetBase("10.1.1.0", "255.255.255.0");
  addr.Assign(apDevs);
  addr.Assign(staDevs);

  // FULL-QUEUE 模型：每個 STA 都會持續送封包
  UdpEchoServerHelper server(9); //9 是這個 Echo Server 要監聽的 UDP 埠號
  ApplicationContainer serverA = server.Install(aps.Get(0));
  ApplicationContainer serverB = server.Install(aps.Get(1));  // Node B 的 AP

  serverA.Start(Seconds(0.0));
  serverB.Start(Seconds(0.0));

  Ipv4Address apA = Ipv4Address("10.1.1.1");
  Ipv4Address apB = Ipv4Address("10.1.1.2");

  UdpEchoClientHelper clientA(apA, 9); // STA A
  clientA.SetAttribute("MaxPackets", UintegerValue(1000000)); //Max throughput: 100(一秒有幾個)x 1024bytes x 8 x 16個
  clientA.SetAttribute("Interval", TimeValue(MilliSeconds(10)));
  clientA.SetAttribute("PacketSize", UintegerValue(1024));

  UdpEchoClientHelper clientB(apB, 9); // STA B
  clientB.SetAttribute("MaxPackets", UintegerValue(1000000));
  clientB.SetAttribute("Interval", TimeValue(MilliSeconds(10)));
  clientB.SetAttribute("PacketSize", UintegerValue(1024));

  for (auto node : staGroupA) {
    ApplicationContainer app = clientA.Install(node);
    app.Start(Seconds(1.0));
    StaToApMap[node->GetId()] = 0;  // 初始化 STA 對應 AP
  }
  
  for (auto node : staGroupB) {
    ApplicationContainer app = clientB.Install(node);
    app.Start(Seconds(1.0));
    StaToApMap[node->GetId()] = 1;
  }
  
  
  // 初始化節點分組資訊
  TagNodes();

  // 建立 FlowMonitor 與分類器
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());

  out << "Time,NodeA_Throughput_Mbps,NodeB_Throughput_Mbps,NumUE_A,NumUE_B\n";
  PrintUserTableAtStart(staDevs, apDevs);

  Simulator::Schedule(Seconds(1.0), &CheckThroughput, monitor, classifier);

  // 使用者移動事件
//   Simulator::Schedule(Seconds(20.0), &SwapUsers, std::ref(staGroupA), std::ref(staGroupB), 0.25, 0.5);
//   Simulator::Schedule(Seconds(20.1), &PrintUserTable);
//   Simulator::Schedule(Seconds(40.0), &SwapUsers, std::ref(staGroupA), std::ref(staGroupB), 0.25, 0.5);
//   Simulator::Schedule(Seconds(40.1), &PrintUserTable);
  Simulator::Schedule(Seconds(20.0), &SwapUsers, std::ref(staGroupA), std::ref(staGroupB), 0.25, 0.5);
  Simulator::Schedule(Seconds(20.1), &StopAllClients);
  Simulator::Schedule(Seconds(20.2), &ReconnectClients, apA, apB);
  Simulator::Schedule(Seconds(20.3), &PrintUserTable);
  Simulator::Schedule(Seconds(40.0), &SwapUsers, std::ref(staGroupA), std::ref(staGroupB), 0.5, 0.5);
  Simulator::Schedule(Seconds(40.1), &StopAllClients);
  Simulator::Schedule(Seconds(40.2), &ReconnectClients, apA, apB);
  Simulator::Schedule(Seconds(40.3), &PrintUserTable);

  Simulator::Stop(Seconds(60.0));
  Simulator::Run();
  Simulator::Destroy();
  out.close();
  return 0;
}
