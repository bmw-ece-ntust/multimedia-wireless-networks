# Assignment 2 - M11302204
## 1. What is ns-3?
![image](https://github.com/user-attachments/assets/a6daee7a-330d-40ce-b307-b88fba42a51c)

ns-3 is built as a system of software libraries that work together. User programs can be written that links with (or imports from) these libraries. User programs are written in either the C or Python programming languages.

* ns-3 is an open-souce network simulation tool
* Can wrtie scripts in C++ & python.
* Can simulate simple and complex networking scenarios.

*Source: [ns-3 Network Simulator - Introduction Lecture](https://www.youtube.com/watch?v=2W5mdzQrwXI&ab_channel=AdilAlsuhaim)*

### 1-1 Prerequisites

![image](https://github.com/user-attachments/assets/2f30ebd6-787a-447e-b4b1-6e06e303da3c)



### 1-2 Key Terms and Abstractions
* Node - the hardware (eg. router, PC, phone)
* Network device (ND) - transmits and receives over the channel
* Channel - transmission medium between NDs (eg. WiFi, ethernet)
* Application - creates or receives data sent between nodes
* Helper - NS-3 construct used to quickly configure and create the above

![image](https://github.com/user-attachments/assets/e1a9cfc0-cd68-4e56-ab43-01cae0efd761)


*Source: [Introduction to NS-3 by MOHAMED KASSEM](https://www.inf.ed.ac.uk/teaching/courses/cn/ns3_intro.pdf)*


## 2. Install the ns-3
> [!IMPORTANT]
> * Root permissions are not needed
> * The use of a non-privileged user account is recommended.


### 2-1 Set Virtual Machine environment

| OS                           | Processor | Memory | Disk |
| ---------------------------- | --------- | ------ | ---- |
| ubuntu-22.04.5-desktop-amd64 | 4         | 4G     | 40G  |

### 2-2 Install Required Tools

```shell
sudo apt-get update
sudo apt-get install git python3-pip g++ cmake qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools gnuplot
```

### 2-3 Download and Build NS-3
```shell
wget https://www.nsnam.org/release/ns-allinone-3.39.tar.bz2
tar xjf ns-allinone-3.39.tar.bz2
cd ns-allinone-3.39/
./build.py
```
### 2-4 Build and Open Netanim
```shell=
cd ~/ns-allinone-3.39/netanim-3.109
make clean
qmake NetAnim.pro
make
```
### 2-5 Run Example to Verification
Cpoy the `/ns-allinone-3.39/ns-3.39/examples/tutorial/hello-simulator.cc` to `/ns-allinone-3.39/ns-3.39/scratch/`
```shell=
cd ~/ns-allinone-3.39/ns-3.39
./ns3 run scratch/hello-simulator.cc
```
![image](https://github.com/user-attachments/assets/b790ac0f-9d1e-4afa-bc52-99370b05bf95)



## 3. Create `lte_assignment2.cc` to Deploy 9 gNBs
```shell
cd ns-allinone-3.39/ns-3.39/scratch
vim lte_assignment2.cc
```
```cpp
#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/mobility-module.h>
#include <ns3/lte-module.h>
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"
using namespace ns3;

int main (int argc, char *argv[])
{
  CommandLine cmd;

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

  Config::SetDefault ("ns3::UdpClient::Interval", TimeValue (MilliSeconds (1000)));
  Config::SetDefault ("ns3::UdpClient::MaxPackets", UintegerValue (1000000));
  Config::SetDefault ("ns3::LteHelper::UseIdealRrc", BooleanValue (false));

  // Create 9 gNBs and 9 UEs
  NodeContainer enbNodes;
  enbNodes.Create (9);
  NodeContainer ueNodes;
  ueNodes.Create (9);

  // Set gNB mobility
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (20.0),
                                 "DeltaY", DoubleValue (20.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility.Install (enbNodes);

  // Set UE mobility
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (10.0),
                                 "MinY", DoubleValue (10.0),
                                 "DeltaX", DoubleValue (20.0),
                                 "DeltaY", DoubleValue (20.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Mode", StringValue ("Time"),
                             "Time", StringValue ("2s"),
                             "Bounds", RectangleValue (Rectangle (-100, 100, -100, 100)));
  mobility.Install (ueNodes);

  // Install LTE devices
  NetDeviceContainer enbDevs = lteHelper->InstallEnbDevice (enbNodes);
  NetDeviceContainer ueDevs = lteHelper->InstallUeDevice (ueNodes);

  // Attach UEs to gNBs
  for (uint32_t i = 0; i < ueNodes.GetN(); i++) {
    lteHelper->Attach (ueDevs.Get(i), enbDevs.Get(i));
  }

  // Activate data bearers
  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
  EpsBearer bearer (q);
  lteHelper->ActivateDataRadioBearer (ueDevs, bearer);

  Simulator::Stop (Seconds (20));
  lteHelper->EnablePhyTraces ();
  lteHelper->EnableMacTraces ();
  lteHelper->EnableRlcTraces ();

  AnimationInterface anim ("lte9.xml");
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
```

## 4. Compile and Run
```shell
cd ~/ns-allinone-3.39/ns-3.39
./ns3 run scratch/lte_assignment2.cc
```

## 5. Visualize with NetAnim
```shell
cd ~/ns-allinone-3.39/netanim-3.109
./NetAnim
```
