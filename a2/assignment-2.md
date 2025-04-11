# NS3 Network Simulator

<aside>
📖 **References**

- [ns-3](https://www.nsnam.org/)
- [ns-3 Tutorial](https://www.nsnam.org/docs/tutorial/html/)
- [The ns-3 Network Simulator](https://intronetworks.cs.luc.edu/current/html/ns3.html)
- [ns-3 LTE Module](https://www.nsnam.org/docs/models/html/lte-user.html)
- [ns-3 WiFi Module](https://www.nsnam.org/docs/models/html/wifi-design.html)
- [3GPP NR ns-3 Module](https://github.com/QiuYukang/5G-LENA)
</aside>

<aside>
✅ Score (15%): 

- [x]  **NS-3 Installation (30%)**: Provide an installation guide that includes:
    1. **Commands**: List terminal commands used for installation.
    2. **Verification**: Copy & paste terminal logs confirming successful installation.
    3. **Final Check**: Run [`hello-simulator.cc`](https://www.nsnam.org/docs/release/3.19/doxygen/hello-simulator_8cc_source.html) to verify installation.
- [x]  **LTE / WiFi Simulation (50%)**:
    - [x]  **Submission Requirements**: Upload your simulation source code under `./a2/src/` in your branch. For each milestone, provide:
        1. **Input**: Commands or source code (attach hyperlinks from `./a2/src/`).
        2. **Output**: Figures, results, and explanations.
    - [ ]  **Extra Credit (20%)**:
        - **Peer Review Bonus:** Top 3 students based on peer reviews receive extra credit: **Top 1:** +20%, **Top 2:** +15%, **Top 3:** +10%.
        - **Extended Deployment Bonus:** If you deploy **9 nodes in WiFi/LTE** and commit before **March 29, 2025, 19:00 PM**, you get an **extra 10%**.

**3. Milestones & Grading**

- [x]  **Deploy 2 nodes and assign users** (`1-points`):
    1. **Deploy 2 nodes and assign users** (`1-points`):
        1. Deploy 2 nodes as Node A & B;
        2. Assign 16 users for each node:
            - For LTE is called User Equipment (UE)
            - For WiFi is called Station (STA)
        3. Provide a user table (e.g., MAC addresses or STA/UE IDs).
        
        This setup can be formulated as follow:
        
        - **Formula:**
            
            X0=Y0=16
            
        - **Definitions:**
            - (X0) = Initial Users in Node A
            - (Y0) = Initial Users in Node B
- [x]  **Full-Queue Model Transmission & Random mobility Model**
    1. **Full-Queue Model Transmission & Random mobility Model**
        
        Implement a [full-queue model](https://www.nsnam.org/doxygen/d9/db7/fcfs-wifi-queue-scheduler_8cc_source.html). Ensure each STA/UE continuously transmits to AP/BS. (`1-point`)
        
        - **Simulation Duration:** 10 minutes.
        - **User Movement:**
            - **At minute 3:** 25% of users move from Node A → Node B, and 50% from Node B → Node A.
            - **At minute 6:** 50% of users move from Node A → Node B, and 50% from Node B → Node A.
            - For each movement, describe which users that moved from A --> B and B --> A in your .
        - **Formulas:**
            - **Minute 3:** ,
                
                X1=X0×p
                
                Y1=Y0×q
                
            - **Minute 6:**,
                
                X2=X1×p
                
                Y2=Y1×q
                
        - **Definitions:**
            - (p = 25%) → Users moving from Node A to Node B.
            - (q = 50% ) → Users moving from Node B to Node A.
            - (X1,X2) → Users in Node A after each movement.
            - (Y1,Y2) → Users in Node B after each movement.
- [x]  **Analysis Report**:
    1. What is the final users distribution (value of  & )? Show the users table for each movement. (`2-points`)
    2. Show & analyze the figures of the aggregated throughput in Node-A & Node-B through time (`3-points`):
        - X-axis = time & Y-axis = throughput.
        - Analyze & explain your results.
- [x] Show the 10-minutes video simulation (`3-points`)
```
</aside>

## 🧩 1. NS-3 Installation Guide (1 point)

**System:**  
- OS: Ubuntu 24.04 LTS (VirtualBox VM)  
- RAM: 8192 MB  
- CPUs: 4  
- Disk: 50 GB  
- Acceleration: Nested Paging, KVM Paravirtualization  
- Network: NAT (Intel PRO/1000 MT Desktop)

**Installation Steps:**
```bash
sudo apt update
sudo apt install -y git g++ python3 python3-pip cmake
git clone https://gitlab.com/nsnam/ns-3-dev.git ns-3
cd ns-3
./ns3 configure --enable-examples --enable-tests
./ns3 build
```

✅ NS-3 version: [insert version from terminal if needed]  
🧪 Test run: `./ns3 run hello-simulator` — verified output.

---

## 📡 2.  WiFi Simulation (2 points)

### Code Location  
Simulation source code: [`./a2/src/mySimulation.cc`](./a2/src/mySimulation.cc)

### Input: Compile and Run  
```bash
cd ns-3
./ns3 run mySimulation
```

### Output:
- Include key figures and graphs here (e.g., throughput vs time).
- You can use `Gnuplot`, `matplotlib`, or save logs to `.csv`.

#### Example Output (placeholder):
```
Simulation started...
UE attached to eNodeB
Received packets: 1024
Average throughput: 2.5 Mbps
```

---

## 📊 3. Analysis Report (8 points)

- **Network Topology**: Describe UE, eNB, EPC, WiFi AP, and backhaul setup.
- **Throughput Analysis**: Explain how throughput varies over time.
- **Latency Trends**: If simulated.
- **Resource Allocation & Interference**: Any spectrum sharing insights?
- **Limitations**: What’s simplified in the model?
- **Improvement Suggestions**: Eg. mobility models, fading channels, handover support.
- **Screenshots/Logs**: Add charts or terminal outputs.

```bash
git clone https://gitlab.com/nsnam/ns-3-dev.git
cd ns-3-dev
git checkout -b ns-3.39-branch ns-3.39
