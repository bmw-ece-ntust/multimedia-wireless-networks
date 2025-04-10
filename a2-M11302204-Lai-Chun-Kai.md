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



## 3. Scenario

### 3-1 Network Topology
Formula
X₀ = Y₀ = 16
**Definitions:**
- `X₀`: Initial users in Node A
- `Y₀`: Initial users in Node B

### 3-2 Transmission Model
- **Full-Queue Model:**
  -  Each STA/UE continuously transmits to AP/BS.
  -  Ref: [FCFS WiFi Queue Scheduler](https://www.nsnam.org/doxygen/d9/db7/fcfs-wifi-queue-scheduler_8cc_source.html)
- **Duration:** 5 minutes = 300 Seconds

### 3-3 User Movement Schedule
---
#### 100th Seconds
- **Movement:**
  - 25% of users from Node A → Node B
  - 50% of users from Node B → Node A

- **Formula:**
  - X₁ = X₀ × (1 - p) + Y₀ × q
  - Y₁ = Y₀ × (1 - q) + X₀ × p

Where:
- `p = 25%`
- `q = 50%`
---
#### 200th Seconds
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


---
### 3-4 Execution steps
### 3-5 User Table
### 3-6 User Movement Log



---
