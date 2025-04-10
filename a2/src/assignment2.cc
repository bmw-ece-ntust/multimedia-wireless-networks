#include <ns3/core-module.h>
#include "ns3/internet-module.h"
#include <ns3/network-module.h>
#include <ns3/mobility-module.h>
#include <ns3/wifi-module.h>
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/applications-module.h"

#include <fstream>

using namespace ns3;

std::ofstream throughputLog("throughput.csv");
std::ofstream ueLog("ue_count.csv");
static std::map<FlowId, uint64_t> lastRxBytes;

void MoveUsers(NodeContainer& ueNodes, std::vector<int> moveAtoB, std::vector<int> moveBtoA, double xA, double xB) {
    Ptr<MobilityModel> mobility;
    for (int i : moveAtoB) {
        mobility = ueNodes.Get(i)->GetObject<MobilityModel>();
        mobility->SetPosition(Vector(200 - (17*(i%4) + 5), 17*(int((i+16)/4)) + 10 - 61, 0));  // Move to Node B
    }
    for (int i : moveBtoA) {
        mobility = ueNodes.Get(i)->GetObject<MobilityModel>();
        mobility->SetPosition(Vector((17*(i%4)+5)*(-1) - 10, 17*(int((i+16)/4))+5 - 120, 0));  // Move to Node A
    }
}

void LogStats(FlowMonitorHelper* flowHelper, Ptr<FlowMonitor> monitor, NodeContainer ueNodes,
              double xA, double xB, double range, double interval) {
    monitor->CheckForLostPackets();
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();

    double ap0_rx = 0, ap1_rx = 0;

    for (auto& stat : stats) {
        Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowHelper->GetClassifier());
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(stat.first);
        uint64_t last = lastRxBytes[stat.first];
        uint64_t current = stat.second.rxBytes;
        double deltaBits = (current - last) * 8.0;

        lastRxBytes[stat.first] = current;
        if (t.destinationAddress == Ipv4Address("192.168.1.1"))
            ap0_rx += deltaBits / interval / 1e6;
        else
            ap1_rx += deltaBits / interval / 1e6;
    }

    int ap0_ue = 0, ap1_ue = 0;
    for (uint32_t i = 0; i < ueNodes.GetN(); ++i) {
        Vector pos = ueNodes.Get(i)->GetObject<MobilityModel>()->GetPosition();
        if (std::abs(pos.x - xA) < range)
            ap0_ue++;
        else if (std::abs(pos.x - xB) < range)
            ap1_ue++;
    }

    double time = Simulator::Now().GetSeconds();
    throughputLog << time << "," << ap0_rx << "," << ap1_rx << "\n";
    ueLog << time << "," << ap0_ue << "," << ap1_ue << "\n";

    Simulator::Schedule(Seconds(interval), [=]() {
      LogStats(flowHelper, monitor, ueNodes, xA, xB, range, interval);
    });
}

std::vector<int> ueToApMap;
std::vector<ApplicationContainer> ueApps;

void UpdateTrafficTarget(NodeContainer& ueNodes,
                         NodeContainer& wifiApNodes,
                         std::vector<Ipv4Address> apAddresses,
                         uint16_t port, double interval)
{
    for (uint32_t i = 0; i < ueNodes.GetN(); ++i) {
        Ptr<Node> ue = ueNodes.Get(i);
        Vector uePos = ue->GetObject<MobilityModel>()->GetPosition();

        // Find closest AP
        double minDist = std::numeric_limits<double>::max();
        int nearestAp = -1;
        for (uint32_t j = 0; j < wifiApNodes.GetN(); ++j) {
            Vector apPos = wifiApNodes.Get(j)->GetObject<MobilityModel>()->GetPosition();
            double dist = CalculateDistance(uePos, apPos);
            if (dist < minDist) {
                minDist = dist;
                nearestAp = j;
            }
        }

        // Handover to another AP（If connection is changed）
        if (nearestAp != ueToApMap[i]) {
          if (ueToApMap[i] == -1) {
            std::cout << "Initial UE " << i << " to AP" << nearestAp + 32 << " at " << Simulator::Now().GetSeconds() << "s\n";
          }
          else {
            std::cout << "UE " << i << " handover from AP" << ueToApMap[i] + 32 << " to AP" << nearestAp + 32 << " at " << Simulator::Now().GetSeconds() << "s\n";
          }
          
          // Stop the old App
          ueApps[i].Stop(Simulator::Now());

          Address targetAddress(InetSocketAddress(apAddresses[nearestAp], port));
          Simulator::Schedule(Seconds(0.1), [=]() {
            BulkSendHelper bulkSend("ns3::TcpSocketFactory", targetAddress);
            bulkSend.SetAttribute("MaxBytes", UintegerValue(0));
            ueApps[i] = bulkSend.Install(ue);
            ueApps[i].Start(Simulator::Now() + Seconds(0.1));
            ueApps[i].Stop(Seconds(60.0));
          });

          ueToApMap[i] = nearestAp;
        }
    }

    // Schedule next update
    Simulator::Schedule(Seconds(interval), &UpdateTrafficTarget,
                        std::ref(ueNodes), std::ref(wifiApNodes), apAddresses, port, interval);
}


