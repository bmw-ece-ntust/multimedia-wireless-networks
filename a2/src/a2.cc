#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/ssid.h"
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TwoApSwitchSta");

double ap1RxBytes = 0;
double ap2RxBytes = 0;

std::ofstream ap1Log("ap1_throughput.csv");
std::ofstream ap2Log("ap2_throughput.csv");
std::ofstream staBssidLog("sta_bssid_log.csv");
std::ofstream staIpMacLog("sta_ip_mac_map.csv");
std::ofstream movementLog("movement_log.csv");
std::ofstream apStaCountLog("ap_sta_count.csv");

NodeContainer globalStaNodes;
std::vector<Ipv4Address> staIpList;
std::vector<std::pair<ApplicationContainer, ApplicationContainer>> staOnOffApps;
std::vector<Mac48Address> previousBssids;
Mac48Address ap1Bssid, ap2Bssid;

//get AP1 received packet
void RxCallbackAp1(Ptr<const Packet> packet, const Address &) {
    ap1RxBytes += packet->GetSize();
}

//get AP2 received packet
void RxCallbackAp2(Ptr<const Packet> packet, const Address &) {
    ap2RxBytes += packet->GetSize();
}

//caculate and record AP throughput
void RecordThroughput() {
    double time = Simulator::Now().GetSeconds();
    double ap1Mbps = ap1RxBytes * 8.0 / 0.5 / 1e6;
    double ap2Mbps = ap2RxBytes * 8.0 / 0.5 / 1e6;
    ap1Log << time << "," << ap1Mbps << "\n";
    ap2Log << time << "," << ap2Mbps << "\n";
    ap1RxBytes = 0;
    ap2RxBytes = 0;

    if (time < 30.0)
        Simulator::Schedule(Seconds(0.5), &RecordThroughput);
}

//record STA's BSSID
void LogStaBssids() {
    double time = Simulator::Now().GetSeconds();
    staBssidLog << time;
    for (uint32_t i = 0; i < globalStaNodes.GetN(); ++i) {
        Ptr<WifiNetDevice> dev = DynamicCast<WifiNetDevice>(globalStaNodes.Get(i)->GetDevice(0));
        Ptr<StaWifiMac> mac = DynamicCast<StaWifiMac>(dev->GetMac());
        Mac48Address bssid = mac->GetBssid(0);
        staBssidLog << "," << bssid;
    }
    staBssidLog << "\n";

    if (time < 30.0)
        Simulator::Schedule(Seconds(1.0), &LogStaBssids);
}

//record the number of STAs for each AP
void LogApStaCounts() {
    double time = Simulator::Now().GetSeconds();
    int ap1Count = 0;
    int ap2Count = 0;

    for (uint32_t i = 0; i < globalStaNodes.GetN(); ++i) {
        Ptr<WifiNetDevice> dev = DynamicCast<WifiNetDevice>(globalStaNodes.Get(i)->GetDevice(0));
        Ptr<StaWifiMac> mac = DynamicCast<StaWifiMac>(dev->GetMac());
        Mac48Address bssid = mac->GetBssid(0);

        if (bssid == ap1Bssid)
            ap1Count++;
        else if (bssid == ap2Bssid)
            ap2Count++;
    }

    apStaCountLog << time << "," << ap1Count << "," << ap2Count << "\n";

    if (time < 30.0)
        Simulator::Schedule(Seconds(1.0), &LogApStaCounts);
}

//check the STA's SSID to decide which app to be open
void UpdateStaAppsBasedOnBssid() {
    for (uint32_t i = 0; i < globalStaNodes.GetN(); ++i) {
        Ptr<WifiNetDevice> dev = DynamicCast<WifiNetDevice>(globalStaNodes.Get(i)->GetDevice(0));
        Ptr<StaWifiMac> mac = DynamicCast<StaWifiMac>(dev->GetMac());
        Mac48Address currentBssid = mac->GetBssid(0);

        if (currentBssid != previousBssids[i]) {
            ApplicationContainer app1 = staOnOffApps[i].first;
            ApplicationContainer app2 = staOnOffApps[i].second;

            if (previousBssids[i] == ap1Bssid) {
                app1.Stop(Simulator::Now());
            } else if (previousBssids[i] == ap2Bssid) {
                app2.Stop(Simulator::Now());
            }

            if (currentBssid == ap1Bssid) {
                app1.Start(Simulator::Now());
            } else if (currentBssid == ap2Bssid) {
                app2.Start(Simulator::Now());
            }

            previousBssids[i] = currentBssid;
        }
    }

    if (Simulator::Now().GetSeconds() < 30.0)
        Simulator::Schedule(Seconds(1.0), &UpdateStaAppsBasedOnBssid);
}

