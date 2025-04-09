// LTE Handover Simulation with NetAnim support for ns-3.39
// Two eNodeBs and 32 UEs. Mobility and handovers logged in detail.

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("LteHandoverSim");

int main(int argc, char *argv[]) {
  Time simTime = Seconds(600.0); // Total simulation time: 10 minutes
  CommandLine cmd;
  cmd.Parse(argc, argv);

  // Enable logging
  LogComponentEnable("LteHandoverSim", LOG_LEVEL_INFO);

  // Create LTE and EPC helpers
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
  Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
  lteHelper->SetEpcHelper(epcHelper);

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
  enbNodes.Get(0)->GetObject<MobilityModel>()->SetPosition(Vector(0, 0, 0));      // eNodeB A
  enbNodes.Get(1)->GetObject<MobilityModel>()->SetPosition(Vector(500, 0, 0));    // eNodeB B

  // Set positions for UEs (half near each eNodeB)
  MobilityHelper ueMobility;
  ueMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  ueMobility.Install(ueNodes);
  for (uint32_t i = 0; i < 16; ++i) {
    ueNodes.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(50 + i*5, 10, 0));
  }
  for (uint32_t i = 16; i < 32; ++i) {
    ueNodes.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(550 + (i-16)*5, -10, 0));
  }

  // Install LTE devices
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice(enbNodes);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice(ueNodes);

  // Install internet stack on UEs
  InternetStackHelper internet;
  internet.Install(ueNodes);

  // Assign IP addresses to UEs
  epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueLteDevs));

  // Attach UEs to eNodeBs initially
  for (uint32_t i = 0; i < 16; ++i) {
    lteHelper->Attach(ueLteDevs.Get(i), enbLteDevs.Get(0));
  }
  for (uint32_t i = 16; i < 32; ++i) {
    lteHelper->Attach(ueLteDevs.Get(i), enbLteDevs.Get(1));
  }

  // Activate EPS bearers (optional QoS config)
  for (uint32_t i = 0; i < 32; ++i) {
    lteHelper->ActivateDedicatedEpsBearer(ueLteDevs.Get(i), EpsBearer(EpsBearer::NGBR_VIDEO_TCP_DEFAULT), EpcTft::Default());
  }

  // First handover event at 3 minutes
  Simulator::Schedule(Seconds(180.0), [&ueNodes]() {
    NS_LOG_INFO("[3min] 4 UEs from eNB A → B; 8 UEs from B → A");
    for (uint32_t i = 0; i < 4; ++i) {
      ueNodes.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(600 + i*5, 20, 0));
    }
    for (uint32_t i = 0; i < 8; ++i) {
      ueNodes.Get(16 + i)->GetObject<MobilityModel>()->SetPosition(Vector(100 + i*5, -20, 0));
    }
  });

  // Second handover event at 6 minutes
  Simulator::Schedule(Seconds(360.0), [&ueNodes]() {
    NS_LOG_INFO("[6min] Half of UEs on A & B swap positions");
    for (uint32_t i = 0; i < 8; ++i) {
      ueNodes.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(620 + i*5, 30, 0));
    }
    for (uint32_t i = 0; i < 8; ++i) {
      ueNodes.Get(16 + i)->GetObject<MobilityModel>()->SetPosition(Vector(120 + i*5, 30, 0));
    }
  });

  // Enable NetAnim output
  AnimationInterface anim("lte-handover.xml");
  anim.SetMobilityPollInterval(Seconds(1.0));

  Simulator::Stop(simTime);
  Simulator::Run();
  Simulator::Destroy();
  return 0;
}