int main (int argc, char *argv[]) {
  CommandLine cmd;
    int ap_num = 2;       // 2 APs: Node A & Node B
    int ue_num = 32;      // 16 UEs per AP

  NodeContainer ueNodes;
  ueNodes.Create(ue_num);

  NodeContainer wifiApNodes;
  wifiApNodes.Create(ap_num);

  // ---------------- WiFi setup ---------------- //
  WifiHelper wifi;
  wifi.SetStandard(WIFI_STANDARD_80211n);

  WifiMacHelper mac;
  mac.SetType("ns3::AdhocWifiMac", "QosSupported", BooleanValue(true));

  YansWifiPhyHelper phy;
  phy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
  phy.SetChannel(channel.Create());

  NetDeviceContainer apDevices = wifi.Install(phy, mac, wifiApNodes);
  NetDeviceContainer ueDevices = wifi.Install(phy, mac, ueNodes);

  // ---------------- Set mobility ---------------- //
  MobilityHelper mobility;

  // Set AP position
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(wifiApNodes);
  wifiApNodes.Get(0)->GetObject<MobilityModel>()->SetPosition(Vector(0, 0, 0));   // Node A 位置
  wifiApNodes.Get(1)->GetObject<MobilityModel>()->SetPosition(Vector(200, 0, 0)); // Node B 位置

  // Set UEs initial position
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(ueNodes);
  for (int i = 0; i < 16; i++) {
      ueNodes.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(17*(i%4)+5, 17*(int((i+16)/4))+5 -60, 0));  // 初始在 Node A
  }
  for (int i = 16; i < 32; i++) {
      ueNodes.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(200 + 17*(i%4) +5, 17*(int(i/4)) +5 - 60, 0)); // 初始在 Node B
  }

  // ---------------- User movement ---------------- //
  // 20 seconds
  std::vector<int> moveAtoB_3min = {0, 1, 2, 3};               // 25% 的 16 人 (4 人)
  std::vector<int> moveBtoA_3min = {16, 17, 18, 19, 20, 21, 22, 23};  // 50% 的 16 人 (8 人)
  Simulator::Schedule(Seconds(20), &MoveUsers, ueNodes, moveAtoB_3min, moveBtoA_3min, 0, 200);
  // AP1: {  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23} => 20
  // AP2: {  0,  1,  2,  3, 24, 25, 26, 27, 28, 29, 30, 31}                                 => 12

  // 40 seconds
  std::vector<int> moveAtoB_6min = {4,  5,  6,  7,  8,  9, 10, 11, 12, 13};  // 50% 的 20 人 (10 人)
  std::vector<int> moveBtoA_6min = {24, 25, 26, 27, 28, 29};  // 50% 的 12 人 (6 人)
  Simulator::Schedule(Seconds(40), &MoveUsers, ueNodes, moveAtoB_6min, moveBtoA_6min, 0, 200);
  // AP1: { 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29} => 16
  // AP2: {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 30, 31} => 16

  // ---------------- Network stack & traffic ---------------- //
  InternetStackHelper stack;
  stack.Install(ueNodes);
  stack.Install(wifiApNodes);

  Ipv4AddressHelper address;
  address.SetBase("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer apInterfaces = address.Assign(apDevices);
  Ipv4InterfaceContainer ueInterfaces = address.Assign(ueDevices);

  // Set Full-Queue Model
  uint16_t port = 9;

  std::vector<Ipv4Address> apAddresses;
  for (int i = 0; i < ap_num; ++i) {
      apAddresses.push_back(apInterfaces.GetAddress(i));
      PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
      ApplicationContainer sinkApp = sink.Install(wifiApNodes.Get(i));
      sinkApp.Start(Seconds(0.0));
      sinkApp.Stop(Seconds(60.0));
  }

  ueToApMap.resize(ue_num, -1);
  ueApps.resize(ue_num);
  UpdateTrafficTarget(std::ref(ueNodes), std::ref(wifiApNodes), apAddresses, port, 1.0);

  Simulator::Schedule(Seconds(1), &UpdateTrafficTarget,
                      std::ref(ueNodes), std::ref(wifiApNodes), apAddresses, port, 1.0);

  // ---------------- 設Animation settings ---------------- //

  AnimationInterface anim("wifi_mobility.xml");

  uint32_t ap_img = anim.AddResource("/home/ns3/workspace/ns-3-allinone/ns-3.39/scratch/AP.png");
  uint32_t sta_img = anim.AddResource("/home/ns3/workspace/ns-3-allinone/ns-3.39/scratch/sta.png");

  for (int i = 0; i < ue_num; i++) {
    anim.UpdateNodeImage(i, sta_img);
    anim.UpdateNodeSize(i, 7, 7);
  }

  for (int i = ue_num; i < ap_num + ue_num; i++) {
    anim.UpdateNodeImage(i, ap_img);
    anim.UpdateNodeSize(i, 25, 25);
  }

  anim.SetMobilityPollInterval(Seconds(10.00));
  anim.SetMaxPktsPerTraceFile(100000000000);
  
  FlowMonitorHelper flowHelper;
  Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

  throughputLog << "Time(s),AP0_Throughput(Mbps),AP1_Throughput(Mbps)\n";
  ueLog << "Time(s),AP0_UEs,AP1_UEs\n";
  
  Simulator::Schedule(Seconds(1.0), [&]() {
    LogStats(&flowHelper, monitor, ueNodes, 0, 200, 100.0, 1.0);
  });

  Simulator::Stop(Seconds(60));
  Simulator::Run();
  Simulator::Destroy();

  throughputLog.close();
  ueLog.close();

  return 0;
}