//STA mobility
void MoveStasAtTime(double time, NodeContainer &staNodes, std::vector<Ipv4Address> &staIpList, std::ofstream &movementLog) {
    std::vector<uint32_t> ap1Stas, ap2Stas;
    for (uint32_t i = 0; i < staNodes.GetN(); ++i) {
        Ptr<MobilityModel> mob = staNodes.Get(i)->GetObject<MobilityModel>();
        Vector pos = mob->GetPosition();
        if (pos.x < 5.0)
            ap1Stas.push_back(i);
        else
            ap2Stas.push_back(i);
    }

    std::random_shuffle(ap1Stas.begin(), ap1Stas.end());
    std::random_shuffle(ap2Stas.begin(), ap2Stas.end());

    int moveCountAp1 = (time == 10.0) ? ap1Stas.size() * 0.25 : ap1Stas.size() * 0.5;
    int moveCountAp2 = (time == 10.0) ? ap2Stas.size() * 0.5 : ap2Stas.size() * 0.5;

    for (int i = 0; i < moveCountAp1 && i < (int)ap1Stas.size(); ++i) {
        uint32_t idx = ap1Stas[i];
        staNodes.Get(idx)->GetObject<MobilityModel>()->SetPosition(Vector(10.0, 0.0, 0.0));
        std::cout << "[" << time << "s] Moved STA " << idx << " (IP: " << staIpList[idx] << ") → AP2\n";
        movementLog << time << "," << idx << "," << staIpList[idx] << ",AP2\n";
    }

    for (int i = 0; i < moveCountAp2 && i < (int)ap2Stas.size(); ++i) {
        uint32_t idx = ap2Stas[i];
        staNodes.Get(idx)->GetObject<MobilityModel>()->SetPosition(Vector(0.0, 0.0, 0.0));
        std::cout << "[" << time << "s] Moved STA " << idx << " (IP: " << staIpList[idx] << ") → AP1\n";
        movementLog << time << "," << idx << "," << staIpList[idx] << ",AP1\n";
    }
}

