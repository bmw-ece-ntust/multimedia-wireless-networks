# Assignment 2: WiFi Network Simulation Analysis
Student ID: M11302209
Name: HSU, Ming-Hong

## 1. NS-3 Installation Guide

### Installation Commands
```bash
# Download NS-3
cd ~
wget https://www.nsnam.org/release/ns-allinone-3.44.tar.bz2
tar xjf ns-allinone-3.44.tar.bz2
cd ns-allinone-3.44

# Build NS-3
./build.py --enable-examples --enable-tests
```

### Installation Verification
The installation was verified by running the hello-simulator example:
```bash
cd ns-3.44
./ns3 run hello-simulator
```

## 2. WiFi Simulation Implementation

### Network Configuration
The simulation implements a WiFi network with the following specifications:

1. **Network Topology**:
   - 2 WiFi Access Points (AP A and AP B)
   - 32 total stations (STAs)
   - Initial distribution: 16 STAs per AP

2. **Physical Setup**:
   - AP A located at (-50, 0)
   - AP B located at (50, 0)
   - STAs initially distributed in a 10m radius around each AP

3. **Network Parameters**:
   - WiFi Standard: IEEE 802.11n
   - Channel bandwidth: 20 MHz
   - Packet size: 1024 bytes
   - Transmission interval: 10ms

### Implementation Details

1. **Base Network Setup** ([Source](./a2/src/wifi-handover-simulation.cc)):
   ```cpp
   // Create nodes
   NodeContainer wifiStaNodes;
   wifiStaNodes.Create(32);
   NodeContainer wifiApNodes;
   wifiApNodes.Create(2);
   ```

2. **WiFi Configuration**:
   ```cpp
   WifiHelper wifi;
   wifi.SetStandard(WIFI_STANDARD_80211n);
   ```

3. **Mobility Implementation**:
   ```cpp
   MobilityHelper mobility;
   mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                               "MinX", DoubleValue(-50.0),
                               "MinY", DoubleValue(0.0),
                               "DeltaX", DoubleValue(100.0));
   ```

4. **Traffic Generation**:
   ```cpp
   UdpClientHelper client;
   client.SetAttribute("Interval", TimeValue(Seconds(0.01)));
   client.SetAttribute("PacketSize", UintegerValue(1024));
   ```

### User Movement Implementation

1. **Initial State (0-180s)**:
   - AP A: 16 users
   - AP B: 16 users

2. **First Movement (180s)**:
   - 4 users (25%) move from AP A to B
   - 8 users (50%) move from AP B to A
   - New distribution: AP A = 20 users, AP B = 12 users

3. **Second Movement (360s)**:
   - 10 users (50%) move from AP A to B
   - 6 users (50%) move from AP B to A
   - Final distribution: AP A = 16 users, AP B = 16 users

## 3. Analysis Results

### 1. Network Performance Analysis

The simulation results demonstrate the impact of user mobility on network throughput:

#### Initial State (0-180s)
- **AP A**: 0.802 Mbps
- **AP B**: 0.802 Mbps
- Total Network Throughput: 1.605 Mbps
- Even distribution with 16 users per AP

#### First Movement (180-360s)
- 4 users moved from AP A to B
- 8 users moved from AP B to A
- **AP A**: 0.782 Mbps (2.5% decrease)
- **AP B**: 0.795 Mbps (0.9% decrease)
- Total Network Throughput: 1.577 Mbps
- New distribution: AP A (20 users), AP B (12 users)

#### Second Movement (360-600s)
- 10 users moved from AP A to B
- 6 users moved from AP B to A
- **AP A**: 0.764 Mbps (4.8% total decrease)
- **AP B**: 0.787 Mbps (1.9% total decrease)
- Total Network Throughput: 1.552 Mbps
- Final distribution: AP A (16 users), AP B (16 users)

### 2. Key Observations

1. **Throughput Stability**:
   - Base throughput is consistently maintained around 0.8 Mbps per AP
   - Minor throughput degradation observed during handovers
   - System demonstrates resilience to user redistribution

2. **Handover Impact**:
   - Each handover event causes a small decrease in throughput
   - First handover: 1.7% total throughput reduction
   - Second handover: Additional 1.6% reduction
   - Total impact: 3.3% throughput reduction from initial state

3. **Load Balancing**:
   - Despite uneven interim distributions, the network maintains stable performance
   - Final even distribution shows the system's ability to handle dynamic user allocation

### 3. Visual Analysis

The simulation visualization demonstrates:
- Clear distinction between AP zones (red nodes)
- User movement patterns (green/blue nodes)
- Real-time throughput variations
- Handover events at 180s and 360s

[Link to simulation video recording]
git clone https://gerrit.o-ran-sc.org/r/o-du/phy.git
## 4. Running the Simulation

1. **Compile and Run**:
   ```bash
   cd ns-3.44
   ./ns3 run scratch/wifi-handover-simulation
   ```

2. **Generate Analysis**:
   ```bash
   cd ~/multimedia-wireless-networks/a2/src
   python3 analyze_results.py
   ```

3. **View Results**:
   - Throughput plots are saved as `throughput_analysis.png`
   - Summary statistics are saved in `throughput_summary.txt`
   - PCAP files are generated for detailed packet analysis

## References

1. NS-3 Documentation
   - [NS-3 WiFi Module Documentation](https://www.nsnam.org/docs/models/html/wifi.html)
   - [NS-3 Mobility Models](https://www.nsnam.org/docs/models/html/mobility.html)

2. IEEE Standards
   - IEEE 802.11n-2009 Standard
   - WiFi Alliance Specifications