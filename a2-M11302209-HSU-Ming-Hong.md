# NS-3 Installation

## 1. Commands
```bash
# Update package lists and install ns-3 (adjust as necessary for your setup)
sudo apt-get update
sudo apt-get install ns3
```

## 2. Verification
```bash
# Check the installed ns-3 version
ns3 --version

# Example terminal log output:
# ns-3 version 3.33
# Compiled with gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
```

## 3. Final Check
```bash
# Run hello-simulator to verify the installation
ns3 hello-simulator
```

Ensure that the output confirms successful execution.

# LTE / WiFi Simulation

## 1. Deploy Two Nodes (10%)
- Deploy a Base Station (BS) for LTE or an Access Point (AP) for WiFi simulation.

## 2. Assign 16 Users to Each Node (10%)
- Provide 16 User Equipments (UEs) for LTE or STAs for WiFi per node.
- **Formula:**  
  $$ X_0 = Y_0 = 16 $$
- **Definitions:**  
  - \( X_0 \): Initial Users in Node A  
  - \( Y_0 \): Initial Users in Node B

## 3. Full-Queue Model Transmission (10%)
- Implement a [full-queue model](https://www.nsnam.org/doxygen/d9/db7/fcfs-wifi-queue-scheduler_8cc_source.html) ensuring each STA/UE continuously transmits to the AP/BS.

## 4. DCF & Throughput Analysis (10%)
- Implement the Distributed Coordination Function (DCF) and analyze throughput behavior.

## 5. Mobility Model Simulation (10%)
- **Simulation Duration:** 10 minutes.
- **User Movement:**  
  - **At minute 3:** 20% of STAs move from Node A → Node B, and 40% move from Node B → Node A.  
  - **At minute 6:** 20% of STAs move from Node A → Node B, and 40% move from Node B → Node A.
- **Formulas:**  
  - Minute 3:  
    $$ X_1 = X_0 \times p, \quad Y_1 = Y_0 \times q $$
  - Minute 6:  
    $$ X_2 = X_1 \times p, \quad Y_2 = Y_1 \times q $$
- **Definitions:**  
  - \( p = 25\% \): Users moving from Node A to Node B  
  - \( q = 50\% \): Users moving from Node B to Node A

## 6. Final Report (10%)
- **Final User Distribution:** Report the remaining number of users in Node A and Node B.
- **Throughput Analysis:** Provide a graph with:
  - **X-axis:** Time (minutes)
  - **Y-axis:** Throughput  
  Include separate throughput lines for Node A and Node B.
- **Video Demonstration:** Record the simulation activity and attach the hyperlink in your report (ensure file access is open).