int main(int argc, char *argv[]) {
    Time::SetResolution(Time::NS);
    LogComponentEnable("TwoApSwitchSta", LOG_LEVEL_INFO);

    ap1Log << "Time,Throughput(Mbps)\n";
    ap2Log << "Time,Throughput(Mbps)\n";
    apStaCountLog << "Time,AP1_STA_Count,AP2_STA_Count\n";

    NodeContainer staNodes, apNodes;
    staNodes.Create(32);
    apNodes.Create(2);
    globalStaNodes = staNodes;

    //seting propagation range to 5m
    Config::SetDefault("ns3::RangePropagationLossModel::MaxRange", DoubleValue(5));
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    channel.AddPropagationLoss("ns3::RangePropagationLossModel");
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211n);

    WifiMacHelper mac;
    Ssid ssid = Ssid("ssid");

    //install NetDevice
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));
    NetDeviceContainer staDevices = wifi.Install(phy, mac, staNodes);

    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    NetDeviceContainer apDevices = wifi.Install(phy, mac, apNodes);

    ap1Bssid = DynamicCast<WifiNetDevice>(apDevices.Get(0))->GetMac()->GetAddress();
    ap2Bssid = DynamicCast<WifiNetDevice>(apDevices.Get(1))->GetMac()->GetAddress();


    //setting STA and AP's position
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> posAlloc = CreateObject<ListPositionAllocator>();
    for (uint32_t i = 0; i < 32; ++i)
        posAlloc->Add(Vector(i < 16 ? 0.0 : 10.0, 0.0, 0.0));
    posAlloc->Add(Vector(0.0, 0.0, 0.0));
    posAlloc->Add(Vector(10.0, 0.0, 0.0));
    mobility.SetPositionAllocator(posAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(staNodes);
    mobility.Install(apNodes);


    //install IP and stack on STA and AP
    InternetStackHelper stack;
    stack.Install(staNodes);
    stack.Install(apNodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    NetDeviceContainer allDevices;
    allDevices.Add(staDevices);
    allDevices.Add(apDevices);
    Ipv4InterfaceContainer interfaces = address.Assign(allDevices);

    Ipv4Address ap1Addr = interfaces.GetAddress(32);
    Ipv4Address ap2Addr = interfaces.GetAddress(33);

    for (uint32_t i = 0; i < staNodes.GetN(); ++i)
        staIpList.push_back(interfaces.GetAddress(i));

    
    //print each STA's IP in .csv
    staIpMacLog << "STA_ID,IP_Address,MAC_Address\n";
    for (uint32_t i = 0; i < staNodes.GetN(); ++i) {
        Ptr<WifiNetDevice> dev = DynamicCast<WifiNetDevice>(staNodes.Get(i)->GetDevice(0));
        staIpMacLog << i << "," << staIpList[i] << "," << dev->GetMac()->GetAddress() << "\n";
    }

    movementLog << "Time,STA_ID,IP,New_AP\n";

    //install sink app to monitor throughput on each AP
    uint16_t port = 9;
    PacketSinkHelper sink("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApp1 = sink.Install(apNodes.Get(0));
    ApplicationContainer sinkApp2 = sink.Install(apNodes.Get(1));
    sinkApp1.Start(Seconds(0.0));
    sinkApp2.Start(Seconds(0.0));
    sinkApp1.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&RxCallbackAp1));
    sinkApp2.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&RxCallbackAp2));

    //install two onoff app to every STA node
    for (uint32_t i = 0; i < 32; ++i) {
        OnOffHelper onoff1("ns3::UdpSocketFactory", Address(InetSocketAddress(ap1Addr, port)));
        onoff1.SetConstantRate(DataRate("100kbps"), 125);
        ApplicationContainer app1 = onoff1.Install(staNodes.Get(i));
        app1.Stop(Seconds(0.0));

        OnOffHelper onoff2("ns3::UdpSocketFactory", Address(InetSocketAddress(ap2Addr, port)));
        onoff2.SetConstantRate(DataRate("100kbps"), 125);
        ApplicationContainer app2 = onoff2.Install(staNodes.Get(i));
        app2.Stop(Seconds(0.0));

        staOnOffApps.emplace_back(app1, app2);
    }

    previousBssids.resize(32, Mac48Address::GetBroadcast());

    //change STA position first time at 10s
    Simulator::Schedule(Seconds(10.0), &MoveStasAtTime, 10.0, std::ref(staNodes), std::ref(staIpList), std::ref(movementLog));
    //change STA position second time at 20s
    Simulator::Schedule(Seconds(20.0), &MoveStasAtTime, 20.0, std::ref(staNodes), std::ref(staIpList), std::ref(movementLog));

    //print each STA's BSSID in each sec
    staBssidLog << "Time";
    for (const auto &ip : staIpList)
        staBssidLog << "," << ip;
    staBssidLog << "\n";

    Simulator::Schedule(Seconds(0.5), &RecordThroughput);             //get and record throughput in every 0.5s
    Simulator::Schedule(Seconds(1.0), &LogStaBssids);                 //record STA's BSSID in every 1s
    Simulator::Schedule(Seconds(1.0), &LogApStaCounts);               //record the number of STAs for each AP in every 1s
    Simulator::Schedule(Seconds(1.0), &UpdateStaAppsBasedOnBssid);    //check the STA's SSID to decide which app to be open

    Simulator::Stop(Seconds(30.0));
    Simulator::Run();
    Simulator::Destroy();

    ap1Log.close();
    ap2Log.close();
    staBssidLog.close();
    staIpMacLog.close();
    movementLog.close();
    apStaCountLog.close();
    return 0;
}
