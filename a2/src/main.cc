#include "ns3/core-module.h"
   #include "ns3/mobility-module.h"
   #include "ns3/lte-module.h"

   using namespace ns3;

   int main(int argc, char *argv[]) {
       Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();

       // Create 9 eNodeB nodes
       NodeContainer enbNodes;
       enbNodes.Create(9);

       // Set mobility for eNodeBs (fixed positions)
       MobilityHelper mobility;
       mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
       mobility.Install(enbNodes);

       // Position the 9 nodes in a grid (example layout)
       Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
       for (int i = 0; i < 3; ++i) {
           for (int j = 0; j < 3; ++j) {
               positionAlloc->Add(Vector(i * 500.0, j * 500.0, 0.0));
           }
       }
       mobility.SetPositionAllocator(positionAlloc);
       mobility.Install(enbNodes);

       // Install LTE stack on eNodeBs
       NetDeviceContainer enbDevs = lteHelper->InstallEnbDevice(enbNodes);

       // Set simulation time and run
       Simulator::Stop(Seconds(1.0));
       Simulator::Run();
       Simulator::Destroy();

       std::cout << "Simulation with 9 LTE nodes completed!" << std::endl;
       return 0;
   }