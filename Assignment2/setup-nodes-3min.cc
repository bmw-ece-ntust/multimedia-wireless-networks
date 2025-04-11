#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/point-to-point-module.h"
#include <fstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("WifiHandoverSim");

//因筆電關係，模擬不為3min
// 全局變數，用於存儲 AP 和 STA 設備
NetDeviceContainer apDevices;
NetDeviceContainer staDevices;
NodeContainer apNodes;
NodeContainer staNodesA, staNodesB;

// 全局輸出文件
static std::ofstream throughputFile;
static std::ofstream posFile;

// 啟用日誌以調試流量
static void EnableLogging() {
    LogComponentEnable("WifiHandoverSim", LOG_LEVEL_INFO);
    // 禁用 UdpClient 和 UdpServer 的預設追蹤日誌
    LogComponentEnable("UdpClient", LOG_LEVEL_ERROR);
    LogComponentEnable("UdpServer", LOG_LEVEL_ERROR);
}

// 檢查 STA 與 AP 的距離，模擬重新關聯
void ReassociateStaToNearestAp() {
    NodeContainer allStaNodes;
    allStaNodes.Add(staNodesA);
    allStaNodes.Add(staNodesB);

    for (uint32_t i = 0; i < allStaNodes.GetN(); ++i) {
        Ptr<MobilityModel> staMobility = allStaNodes.Get(i)->GetObject<MobilityModel>();
        Vector staPos = staMobility->GetPosition();

        // 計算與 AP-A 和 AP-B 的距離
        Ptr<MobilityModel> apAMobility = apNodes.Get(0)->GetObject<MobilityModel>();
        Ptr<MobilityModel> apBMobility = apNodes.Get(1)->GetObject<MobilityModel>();
        double distToApA = CalculateDistance(staPos, apAMobility->GetPosition());
        double distToApB = CalculateDistance(staPos, apBMobility->GetPosition());

        // 獲取 STA 的 MAC 層
        Ptr<NetDevice> staDevice = staDevices.Get(i);
        Ptr<WifiNetDevice> wifiStaDevice = DynamicCast<WifiNetDevice>(staDevice);
        Ptr<StaWifiMac> staMac = DynamicCast<StaWifiMac>(wifiStaDevice->GetMac());

        // 根據距離選擇最近的 AP
        if (distToApA < distToApB) {
            staMac->SetSsid(Ssid("wifi-network-1"));
        } else {
            staMac->SetSsid(Ssid("wifi-network-2"));
        }
    }
}

// 打印所有 STA 的連線狀態
void PrintStaConnectionStatus(NetDeviceContainer staDevices, uint32_t numStas) {
    int apACount = 0, apBCount = 0;
    for (uint32_t i = 0; i < numStas; ++i) {
        Ptr<NetDevice> staDevice = staDevices.Get(i);
        Ptr<WifiNetDevice> wifiStaDevice = DynamicCast<WifiNetDevice>(staDevice);
        Ptr<StaWifiMac> staMac = DynamicCast<StaWifiMac>(wifiStaDevice->GetMac());
        std::string ssid = staMac->GetSsid().PeekString();
        if (ssid == "wifi-network-1") {
            apACount++;
        } else if (ssid == "wifi-network-2") {
            apBCount++;
        }
    }
    std::cout << "TIME: " << Simulator::Now().GetSeconds() << "s AP-A has " << apACount << " users, AP-B has " << apBCount << " users." << std::endl;

    // 驗證負載平衡
    if (Simulator::Now().GetSeconds() == 10) {
        std::cout << "AP-A should have 20 users (16 + 25% * 16), AP-B should have 12 users (16 - 25% of AP-B's 16)" << std::endl;
        double apAPercent = (apACount / 32.0) * 100.0;
        double apBPercent = (apBCount / 32.0) * 100.0;
        std::cout << "AP-A has " << apACount << " users (" << apAPercent << "% of users), "
                  << "AP-B has " << apBCount << " users (" << apBPercent << "% of users)." << std::endl;
    } else if (Simulator::Now().GetSeconds() == 20) {
        std::cout << "Using calculated percentages to ensure proper balance" << std::endl;
        std::cout << "AP-A current user count: " << apACount << std::endl;
        std::cout << "AP-B current user count: " << apBCount << std::endl;
        if (apACount == 16 && apBCount == 16) {
            std::cout << "PERFECT BALANCE ACHIEVED!" << std::endl;
        }
    } else if (Simulator::Now().GetSeconds() == 40) {
        std::cout << "AP-A should have exactly 16 users to ensure proper balance" << std::endl;
        std::cout << "AP-B should have exactly 16 users to ensure proper balance" << std::endl;
        double apAPercent = (apACount / 32.0) * 100.0;
        double apBPercent = (apBCount / 32.0) * 100.0;
        std::cout << "AP-A has " << apACount << " users (" << apAPercent << "% of users), "
                  << "AP-B has " << apBCount << " users (" << apBPercent << "% of users)." << std::endl;
    }
}

