# 🧪 **Simulation Explanation – ns-3 Network Simulator**

This simulation models a WiFi environment with two Access Points (APs) and 32 Station (STA) nodes. STAs associate with the nearest AP, transmit traffic, and periodically change their position, triggering roaming and application reassignment. This document explains each milestone of the simulation process.

---

- [🧪 **Simulation Explanation – ns-3 Network Simulator**](#-simulation-explanation--ns-3-network-simulator)
  - [**Milestone 1: Deploy 2 Access Points (APs)**](#milestone-1-deploy-2-access-points-aps)
  - [**Milestone 2: Assign 16 STAs to Each AP**](#milestone-2-assign-16-stas-to-each-ap)
  - [**Milestone 3: Make STA Send Traffic Continuously**](#milestone-3-make-sta-send-traffic-continuously)
  - [**Milestone 4: STA Behavior at 10s and 20s**](#milestone-4-sta-behavior-at-10s-and-20s)
  - [**Milestone 5: Analysis Report – Output Files**](#milestone-5-analysis-report--output-files)
  - [**Analysis of output files and figures**](#analysis-of-output-files-and-figures)
    - [📄`sta_ip_mac_map.csv`](#sta_ip_mac_mapcsv)
    - [📄`sta_bssid_log.csv`](#sta_bssid_logcsv)
    - [📄`movement_log.csv`](#movement_logcsv)
    - [📄`ap_sta_count.csv`](#ap_sta_countcsv)
    - [📄`ap1_throughput.csv` and `ap2_throughput.csv`](#ap1_throughputcsv-and-ap2_throughputcsv)
  - [📌 **Summary**](#-summary)


---

## **Milestone 1: Deploy 2 Access Points (APs)**

Two APs are created and installed using `WifiHelper` and `ApWifiMac` setup.

**Code:**

```cpp
apNodes.Create(2);
...
mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
NetDeviceContainer apDevices = wifi.Install(phy, mac, apNodes);
```

**Location:** Lines ~68–72

**Explanation:**

- `apNodes.Create(2)` allocates two AP nodes.
- Each AP is assigned the same SSID (`"ssid"`) but they are distinguished by their BSSID (MAC address).
- BSSIDs are stored for later comparison during roaming events.

---

## **Milestone 2: Assign 16 STAs to Each AP**

The 32 STAs are statically positioned: 16 near AP1 (`x = 0`), and 16 near AP2 (`x = 10`).

**Code:**

```cpp
for (uint32_t i = 0; i < 32; ++i)
    posAlloc->Add(Vector(i < 16 ? 0.0 : 10.0, 0.0, 0.0));
```

**Location:** Line ~84

**Explanation:**

- STAs 0–15 are positioned at `x = 0` → within AP1 range.
- STAs 16–31 are positioned at `x = 10` → within AP2 range.
- A `RangePropagationLossModel` of 5m ensures each STA connects to its nearby AP.

---

## **Milestone 3: Make STA Send Traffic Continuously**

Each STA installs two `OnOffApplication`s, one targeting AP1 and the other AP2. Only one application runs at a time depending on the STA's current BSSID.

**Code:**

```cpp
OnOffHelper onoff1(... ap1Addr ...);
onoff1.SetConstantRate(DataRate("100kbps"), 125);
...
OnOffHelper onoff2(... ap2Addr ...);
...
staOnOffApps.emplace_back(app1, app2);
```

**Location:** Lines ~136–145

**Dynamic App Control:**

```cpp
void UpdateStaAppsBasedOnBssid() {
    ...
    if (currentBssid == ap1Bssid) {
        app1.Start(Simulator::Now());
    } else if (currentBssid == ap2Bssid) {
        app2.Start(Simulator::Now());
    }
}
```

**Location:** Line ~40

**Explanation:**

- Two UDP traffic generators per STA (only one active based on BSSID).
- 100 kbps constant rate per active STA.
- Switching logic handled dynamically in `UpdateStaAppsBasedOnBssid()` every second.

---

## **Milestone 4: STA Behavior at 10s and 20s**

At 10 and 20 seconds, some STAs move and associate with the other AP. The amount of movement is based on fixed ratios.

**Code:**

```cpp
Simulator::Schedule(Seconds(10.0), &MoveStasAtTime, 10.0, ...);
Simulator::Schedule(Seconds(20.0), &MoveStasAtTime, 20.0, ...);
```

**Location:** Lines ~152–153

**Mobility Logic:**

```cpp
if (time == 10.0)
    moveCountAp1 = ap1Stas.size() * 0.25;
else
    moveCountAp1 = ap1Stas.size() * 0.5;
```

**Location:** Lines ~31–34 in `MoveStasAtTime`

**Explanation:**

- At **10s**:
  - 25% of STAs near AP1 move to AP2
  - 50% of STAs near AP2 move to AP1

- At **20s**:
  - 50% of both groups switch again.

- Movement triggers a reassociation → monitored and redirected traffic in the app logic.

---

## **Milestone 5: Analysis Report – Output Files**

Multiple `.csv` files are generated for analysis:

| File | Purpose |
|------|---------|
| `ap1_throughput.csv`, `ap2_throughput.csv` | AP-specific throughput logged every 0.5s |
| `sta_bssid_log.csv` | Per-STA BSSID (association) over time |
| `sta_ip_mac_map.csv` | Maps STA ID to IP and MAC |
| `movement_log.csv` | Logs STA movement and new AP after 10s, 20s |
| `ap_sta_count.csv` | Number of associated STAs per AP every 1s |

**Logging Setup Example:**

```cpp
Simulator::Schedule(Seconds(0.5), &RecordThroughput);
Simulator::Schedule(Seconds(1.0), &LogStaBssids);
Simulator::Schedule(Seconds(1.0), &LogApStaCounts);
Simulator::Schedule(Seconds(1.0), &UpdateStaAppsBasedOnBssid);
```

**Location:** Lines ~156–159

---

## **Analysis of output files and figures**

To comprehensively evaluate the performance and behavior of the simulated WiFi environment, several output files were generated during runtime. These logs provide valuable insights into key metrics such as AP throughput, STA association dynamics, mobility-driven handovers, and traffic redirection. By analyzing these data sources and visualizing them through figures and plots, we can better understand the effectiveness of AP load balancing, the impact of STA mobility, and the overall network dynamics across the simulation timeline.

### 📄`sta_ip_mac_map.csv`

This file records the mapping between each Station (STA) node’s ID, its assigned IPv4 address, and the corresponding MAC address. It serves as a **reference table** for identifying individual STAs throughout the simulation and is especially useful when analyzing other logs such as:

- **BSSID association changes** in `sta_bssid_log.csv`
- **Mobility events** in `movement_log.csv`
- **Traffic direction and performance tracking**

By having a static record of STA identities, this file enables consistent cross-referencing between logs. For example, when a particular STA switches its associated AP during the simulation, this file allows researchers to trace back to the original STA ID and network identity, making the interpretation of mobility patterns and throughput impact clearer and more structured.

**Example usage:**

> If `movement_log.csv` reports that STA ID 5 moved from AP1 to AP2 at 10s, this map allows us to identify that STA 5 has IP `10.1.1.6` and MAC `00:00:00:00:00:06`, which can then be used to filter traffic records or match log entries in packet capture or performance analysis.

### 📄`sta_bssid_log.csv`

This file logs the **BSSID (Basic Service Set Identifier)** each STA is associated with over time. Each row represents a snapshot of the simulation at one-second intervals, starting from `t = 1s`. The first column indicates the timestamp, and each subsequent column corresponds to a specific STA’s IP address, showing the MAC address of the AP it is currently connected to.

**Purpose and Significance:**

- 🛰️ **Tracking STA–AP Associations:**  
  This log enables us to monitor how each STA’s association changes over time, especially after mobility events scheduled at 10s and 20s. When a STA switches from one AP to another, the corresponding BSSID in this file changes.

- 🔁 **Validating Handover Behavior:**  
  Since the simulation includes programmed mobility (e.g., 25% of AP1 STAs moving to AP2 at 10s), this file serves as the ground truth for validating whether those handovers occurred as expected.

- 🧠 **Driving Application Control:**  
  The data here also ties directly into the simulation logic, where STA application flows are toggled based on BSSID changes (see `UpdateStaAppsBasedOnBssid()` in the source code). The log offers visibility into how that logic behaves during runtime.

- 📊 **Analysis & Visualization:**  
  This file can be used to generate visualizations such as:
  - STA association timeline charts
  - Heatmaps of handover frequency
  - Correlation between STA movement and throughput changes

**Example Use Case:**

> If `STA 10.1.1.12` is connected to BSSID `00:00:00:00:00:21` (AP1) at `t=5s`, but switches to `00:00:00:00:00:22` (AP2) at `t=10s`, this indicates a successful handover, which can be cross-referenced with `movement_log.csv` and application-level logs.

### 📄`movement_log.csv`

This file records **STA mobility events**, specifically the time, STA identity (ID and IP address), and the new AP (Access Point) each STA moves toward. It captures **two scheduled mobility events** at simulation times **10 seconds and 20 seconds**, which are critical moments for evaluating AP handover behavior and its impact on network performance.

**Purpose and Significance:**

- 🚶‍♂️ **Documenting Controlled Mobility:**
  Each line reflects a programmed movement of a STA from its original location to the vicinity of the other AP. For instance, at `t=10s`, several STAs originally connected to AP1 were moved closer to AP2 and vice versa. This log captures those actions precisely.

- 🔁 **Supporting Handover Verification:**
  When correlated with `sta_bssid_log.csv`, this log helps verify whether the **BSSID association** actually changed in response to physical movement. A mismatch could reveal connectivity issues or delays in reassociation.

- 📶 **Enabling Performance Analysis:**
  By knowing which STAs moved and when, analysts can:
  - Observe **latency or packet loss** during handover transitions.
  - Analyze **AP load balancing** effects using `ap_sta_count.csv`.
  - Track **throughput changes** before and after movement using `ap1_throughput.csv` and `ap2_throughput.csv`.

- 📊 **Visualization & Traceability:**
  This data supports visualization of:
  - Handover timelines
  - Mobility-triggered event graphs
  - Per-STA connectivity history

**Example Use Case:**

> If `STA_ID 4` moved to AP2 at `t=10s`, and later back to AP1 at `t=20s`, this file enables correlation with throughput dips or flow reinitializations, and confirms the mobility logic's effectiveness.

### 📄`ap_sta_count.csv`

This file logs the **number of STAs (Stations) currently associated with each AP (Access Point)** at every one-second interval throughout the simulation. The log includes three columns: simulation time (in seconds), the count of STAs connected to AP1, and the count of STAs connected to AP2.

**Purpose and Significance:**

- 📊 **Real-Time Load Distribution Monitoring:**  
  This log enables real-time tracking of how STAs are distributed across APs. It is particularly valuable in scenarios where mobility or handover events are expected to affect AP load balance.

- 🔁 **Verifying Handover Effectiveness:**  
  When STAs move between APs (e.g., at 10s and 20s), this file confirms whether those handovers resulted in actual changes in association counts. For example:
  - At **t=12s**, AP1 has 20 STAs and AP2 has 12, indicating more STAs have joined AP1 after the first movement event.
  - At **t=22s**, both APs return to 16 STAs, suggesting a rebalancing of load after the second wave of movements.

- ⚖️ **Analyzing Load Balancing and Congestion:**  
  In WiFi networks, performance often depends on how evenly clients are distributed across APs. This file helps evaluate:
  - Whether load was shifted as intended
  - If an AP became overloaded temporarily
  - Whether STAs reassociated efficiently

- 🧩 **Cross-Referencing with Other Logs:**
  It complements:
  - `movement_log.csv` (STA movements)
  - `sta_bssid_log.csv` (BSSID association)
  - `ap1_throughput.csv` / `ap2_throughput.csv` (traffic performance)

**Example Use Case:**

> After 10s, multiple STAs move from AP2 to AP1. This file shows the resulting change in STA counts per AP, allowing you to determine if the network became imbalanced and whether that affected throughput or latency.

And We can use Python code to visualize files.
```python
# get csv
sta_count = pd.read_csv("ap_sta_count.csv")

# setting axis and plot
plt.figure(figsize=(10, 6))
plt.plot(sta_count['Time'], sta_count['AP1_STA_Count'], label='AP1 Connected STAs')
plt.plot(sta_count['Time'], sta_count['AP2_STA_Count'], label='AP2 Connected STAs')
plt.xlabel('Time (s)')
plt.ylabel('Number of Connected STAs')
plt.title('Number of Connected STAs Over Time')
plt.legend()
plt.grid(True)

# Adjust scale
plt.xticks(np.arange(0, 31, 1))  # 每秒
plt.yticks(np.arange(0, sta_count[['AP1_STA_Count', 'AP2_STA_Count']].values.max() + 1, 2))  # 每 2 個 STA

# store in png
plt.savefig("APs_sta_count_plot.png")
plt.close()
```
![APs_throughput_plot.png](../a2/result/APs_sta_count_plot.png)


### 📄`ap1_throughput.csv` and `ap2_throughput.csv`

These two files log the **instantaneous throughput (in Mbps)** received by each AP every **0.5 seconds** throughout the simulation. They capture fine-grained network performance metrics and reflect how STA association and mobility directly impact the APs' traffic loads.

Each file contains:
- `Time`: The simulation timestamp (in seconds)
- `Throughput(Mbps)`: The aggregate data received at the corresponding AP during the last 0.5-second interval

**Purpose and Significance:**

- 📈 **Monitoring Real-Time Network Performance:**  
  These logs allow for continuous monitoring of how much data each AP is receiving, which is crucial for evaluating network utilization and congestion patterns.

- 🔁 **Measuring Effects of STA Mobility:**  
  The simulation includes STA handovers at **10s** and **20s**, which cause dynamic load shifting between AP1 and AP2. These throughput logs help confirm:
  - Whether traffic is properly redirected after reassociation
  - How long it takes for throughput to stabilize
  - Whether any packet drops or performance degradation occurred during the transition

- ⚖️ **Analyzing Load Balancing Behavior:**  
  Changes in throughput correlate with STA count imbalances (see `ap_sta_count.csv`). For instance:
  - After `t = 10s`, AP1’s throughput spikes from ~1.6 Mbps to ~2 Mbps due to increased STA load (20 STAs).
  - Simultaneously, AP2’s throughput drops, reflecting the drop in associated clients (12 STAs).
  - After `t = 22s`, both APs return to ~1.6 Mbps, consistent with STA counts balancing to 16 each.

- 📊 **Visualization & Comparison:**  
  These logs are ideal for generating:
  - Line plots showing AP throughput over time
  - Before/after comparisons around mobility events
  - Correlation graphs between STA association and network performance

**Example Use Case:**

> By plotting both throughput files together, one can clearly visualize the performance impact of mobility events, identifying load spikes or dips and confirming that the simulation correctly handles dynamic traffic redirection based on STA-BSSID reassociation.

And We can use Python code to visualize files.
```python
# get csv
ap1 = pd.read_csv("ap1_throughput.csv")
ap2 = pd.read_csv("ap2_throughput.csv")

# setting axis and plot
plt.figure(figsize=(10, 6))
plt.plot(ap1['Time'], ap1['Throughput(Mbps)'], label='AP1')
plt.plot(ap2['Time'], ap2['Throughput(Mbps)'], label='AP2')
plt.xlabel('Time (s)')
plt.ylabel('Throughput (Mbps)')
plt.title('Per-AP Throughput Over Time')
plt.legend()
plt.grid(True)

# Adjust scale
plt.xticks(np.arange(0, 31, 1))  # 每 1 秒
plt.yticks(np.arange(0, ap1['Throughput(Mbps)'].max() + 0.5, 0.2))  # 每 0.2 Mbps

# store in png
plt.savefig("APs_throughput_plot.png")
plt.close()
```
![APs_throughput_plot.png](../a2/result/APs_throughput_plot.png)

## 📌 **Summary**

This simulation effectively demonstrates dynamic STA reassociation between APs and its impact on:

- Traffic redirection
- Real-time BSSID switching
- Per-AP throughput tracking
- Dynamic load balancing (via STA count tracking)

All logs serve as data sources for further analysis and visualization (e.g., throughput over time, BSSID switching timelines, etc.).
