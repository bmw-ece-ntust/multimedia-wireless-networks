// LTE Handover Simulation without Traffic for ns-3.39
// Two eNodeBs and 32 UEs. Mobility and handovers logged in detail.

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/internet-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("LteHandoverSim");

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

int main(int argc, char *argv[]) {
    Time simTime = Seconds(30.0); // Shortened for testing
    CommandLine cmd;
    cmd.Parse(argc, argv);

    // Enable logging
    LogComponentEnable("LteHandoverSim", LOG_LEVEL_INFO);

    // Create LTE and EPC helpers
    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
    Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
    lteHelper->SetEpcHelper(epcHelper);

    // Configure handover algorithm (A3-RSRP with relaxed parameters)
    lteHelper->SetHandoverAlgorithmType("ns3::A3RsrpHandoverAlgorithm");
    lteHelper->SetHandoverAlgorithmAttribute("Hysteresis", DoubleValue(0.5)); // Very low hysteresis
    lteHelper->SetHandoverAlgorithmAttribute("TimeToTrigger", TimeValue(MilliSeconds(200))); // Longer trigger time

    // Configure path loss model for clear signal difference
    lteHelper->SetAttribute("PathlossModel", StringValue("ns3::LogDistancePropagationLossModel"));
    lteHelper->SetPathlossModelAttribute("Exponent", DoubleValue(3.0)); // Realistic path loss exponent

    // Create two eNodeB nodes
    NodeContainer enbNodes;
    enbNodes.Create(2);

    // Create 32 UE nodes
    NodeContainer ueNodes;
    ueNodes.Create(32);

    // Set static positions for eNodeBs
    MobilityHelper enbMobility;
    enbMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    enbMobility.Install(enbNodes);
    enbNodes.Get(0)->GetObject<MobilityModel>()->SetPosition(Vector(0, 0, 0));    // eNodeB A
    enbNodes.Get(1)->GetObject<MobilityModel>()->SetPosition(Vector(500, 0, 0));  // eNodeB B

    // Set initial positions for UEs (half near each eNodeB)
    MobilityHelper ueMobility;
    ueMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    ueMobility.Install(ueNodes);
    for (uint32_t i = 0; i < 16; ++i) {
        ueNodes.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(50 + i * 5, 10, 0));
    }
    for (uint32_t i = 16; i < 32; ++i) {
        ueNodes.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(550 + (i - 16) * 5, -10, 0));
    }

    // Install LTE devices
    NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice(enbNodes);
    NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice(ueNodes);

    // Install internet stack on UEs
    InternetStackHelper internet;
    internet.Install(ueNodes);

    // Assign IP addresses to UEs
    Ipv4InterfaceContainer ueIpIfaces = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueLteDevs));

    // Attach UEs to eNodeBs initially
    for (uint32_t i = 0; i < 16; ++i) {
        lteHelper->Attach(ueLteDevs.Get(i), enbLteDevs.Get(0));
    }
    for (uint32_t i = 16; i < 32; ++i) {
        lteHelper->Attach(ueLteDevs.Get(i), enbLteDevs.Get(1));
    }

    // Print initial UE connection status
    Simulator::Schedule(Seconds(1.0), &PrintUeConnectionStatus, lteHelper, ueLteDevs, 32);

    // First handover event at 3 mins (180 seconds)
    Simulator::Schedule(Seconds(10.0), [&ueNodes]() {
        NS_LOG_INFO("[3-min] 4 UEs from eNB A → B; 8 UEs from B → A");
        // Move UEs closer to target eNodeBs
        for (uint32_t i = 0; i < 4; ++i) {
            ueNodes.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(550 + i * 5, 20, 0)); // Very close to eNodeB B
        }
        for (uint32_t i = 0; i < 8; ++i) {
            ueNodes.Get(16 + i)->GetObject<MobilityModel>()->SetPosition(Vector(50 + i * 5, -20, 0)); // Very close to eNodeB A
        }
    });

    // Print connection status after first handover
    Simulator::Schedule(Seconds(15.0), &PrintUeConnectionStatus, lteHelper, ueLteDevs, 32);

    // Second handover event at 6 mins (360 seconds)
    Simulator::Schedule(Seconds(20.0), [&ueNodes]() {
        NS_LOG_INFO("[6-min] Half of UEs on A & B swap positions");
        // Move UEs
        for (uint32_t i = 0; i < 8; ++i) {
            ueNodes.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(550 + i * 5, 30, 0)); // Near eNodeB B
        }
        for (uint32_t i = 0; i < 8; ++i) {
            ueNodes.Get(16 + i)->GetObject<MobilityModel>()->SetPosition(Vector(50 + i * 5, 30, 0)); // Near eNodeB A
        }
    });

    // Print connection status after second handover
    Simulator::Schedule(Seconds(25.0), &PrintUeConnectionStatus, lteHelper, ueLteDevs, 32);

    Simulator::Stop(simTime);
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("Simulation completed successfully!");
    return 0;
}