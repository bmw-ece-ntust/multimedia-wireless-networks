## A2

- **Title**: ns-3 Network Simulator
- **Deadline**: 11th April 2025, 23.59 A.M.
- **1. Objectives**

  - **NS-3 Installation (30%)**: Provide an installation guide that includes:
    1. **Commands**: List terminal commands used for installation.
    2. **Verification**: Copy & paste terminal logs confirming successful installation.
    3. **Final Check**: Run [`hello-simulator.cc`](https://www.nsnam.org/docs/release/3.19/doxygen/hello-simulator_8cc_source.html) to verify installation.
  - **LTE / WiFi Simulation (50%)**:
    - **Submission Requirements**: Upload your simulation source code under `./a2/src/` in your branch. For each milestone, provide:
      1. **Input**: Commands or source code (attach hyperlinks from `./a2/src/`).
      2. **Output**: Figures, results, and explanations.
  - **Extra Credit (20%)**:
    - **Peer Review Bonus:** Top 3 students based on peer reviews receive extra credit: **Top 1:** +20%, **Top 2:** +15%, **Top 3:** +10%.
    - **Extended Deployment Bonus:** If you deploy **9 nodes in WiFi/LTE** and commit before **March 29, 2025, 19:00 PM**, you get an **extra 10%**.

- **2. Submission Guidelines**

  - Upload all source code under `./a2/src/` and include comments. Each milestone should include input commands/source code, output figures, and explanations/analysis. Reports should be uploaded in `assignment-2.md`.

- **3. Milestones & Grading**

  - **1. Deploy Two Nodes (10%)**: Base Station (**BS**) for LTE or Access Point (**AP**) for WiFi simulation.
  - **2. Assign 16 Users to Each Node (10%)**: 16 **User Equipments (UEs)** for LTE or **STAs** for WiFi per node. Provide a user table (e.g., MAC addresses or STA/UE IDs).
    - **Formula:**  
      $$ X_0 = Y_0 = 16 $$
    - **Definitions:** \( X_0 \) = Initial Users in Node A, \( Y_0 \) = Initial Users in Node B.
  - **3. Full-Queue Model Transmission (10%)**: Implement a [full-queue model](https://www.nsnam.org/doxygen/d9/db7/fcfs-wifi-queue-scheduler_8cc_source.html). Ensure each STA/UE continuously transmits to AP/BS.
  - **4. DCF & Throughput Analysis (10%)**: Implement **Distributed Coordination Function (DCF)**. Analyze throughput behavior.
  - **5. Mobility Model Simulation (10%)**:
    - **Simulation Duration:** 10 minutes.
    - **User Movement:**
      - **At minute 3:** 20% of STAs move from Node A → Node B, and 40% move from Node B → Node A.
      - **At minute 6:** 20% of STAs move from Node A → Node B, and 40% move from Node B → Node A.
    - **Formulas:**
      - **Minute 3:**  
        $$ X_1 = X_0 \times p, \quad Y_1 = Y_0 \times q $$
      - **Minute 6:**  
        $$ X_2 = X_1 \times p, \quad Y_2 = Y_1 \times q $$
    - **Definitions:**
      - \( p = 25\% \) → Users moving from Node A to Node B.
      - \( q = 50\% \) → Users moving from Node B to Node A.
      - \( X_1, X_2 \) → Users in Node A after each movement.
      - \( Y_1, Y_2 \) → Users in Node B after each movement.
  - **6. Final Report (10%)**:
    - **Final User Distribution:** How many users remain in Node A & Node B at the end?
    - **Throughput Analysis:** Provide a graph with **X-axis** = Time (minutes), **Y-axis** = Throughput, and two separate throughput lines for Node A & Node B.
    - **Video Demonstration:** Record the simulation activity & attach the hyperlink in your report. Ensure **file access is open**.

- **4. References**
  - [ns-3](https://www.nsnam.org/)
  - [ns-3 Tutorial](https://www.nsnam.org/docs/tutorial/html/)
  - [The ns-3 Network Simulator](https://intronetworks.cs.luc.edu/current/html/ns3.html)
  - [ns-3 LTE Module](https://www.nsnam.org/docs/models/html/lte-user.html)
  - [ns-3 WiFi Module](https://www.nsnam.org/docs/models/html/wifi-design.html)
  - [3GPP NR ns-3 Module](https://github.com/QiuYukang/5G-LENA)
