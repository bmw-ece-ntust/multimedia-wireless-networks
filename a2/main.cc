#include <ns3/core-module.h>
#include "ns3/internet-module.h"
#include <ns3/network-module.h>
#include <ns3/mobility-module.h>
#include <ns3/lte-module.h>
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"
using namespace ns3;

int main (int argc, char *argv[])
{
  CommandLine cmd;
  int enb_num = 9;
  int ue_num = 16;

  //This will instantiate some common objects (e.g., the Channel object) and provide the methods to add eNBs and UEs and configure them.
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

  //------Create Node objects for the eNB(s) and the UEs:------//
  NodeContainer enbNodes;
  enbNodes.Create (enb_num);  //Note that the above Node instances at this point still don’t have an LTE protocol stack installed; they’re just empty nodes.
  NodeContainer ueNodes;
  ueNodes.Create (ue_num);
  // -----------------------------end---------------------------//

  //Configure the Mobility model for all the nodes:
  MobilityHelper mobility;
  
  // ----------------------- Set UEs----------------------------//
  mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
  "X", StringValue ("180.0"),
  "Y", StringValue ("180.0"),
  "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=100]"));
  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
  "Mode", StringValue ("Time"),
  "Time", StringValue ("1s"),
  "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=20]"),
  "Bounds", RectangleValue (Rectangle (-2000, 2000, -2000, 2000)));
  mobility.Install (ueNodes);

  //----------------------------Set eNB--------------------------//
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
  "MinX", DoubleValue (89.0),
  "MinY", DoubleValue (89.0),
  "DeltaX", DoubleValue (180.0),
  "DeltaY", DoubleValue (180.0),
  "GridWidth", UintegerValue (2),
  "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (enbNodes);
  // -----------------------------end---------------------------//

  //Install an LTE protocol stack on the eNB(s):
  NetDeviceContainer enbDevs;
  enbDevs = lteHelper->InstallEnbDevice (enbNodes);

  //Install an LTE protocol stack on the UEs:
  NetDeviceContainer ueDevs;
  ueDevs = lteHelper->InstallUeDevice (ueNodes);

  //Attach the UEs to an eNB. This will configure each UE according to the eNB configuration, and create an RRC connection between them:
  lteHelper->AttachToClosestEnb (ueDevs, enbDevs);
  // lteHelper->Attach (ueDevs, enbDevs.Get (0));

  //Activate a data radio bearer between each UE and the eNB it is attached to:
  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
  EpsBearer bearer (q);
  lteHelper->ActivateDataRadioBearer (ueDevs, bearer);
  //this method will also activate two saturation traffic generators for that bearer, one in uplink and one in downlink.


  //Set the stop time
  Simulator::Stop (Seconds (20));

  //This is needed otherwise the simulation will last forever, because (among others) the start-of-subframe event is scheduled repeatedly, and the ns-3 simulator scheduler will hence never run out of events.
  lteHelper->EnablePhyTraces ();
  lteHelper->EnableMacTraces ();
  lteHelper->EnableRlcTraces ();

  //Run the simulation:
  AnimationInterface anim ("lte_my_mobility.xml");
  anim.EnablePacketMetadata(true);
  uint32_t enb_img = anim.AddResource ("/home/ns3/workspace/ns-3-allinone/ns-3.39/scratch/enb.png");
  uint32_t ue_img = anim.AddResource ("/home/ns3/workspace/ns-3-allinone/ns-3.39/scratch/ue.png");
  for(int i = 0; i < enb_num; i++){
   	anim.UpdateNodeImage (i, enb_img);
  	anim.UpdateNodeSize(i, 30, 30);
   	//anim.UpdateNodeColor(i, 0, 0, 0);
  }
  for(int i = enb_num; i < enb_num+ue_num; i++){
    anim.UpdateNodeImage (i, ue_img);
    anim.UpdateNodeSize(i, 12, 12);
    //anim.UpdateNodeColor(i, 0, 0, 0);
  }
  anim.SetMobilityPollInterval(Seconds(1.00));
  anim.SetMaxPktsPerTraceFile (100000000000);

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