// 記錄 STA 位置
void LogStaPositions(NodeContainer staNodesA, NodeContainer staNodesB, uint32_t numStas, std::ofstream& posFile) {
    NodeContainer allStaNodes;
    allStaNodes.Add(staNodesA);
    allStaNodes.Add(staNodesB);

    for (uint32_t i = 0; i < numStas; ++i) {
        Ptr<MobilityModel> mob = allStaNodes.Get(i)->GetObject<MobilityModel>();
        Vector pos = mob->GetPosition();
        posFile << Simulator::Now().GetSeconds() << "," << i << "," << pos.x << "," << pos.y << "\n";
    }
}

// 監控並記錄吞吐量
void MonitorThroughput(Ptr<FlowMonitor> flowMon, Ptr<Ipv4FlowClassifier> classifier, NetDeviceContainer staDevices, Ipv4InterfaceContainer staIpIfaces, uint32_t numStas) {
    flowMon->CheckForLostPackets();
    FlowMonitor::FlowStatsContainer stats = flowMon->GetFlowStats();

    double throughputApA = 0.0; // AP-A (SSID: wifi-network-1)
    double throughputApB = 0.0; // AP-B (SSID: wifi-network-2)

    for (auto it = stats.begin(); it != stats.end(); ++it) {
        uint32_t flowId = it->first;
        Ipv4FlowClassifier::FiveTuple tuple = classifier->FindFlow(flowId);
        for (uint32_t i = 0; i < numStas; ++i) {
            if (tuple.destinationAddress == staIpIfaces.GetAddress(i)) {
                Ptr<NetDevice> staDevice = staDevices.Get(i);
                Ptr<WifiNetDevice> wifiStaDevice = DynamicCast<WifiNetDevice>(staDevice);
                Ptr<StaWifiMac> staMac = DynamicCast<StaWifiMac>(wifiStaDevice->GetMac());
                std::string ssid = staMac->GetSsid().PeekString();
                double tput = 0.0;
                if (it->second.timeLastRxPacket > it->second.timeFirstTxPacket) {
                    tput = (it->second.rxBytes * 8.0) /
                           (it->second.timeLastRxPacket.GetSeconds() - it->second.timeFirstTxPacket.GetSeconds()) / 1e6;
                }
                if (ssid == "wifi-network-1") {
                    throughputApA += tput;
                } else if (ssid == "wifi-network-2") {
                    throughputApB += tput;
                }
                break;
            }
        }
    }

    throughputFile << Simulator::Now().GetSeconds() << "," << throughputApA << "," << throughputApB << "\n";

    // 每 0.1 秒檢查一次吞吐量
    if (Simulator::Now().GetSeconds() < 40.0) {
        Simulator::Schedule(Seconds(0.1), &MonitorThroughput, flowMon, classifier, staDevices, staIpIfaces, numStas);
    }
}

// 計算最終吞吐量
void CalculateFinalThroughput(Ptr<FlowMonitor> flowMon, Ptr<Ipv4FlowClassifier> classifier, double duration) {
    flowMon->CheckForLostPackets();
    std::map<FlowId, FlowMonitor::FlowStats> stats = flowMon->GetFlowStats();
    double totalThroughput = 0.0;
    uint64_t totalTxPackets = 0, totalRxPackets = 0;

    for (const auto& stat : stats) {
        totalThroughput += stat.second.rxBytes * 8.0 / duration / 1e6; // Mbps
        totalTxPackets += stat.second.txPackets;
        totalRxPackets += stat.second.rxPackets;
    }

    std::cout << "*** Flow monitor statistics ***" << std::endl;
    std::cout << "TX Packets: " << totalTxPackets << std::endl;
    std::cout << "RX Packets: " << totalRxPackets << std::endl;
    std::cout << "Throughput: " << totalThroughput << " Mbps" << std::endl;
}

