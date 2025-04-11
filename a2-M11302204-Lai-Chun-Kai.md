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
```shell
cd ~/ns-allinone-3.39/netanim-3.109
make clean
qmake NetAnim.pro
make
```
### 2-5 Run Example to Verification
Cpoy the `/ns-allinone-3.39/ns-3.39/examples/tutorial/hello-simulator.cc` to `/ns-allinone-3.39/ns-3.39/scratch/`
```shell
cd ~/ns-allinone-3.39/ns-3.39
./ns3 run scratch/hello-simulator.cc
```
![image](https://github.com/user-attachments/assets/b790ac0f-9d1e-4afa-bc52-99370b05bf95)



## 3. Scenario

### 3-1 Network Topology
Formula
X₀ = Y₀ = 16
**Definitions:**
- `X₀`: Initial users in Node A
- `Y₀`: Initial users in Node B
```cpp
// Create Node objects for the APs (formerly eNBs)
NodeContainer apNodes;
apNodes.Create(2); // 2 APs (Node A and Node B)

// Create Node objects for stations (formerly UEs) - 16 stations per AP, total 32 stations
NodeContainer staNodesA; // Stations for Node A
staNodesA.Create(16);
  
NodeContainer staNodesB; // Stations for Node B
staNodesB.Create(16);
.
.
.
// Configure stations for AP A
WifiMacHelper macSta;
macSta.SetType("ns3::StaWifiMac",
             "Ssid", SsidValue(ssid1),
             "ActiveProbing", BooleanValue(false));
NetDeviceContainer staDevsA = wifi.Install(phy, macSta, staNodesA);
  
// Configure stations for AP B
macSta.SetType("ns3::StaWifiMac",
             "Ssid", SsidValue(ssid2),
             "ActiveProbing", BooleanValue(false));
NetDeviceContainer staDevsB = wifi.Install(phy, macSta, staNodesB);
.
.
.
```
### 3-2 Transmission Model
- **Full-Queue Model:**
  -  Each STA/UE continuously transmits to AP/BS.
  -  Ref: [FCFS WiFi Queue Scheduler](https://www.nsnam.org/doxygen/d9/db7/fcfs-wifi-queue-scheduler_8cc_source.html)
- **Duration:** 30 Seconds
```cpp
.
.
.
// Setup applications for full-queue model (continuous traffic)
uint16_t port = 9;

// Create a sink (receiver) application on both APs
PacketSinkHelper packetSinkHelper("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
ApplicationContainer sinkApps;
sinkApps.Add(packetSinkHelper.Install(apNodes.Get(0))); // AP A
sinkApps.Add(packetSinkHelper.Install(apNodes.Get(1))); // AP B
sinkApps.Start(Seconds(1.0));
sinkApps.Stop(Seconds(30.0)); // Changed to 30s for the scenario

// Create OnOff Application for stations connected to AP A
OnOffHelper onOffHelperA("ns3::UdpSocketFactory", InetSocketAddress(apInterfaceA.GetAddress(0), port));
onOffHelperA.SetConstantRate(DataRate("20Mb/s")); // High data rate for full queue
onOffHelperA.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"));
onOffHelperA.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0.0]"));

ApplicationContainer clientAppsA;
for (uint32_t i = 0; i < staNodesA.GetN(); ++i) {
  clientAppsA.Add(onOffHelperA.Install(staNodesA.Get(i)));
}
clientAppsA.Start(Seconds(2.0));
clientAppsA.Stop(Seconds(30.0));
.
.
.
```
### 3-3 User Movement Schedule
---
#### 10th Seconds
- **Movement:**
  - 25% of users from Node A → Node B
  - 50% of users from Node B → Node A

- **Formula:**
  - X₁ = X₀ × (1 - p) + Y₀ × q
  - Y₁ = Y₀ × (1 - q) + X₀ × p

Where:
- `p = 25%`
- `q = 50%`

```cpp
.
.
.
// First movement at 10s - updated percentages to match scenario
// Move 25% from AP-A to AP-B and 50% from AP-B to AP-A
Simulator::Schedule(Seconds(10), &MoveUsers, 
                   std::ref(staNodesA), std::ref(staNodesB),
                   std::ref(staDevsA), std::ref(staDevsB), 
                   ssid1, ssid2, 0.25, 0.50, rng, std::ref(movementLog), "STA-A", "STA-B");
.
.
.
```
---
#### 20th Seconds
- **Movement:**
  - 50% of users from Node A → Node B
  - 50% of users from Node B → Node A

- **Formula:**
  - X₂ = X₁ × (1 - p₂) + Y₁ × q₂
  - Y₂ = Y₁ × (1 - q₂) + X₁ × p₂

Where:
- `p₂ = 50%`
- `q₂ = 50%`
- `X₁, X₂`: Users in Node A after each movement
- `Y₁, Y₂`: Users in Node B after each movement

```cpp
.
.
.
// CORRECTED: Second movement at 20s to restore exact balance of 16 users per AP
// Using more aggressive ratios based on actual user counts after first movement
Simulator::Schedule(Seconds(20), [&staNodesA, &staNodesB, &staDevsA, &staDevsB, ssid1, ssid2, &rng, &movementLog]() {
  // Calculate exact percentages needed to get 16 users at each AP
  uint32_t usersAtA = staNodesA.GetN();
  uint32_t usersAtB = staNodesB.GetN();
  
  // Calculate how many users to move
  int moveFromAtoB = std::max(0, (int)usersAtA - 16); // Move excess from A to B
  int moveFromBtoA = std::max(0, 16 - (int)usersAtA); // Move shortage from B to A
  
  // Calculate percentages
  double percentFromA = (usersAtA > 0) ? (double)moveFromAtoB / usersAtA : 0;
  double percentFromB = (usersAtB > 0) ? (double)moveFromBtoA / usersAtB : 0;
  
  std::cout << "Moving " << moveFromAtoB << " users from A to B and " 
            << moveFromBtoA << " users from B to A to achieve balance" << std::endl;
          
  // Call MoveUsers with calculated percentages
  MoveUsers(staNodesA, staNodesB, staDevsA, staDevsB, ssid1, ssid2, 
           percentFromA, percentFromB, rng, movementLog, "STA-A", "STA-B");
});
.
.
.
```

---
### 3-4 Execution steps
```shell
cd ~/ns-allinone-3.39/ns-3.39
./ns3 run scratch/assignment2.cc
```
![image](https://github.com/user-attachments/assets/4d2a981f-6818-4f18-9922-71386cfb2636)

### 3-5 User Table (Init)
```shell
Node ID	STA ID	MAC Address
Node A	1	00:00:00:00:00:03
Node A	2	00:00:00:00:00:04
Node A	3	00:00:00:00:00:05
Node A	4	00:00:00:00:00:06
Node A	5	00:00:00:00:00:07
Node A	6	00:00:00:00:00:08
Node A	7	00:00:00:00:00:09
Node A	8	00:00:00:00:00:0a
Node A	9	00:00:00:00:00:0b
Node A	10	00:00:00:00:00:0c
Node A	11	00:00:00:00:00:0d
Node A	12	00:00:00:00:00:0e
Node A	13	00:00:00:00:00:0f
Node A	14	00:00:00:00:00:10
Node A	15	00:00:00:00:00:11
Node A	16	00:00:00:00:00:12
Node B	17	00:00:00:00:00:13
Node B	18	00:00:00:00:00:14
Node B	19	00:00:00:00:00:15
Node B	20	00:00:00:00:00:16
Node B	21	00:00:00:00:00:17
Node B	22	00:00:00:00:00:18
Node B	23	00:00:00:00:00:19
Node B	24	00:00:00:00:00:1a
Node B	25	00:00:00:00:00:1b
Node B	26	00:00:00:00:00:1c
Node B	27	00:00:00:00:00:1d
Node B	28	00:00:00:00:00:1e
Node B	29	00:00:00:00:00:1f
Node B	30	00:00:00:00:00:20
Node B	31	00:00:00:00:00:21
Node B	32	00:00:00:00:00:22
```
### 3-6 User Movement Log
![image](https://github.com/user-attachments/assets/60379575-3983-4098-afd0-68e78f59249f)

### 3-7 Performance Analysis
Here is a [simple python code](https://github.com/bmw-ece-ntust/multimedia-wireless-networks/blob/2025-m11302204-Lai-Chun-Kai/a2/src/analysis.py) to create the picture about User Movement and Throughput Over Time
```shell
python3 analysis.py
```
![image](https://github.com/user-attachments/assets/f91138b3-8eab-4e44-81b5-ab4420f26316)
Figure1: Throughput Over Time
> [!NOTE]
> **Initial Phase (0–10 seconds):**
Both APs start with zero throughput at 0 seconds, as traffic begins at 2 seconds.
Throughput gradually rises to 15–20 Mbps per AP by 10 seconds, reflecting 16 STAs per AP generating 20 Mbps each. The aggregate throughput is limited by 802.11ac channel capacity, contention, and interference.

> [!NOTE]
> First Movement Phase (10–20 seconds):
At 10 seconds, 4 STAs move from Node-A to Node-B, and 8 STAs move from Node-B to Node-A, yielding 20 STAs at Node-A and 12 STAs at Node-B.
Node-A’s throughput increases sharply to 35–40 Mbps due to the higher STA count (20 STAs × 20 Mbps = 400 Mbps demand, capped by capacity).
Node-B’s throughput initially peaks at 30 Mbps but declines to 25–30 Mbps, consistent with fewer STAs (12 STAs × 20 Mbps = 240 Mbps demand).

> [!NOTE]
> Second Movement Phase (20–30 seconds):
At 20 seconds, the corrective movement rebalances the network to 16 STAs per AP 
Node-A’s throughput decreases to 30–35 Mbps, while Node-B’s increases slightly to 25–30 Mbps, stabilizing as the load evens out.
A slight throughput disparity may persist due to positional differences or interference.

> [!NOTE]
> Overall Trend (0–30 seconds):
Node-A generally achieves higher peak throughput (~40 Mbps) than Node-B (~30 Mbps), possibly due to its location at (0, 0, 0) experiencing less interference.
Throughput fluctuates due to random STA mobility and continuous traffic, with a ceiling imposed by the 802.11ac standard and contention.

![image](https://github.com/user-attachments/assets/6cfeb70e-1875-4d64-b2d7-a0cbd54c2453) 
Ref: Ming's simulation about 3mins

Because the limitation of my computrer is 30 seconds simulation. So, I want to campare with Ming Result about the influence between short and long simulation.
* Short Simulation (30 seconds):
Advantages: Captures initial transient and steady-state behavior, computationally feasible, and provides a quick throughput snapshot (Node A: 38-40 Mbps, Node B: 28-30 Mbps).
Limitations: Misses migration events (e.g., at 50 and 125 seconds), potentially overestimating stability (Node A) or underestimating degradation (Node B). Long-term trends and rare events (e.g., contention effects) are not observed.
* Long Simulation (175 seconds, "Ming Result"):
Advantages: Offers a comprehensive view, including migration impacts, long-term stability, and statistical reliability. Reveals dynamic responses (e.g., AP A dropping to 5 Mbps, AP B rising to 20 Mbps) critical for assessing network adaptability.
Limitations: Exceeds the user’s computational capacity, requiring more resources.
* Direct Comparison:
The 30-second simulation reflects baseline performance but lacks the dynamic events seen in the 175-second "Ming Result."
Node A’s stability in 30 seconds (38-40 Mbps) contrasts with AP A’s variability in 175 seconds (10-20 Mbps pre-migration, 5 Mbps post-migration), suggesting configuration differences or event impacts.
Node B’s slight decline (28-30 Mbps) might persist or stabilize in a longer run, as seen in AP B’s post-migration behavior (10-15 Mbps).

---

### 3-8 Performance Evaluation
**Throughput Efficiency:**
* Average throughput per STA is approximately:
  * Node-A: 40Mbps/20STAs = 2Mbps
  * Node-B: 25Mbps/12STAs = 2.08Mbps

-> This is far below the **20 Mbps per STA** traffic rate, indicating significant congestion and contention in the full-queue scenario.

---
**Mobility Impact**
- **First movement (10 seconds):**  
  Node-A’s load increases → throughput ↑  
  Node-B’s load decreases → throughput ↓

- **Second movement (20 seconds):**  
  Network rebalances → throughput stabilizes but not fully equalizes  → Suggests **spatial or environmental influences**

### 3-8 Discussion
The throughput trends align with the simulation’s design: user mobility directly affects AP load and throughput. Node-A’s higher throughput post-10 seconds may reflect better channel conditions at (0, 0, 0) compared to Node-B at (500, 0, 0). The random walk mobility model introduces variability, evident in throughput fluctuations, particularly for Node-B. The corrective movement at 20 seconds effectively balances STA counts, but the persistent throughput gap suggests influences beyond user numbers, such as interference, distance.

### 3-9 For the Future
> [!WARNING]
> 1. Interference Mitigation: Investigate interference at Node-B’s location (500, 0, 0) and consider channel allocation or AP repositioning to reduce disparities.
> 2. Rate Adaptation Optimization: Tune `MinstrelHtWifiManager` parameters to better handle high-density, high-traffic conditions and improve throughput efficiency.
> 3. Realistic Mobility Models: Replace the random walk model with one incorporating obstacles or user behavior for more practical insights.
> 4. Proactive Load Balancing: Implement algorithms to preemptively redistribute STAs based on real-time throughput or signal strength, enhancing performance stability.

## 4. Conclusion
The simulation demonstrates the impact of user mobility on Wi-Fi network throughput using 802.11ac. Throughput increases with STA density (e.g., Node-A at 20 STAs) and stabilizes after rebalancing (16 STAs per AP), though disparities persist due to spatial and environmental factors. The analysis underscores the challenges of congestion and contention in high-traffic scenarios, offering actionable recommendations to enhance network design and performance in real-world deployments.
