# Multimedia Wireless Networks Assignment 2
---
- Title: ns-3 Network Simulator
- Deadline: 4/7, 23.59 A.M.
- Purposes:
  - Part I: Study (30%)
    - Learn the basic concept of ns-3;
    - Install ns-3;
  - Part II: Simulate the LTE network using ns-3 (40%)
    - Execute an example for LTE
    - Define and generate a network topology by yourself, e.g. 
      - A square of 10 km x10 km.
      - The center point is (0,0)
      - 9 macro-cells are located in (5,5), (0, 5) (-5, 5), (5,0), (0,0), (-5,0), (5,-5), (0, -5) (-5, -5). The cell radius of each macro cell is 5 km
      - 30 micro-cells, each has radius of 1 km, are randomly distributed in the square area
      - 50 UEs are randomly distributed in the square area
      - Explain the models you used in your simulation, e.g.,
        - mobility model
        - traffic model
        - channel model by yourself
      - Plot the performance metrics you simulated
- References:
  - [ns-3](https://www.nsnam.org/)
  - [ns-3 Tutorial](https://www.nsnam.org/docs/tutorial/html/)
  - [The ns-3 Network Simulator](https://intronetworks.cs.luc.edu/current/html/ns3.html)
  - [ns-3 LTE module](https://www.nsnam.org/docs/models/html/lte-user.html)
  - [3GPP NR ns-3 module](https://github.com/QiuYukang/5G-LENA)
- Deliverables:
  - Part I (30%):
    - Weekly Plan (Setup timeline for you to finish the goal above)
    - Network topology
    - Studying note
    - Presentation video
  - Part II (40%):
  - Demo video: Run a simulation for LTE Network
    - Top 3 students (30%)
---
## Weekly Plan
- Weekly Plan

| Time       | Tesk                                                          |
| ---------- | ------------------------------------------------------------- |
| Mar. 18-20 | Learn ns-3 & write into Study note                            |
| Mar. 20-22 | Install ns-3 & write into Study note                          |
| Mar. 25-29 | Simulate the 5G NR network using ns-3 & write into Study note |



## <center style= "background:#C9FFFF">:notebook_with_decorative_cover: <i style= "background:#C9FFFF">1. Introduction</i></center>
<span style="background:#FFD4D4; border-radius:9px; padding: 6px 18px;">:arrow_forward: *Reference: [ns-3 introduction](https://www.nsnam.org/docs/tutorial/html/introduction.html)*</span>
### <span style="background:#FFF0D4; border-radius:9px; padding: 6px 18px;">**About ns-3:**</span>
- <b style="color: red">The ns-3 simulator is a discrete-event network simulator</b> targeted primarily for research and educational use and typically run from the command line. 
	- It is written directly in C++
	- It has been developed to provide an open, extensible network simulation platform, for networking research and education. In brief, <b style="color: red">ns-3 provides models of how packet data networks work and perform, and provides a simulation engine for users to conduct simulation experiments.</b>
- A few key points are worth noting at the onset:
	- ns-3 is open-source, and the project strives to maintain an open environment for researchers to contribute and share their software.
	- <b style="color: red">ns-3 is not a backwards-compatible extension of ns-2.</b> It is a new simulator. The two simulators are both written in C++ but ns-3 is a new simulator that does not support the ns-2 APIs.

### <span style="background:#FFF0D4; border-radius:9px; padding: 6px 18px;">**Advantage:**</span>
- It is difficult or not possible to perform with real systems for studying. Thus, useing ns-3 to study system behavior in a highly controlled, reproducible environment, and to learn about how networks work is better.
- The following is different features between ns-3 other simulation tools.
	- <b style="color: red">ns-3 is designed as a set of libraries that can be combined together and also with other external software libraries.</b>
	- Some simulation platforms provide users with a single, while ns-3 is more modular in this regard.
	- Several external animators and data analysis and visualization tools can be used with ns-3. However, users should expect to work at the command line and with C++ and/or Python software development tools.
	- <b style="color: red">ns-3 is primarily used on Linux or macOS systems. Windows users may also use a Linux virtual machine.</b>
- ns-3 is a research and educational simulator, by and for the research community. It will rely on the ongoing contributions of the community to develop new models, debug or maintain existing ones, and share results.
---
##  <center style= "background:#C9FFFF">:package: <i>2. Requirement</i></center>
### <center><span style="background:#FFF0D4; border-radius:9px; padding: 6px 270px;">**2.1 Used Language**</span></center>
<span style="background:#FFD4D4; border-radius:9px; padding: 6px 18px;">:arrow_forward: *Reference: [ns-3 quick-start](https://www.nsnam.org/docs/tutorial/html/quick-start.html#prerequisites)*</span>
-  The main features of ns-3 just require a C++ compiler (g++ or clang++), Python (version 3.6 or above), CMake and a build-system (e.g. make, ninja, Xcode).
#### **:arrow_right: Linux**
- For Linux, use either **g++ or clang++** compilers. 
#### **:arrow_right: macOS**
- For macOS, use **clang++** (available in Xcode or Xcode Command Line Tools). 
#### **:arrow_right: Windows**
- For Windows, Msys2 tools with the MinGW64 toolchain can be used (since ns-3.37) for most use cases. 
- For releases earlier than ns-3.37, or for use of emulation modes or Python bindings, we recommend to either use a Linux virtual machine, or the Windows Subsystem for Linux.

### <center><span style="background:#FFF0D4; border-radius:9px; padding: 6px 280px;">**2.2 Prerequisites**</span></center>
<span style="background:#FFD4D4; border-radius:9px; padding: 6px 18px;">:arrow_forward: *Reference: [ns-3 Getting Started](https://www.nsnam.org/docs/tutorial/html/getting-started.html#prerequisites)*</span>

![](https://hackmd.io/_uploads/S1bhqTaJp.png)

---
## <center style= "background:#C9FFFF">:page_facing_up: <i>3. Installation</i></center>
<span style="background:#FFD4D4; border-radius:9px; padding: 6px 18px;">:arrow_forward: *Reference: [ns-3 Getting Started](https://www.nsnam.org/docs/tutorial/html/getting-started.html#getting-started), [ns-3 installation](https://www.nsnam.org/docs/installation/html/linux.html)*</span>
### <center><span style="background:#FFF0D4; border-radius:9px; padding: 6px 280px;">**3.1 Environment**</span></center>
- Using VMware in Windows 

|        Ubuntu Version        | Disk | Memory |  CPU   |
|:----------------------------:|:----:|:------:|:------:|
| ubuntu-20.04.5-desktop-amd64 | 40G  |   4G   | 4 core |

### <center><span style="background:#FFF0D4; border-radius:9px; padding: 6px 260px;">**3.2 ns-3 install step**</span></center>
#### <span style="background:#B5FFB5; border-radius:9px; padding: 6px 11px;">**3.2.1 Install the Dependent Tools**</span>
```shell=
sudo apt-get update
# ns-3 prerequisites
sudo apt-get install -y git vim g++ python3-pip cmake     
# NR   prerequisites
sudo apt-get install -y libc6-dev sqlite sqlite3 libsqlite3-dev 
# gnuplot tool
sudo apt-get install gnuplot-x11
# PyViz visualizer prerequisites
sudo apt-get install gir1.2-goocanvas-2.0 python3-gi python3-gi-cairo python3-pygraphviz gir1.2-gtk-3.0 ipython3
# Python bindings prerequisites
python3 -m pip install --user cppyy
```
**Check the version of g++ and python.**
|  tool  | version |  command   |
|:------:| ------- |:----------:|
| python | 3.8.10  | python3 -V |
|  g++   | 9.4.0   |   g++ -v   |
#### <span style="background:#B5FFB5; border-radius:9px; padding: 6px 11px;">**3.2.2 Download source code from github** and **fetch the ns-3.39 related sourcecode.**</span>
```shell=
cd
mkdir workspace
cd workspace
git clone https://gitlab.com/nsnam/ns-3-allinone.git
cd ns-3-allinone
python3 download.py -n ns-3.39
```
#### <span style="background:#B5FFB5; border-radius:9px; padding: 6px 11px;">**3.2.3 Building ns-3**</span>
```shell=
./build.py
```
#### <span style="background:#B5FFB5; border-radius:9px; padding: 6px 11px;">**3.2.4 Download the NR module(version 5g-lena-v2.5.y).**</span>
```shell=
cd ns-3.39/contrib
git clone https://gitlab.com/cttc-lena/nr.git
cd nr
git checkout 5g-lena-v2.5.y
```
#### <span style="background:#B5FFB5; border-radius:9px; padding: 6px 11px;">**3.2.5 Configuration**</span>
```shell=
./ns3 configure --enable-examples --enable-tests --enable-python-bindings
```
#### <span style="background:#B5FFB5; border-radius:9px; padding: 6px 11px;">**3.2.6 Build all targets**</span>
```shell=
./ns3 build
```
#### <span style="background:#B5FFB5; border-radius:9px; padding: 6px 11px;">**3.2.7 Run NR example for test**</span>
```shell=
./ns3 run traffic-generator-example --vis
```
### <center><span style="background:#FFF0D4; border-radius:9px; padding: 6px 220px;">**3.3 NetAnim tool install step**</span></center>
#### <span style="background:#B5FFB5; border-radius:9px; padding: 6px 11px;">**3.3.1 Install the Dependent Tools**</span>
```shell=
# NetAnim prerequisites
sudo apt install -y qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools
```

#### <span style="background:#B5FFB5; border-radius:9px; padding: 6px 11px;">**3.3.2 Build NetAnim tool**</span>
```shell=
cd netanim-3.109
make clean
qmake NetAnim.pro
make
```

#### <span style="background:#B5FFB5; border-radius:9px; padding: 6px 11px;">**3.3.3 Execute NetAnim tool**</span>
```shell=
./NetAnim
```
---
## <center style= "background:#C9FFFF">:package: <i>4. Simulation</i></center>
:arrow_forward: *Reference: [ns-3 LTE models](https://www.nsnam.org/docs/models/html/lte-user.html), [利用NS3部署一个LTE网络](https://blog.csdn.net/Shmily1107/article/details/110919007?ops_request_misc=&request_id=&biz_id=102&spm=1018.2226.3001.4187)* 

### <center><span style="background:#FFF0D4; border-radius:9px; padding: 6px 260px;">**4.1 Mobility model**</span></center>
#### <span style="background:#B8B8FF; border-radius:9px; padding: 6px 11px;">**Configuration & Topology**</span>

| Parameter | Value |  description  |
|:---------:|:-----:|:-------------:|
|  enb_num  |   9   | Number of eNB |
|  ue_num   |  16   | Number of UE  |

|     Model      | Device |            Mondel name             | 
|:--------------:| ------ |:----------------------------------:|
| Mobility model | eNB    | ns3::ConstantPositionMobilityModel |
| Mobility model | UE     |   ns3::RandomWalk2dMobilityModel   | 

#### <span style="background:#B5FFB5; border-radius:9px; padding: 6px 11px;">**4.1.1 Create Mobility model simulation source code**</span>
==Put your UE an eNB figure into `./ns-3.39/scratch`==

```shell=
vim scratch/MobilityModel.cc
```
```cpp=
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
```
#### <span style="background:#B5FFB5; border-radius:9px; padding: 6px 11px;">**4.1.2 Run simulation**</span>
```shell=
./ns3 run MobilityModel
```