int main(int argc, char *argv[]) {
    Time simTime = Seconds(40.0);

    // 啟用日誌
    EnableLogging();

    // 打開輸出文件
    throughputFile.open("throughput.csv");
    throughputFile << "Time,Throughput_ApA,Throughput_ApB\n";
    posFile.open("sta_positions.csv");
    posFile << "Time,STA_Index,X,Y\n";

    // 模擬場景描述
    std::cout << "*** SIMULATION SCENARIO ***" << std::endl;
    std::cout << "10-20s: Movement at 10s - AP-A should have 20 users, AP-B should have 12 users." << std::endl;
    std::cout << "20-30s: Movement at 20s - AP-A and AP-B each return to 16 users." << std::endl;

    // 創建節點
    apNodes.Create(2);
    staNodesA.Create(16); // 第一組 STA
    staNodesB.Create(16); // 第二組 STA
    NodeContainer remoteHost;
    remoteHost.Create(1);

    // 設置 AP 位置
    MobilityHelper apMobility;
    apMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    apMobility.Install(apNodes);
    apNodes.Get(0)->GetObject<MobilityModel>()->SetPosition(Vector(0, 0, 0));
    apNodes.Get(1)->GetObject<MobilityModel>()->SetPosition(Vector(500, 0, 0));

    // 設置 STA 位置
    MobilityHelper staMobility;
    staMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    staMobility.Install(staNodesA);
    staMobility.Install(staNodesB);
    for (uint32_t i = 0; i < 16; ++i) {
        staNodesA.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(50 + i*5, 10, 0));
        staNodesB.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(450 + i*5, -10, 0));
    }

    // 設置遠程主機位置
    MobilityHelper remoteMobility;
    remoteMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    remoteMobility.Install(remoteHost);

    // WiFi 設置
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211n);
    wifi.SetRemoteStationManager("ns3::MinstrelHtWifiManager");

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());
    phy.Set("TxPowerStart", DoubleValue(20.0)); // 增加發射功率
    phy.Set("TxPowerEnd", DoubleValue(20.0));

    WifiMacHelper mac;
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(Ssid("wifi-network-1")));
    NetDeviceContainer apDevicesA = wifi.Install(phy, mac, apNodes.Get(0));

    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(Ssid("wifi-network-2")));
    NetDeviceContainer apDevicesB = wifi.Install(phy, mac, apNodes.Get(1));

    apDevices.Add(apDevicesA);
    apDevices.Add(apDevicesB);

    // 為 STA 設置初始 SSID
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(Ssid("wifi-network-1")));
    NetDeviceContainer staDevicesA = wifi.Install(phy, mac, staNodesA);
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(Ssid("wifi-network-2")));
    NetDeviceContainer staDevicesB = wifi.Install(phy, mac, staNodesB);

    staDevices.Add(staDevicesA);
    staDevices.Add(staDevicesB);

    // 安裝網路堆疊
    InternetStackHelper internet;
    internet.Install(staNodesA);
    internet.Install(staNodesB);
    internet.Install(apNodes);
    internet.Install(remoteHost);

    // 設置點對點鏈路（連接到遠程主機）
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));
    NetDeviceContainer hostDevices = p2p.Install(apNodes.Get(0), remoteHost.Get(0));

    // 分配 IP 地址
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer apInterfaces = address.Assign(apDevices);
    Ipv4InterfaceContainer staInterfaces = address.Assign(staDevices);
    address.SetBase("7.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer hostInterfaces = address.Assign(hostDevices);

    // 添加靜態路由，確保 remoteHost 可以到達所有 STA
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4> remoteHostIpv4 = remoteHost.Get(0)->GetObject<Ipv4>();
    Ptr<Ipv4StaticRouting> remoteHostRouting = ipv4RoutingHelper.GetStaticRouting(remoteHostIpv4);
    remoteHostRouting->AddNetworkRouteTo(Ipv4Address("10.1.1.0"), Ipv4Mask("255.255.255.0"), 1);

    // 設置 UDP 流量（從遠程主機到 STA）
    uint16_t basePort = 49153;
    ApplicationContainer serverApps, clientApps;
    for (uint32_t i = 0; i < 32; ++i) {
        UdpServerHelper udpServer(basePort + i);
        NodeContainer allStaNodes;
        allStaNodes.Add(staNodesA);
        allStaNodes.Add(staNodesB);
        serverApps.Add(udpServer.Install(allStaNodes.Get(i)));
        serverApps.Start(Seconds(0.5));
        serverApps.Stop(simTime);

        UdpClientHelper udpClient(staInterfaces.GetAddress(i), basePort + i);
        udpClient.SetAttribute("MaxPackets", UintegerValue(100000));
        udpClient.SetAttribute("Interval", TimeValue(MilliSeconds(10)));
        udpClient.SetAttribute("PacketSize", UintegerValue(512));
        clientApps.Add(udpClient.Install(remoteHost.Get(0)));
        clientApps.Start(Seconds(1.0));
        clientApps.Stop(simTime);
    }

    // 設置 FlowMonitor
    FlowMonitorHelper flowMonHelper;
    flowMonHelper.SetMonitorAttribute("StartTime", TimeValue(Seconds(1.0)));
    Ptr<FlowMonitor> flowMon = flowMonHelper.InstallAll();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowMonHelper.GetClassifier());

    // 安排初始任務
    Simulator::Schedule(Seconds(1.0), &PrintStaConnectionStatus, staDevices, 32);
    Simulator::Schedule(Seconds(1.0), &MonitorThroughput, flowMon, classifier, staDevices, staInterfaces, 32);
    Simulator::Schedule(Seconds(1.0), &LogStaPositions, staNodesA, staNodesB, 32, std::ref(posFile));

    // 第一次移動事件（10 秒）
    Simulator::Schedule(Seconds(10.0), []() {
        std::cout << "\n*** Expected after first movement at 10s ***\n";
        for (uint32_t i = 0; i < 4; ++i) {
            staNodesA.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(450 + i*10, 20, 0));
        }
        ReassociateStaToNearestAp();
    });

    // 第一次移動後打印連線狀態
    Simulator::Schedule(Seconds(10.0), &PrintStaConnectionStatus, staDevices, 32);

    // 第二次移動事件（20 秒）
    Simulator::Schedule(Seconds(20.0), []() {
        std::cout << "\n*** Expected after second movement at 20s ***\n";
        std::cout << "Moving 4 users from A to B and 8 users from B to A to achieve balance" << std::endl;
        for (uint32_t i = 0; i < 4; ++i) {
            staNodesA.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(50 + i*10, 30, 0));
        }
        for (uint32_t i = 0; i < 8; ++i) {
            staNodesB.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(50 + i*10, -20, 0));
        }
        ReassociateStaToNearestAp();
    });

    // 第二次移動後打印連線狀態
    Simulator::Schedule(Seconds(20.0), &PrintStaConnectionStatus, staDevices, 32);

    // 定期打印連線狀態（每秒）
    for (int t = 1; t <= 40; ++t) {
        Simulator::Schedule(Seconds(t), &PrintStaConnectionStatus, staDevices, 32);
    }

    // 最終位置記錄
    Simulator::Schedule(simTime, &LogStaPositions, staNodesA, staNodesB, 32, std::ref(posFile));

    // 最終用戶分佈和吞吐量
    Simulator::Schedule(Seconds(40.0), [flowMon, classifier]() {
        std::cout << "\n*** Final user distribution ***\n";
        int apACount = 0, apBCount = 0;
        for (uint32_t i = 0; i < staDevices.GetN(); ++i) {
            Ptr<NetDevice> staDevice = staDevices.Get(i);
            Ptr<WifiNetDevice> wifiStaDevice = DynamicCast<WifiNetDevice>(staDevice);
            Ptr<StaWifiMac> staMac = DynamicCast<StaWifiMac>(wifiStaDevice->GetMac());
            std::string ssid = staMac->GetSsid().PeekString();
            if (ssid == "wifi-network-1") apACount++;
            else if (ssid == "wifi-network-2") apBCount++;
        }
        std::cout << "AP-A final user count: " << apACount << std::endl;
        std::cout << "AP-B final user count: " << apBCount << std::endl;
        std::cout << "Total users in system: " << (apACount + apBCount) << std::endl;

        // 計算最終吞吐量
        CalculateFinalThroughput(flowMon, classifier, 38.0); // 排除啟動時間
    });

    // 運行模擬
    Simulator::Stop(simTime);
    Simulator::Run();

    // 關閉文件
    throughputFile.close();
    posFile.close();

    Simulator::Destroy();
    std::cout << "\nSimulation completed successfully!\n";
    return 0;
}
