# ns-3 Study Note

- [ns-3 Study Note](#ns-3-study-note)
  - [Introduction NS-3](#introduction-ns-3)
    - [🔍 **What is NS-3?**](#-what-is-ns-3)
    - [✅ **Pros of NS-3**](#-pros-of-ns-3)
    - [❌ **Cons of NS-3**](#-cons-of-ns-3)
    - [🧾 **Use Cases for NS-3**](#-use-cases-for-ns-3)
  - [key abstract concepts](#key-abstract-concepts)
    - [Node](#node)
    - [Application](#application)
    - [Channel](#channel)
    - [Net Device](#net-device)
    - [Topology Helpers](#topology-helpers)
  - [Logging Module](#logging-module)
      - [Log Level](#log-level)
      - [Log Prefix](#log-prefix)
      - [Log usage](#log-usage)


---

## Introduction NS-3

### 🔍 **What is NS-3?**

**NS-3** is an open-source discrete-event network simulator designed for research and educational purposes. It supports detailed modeling of network protocols such as WiFi, LTE, 5G, and TCP/IP, and is implemented primarily in C++. NS-3 is widely used in academia for evaluating network performance, testing new protocols, and simulating large-scale network scenarios.

---

### ✅ **Pros of NS-3**

- 🎯 High fidelity simulation of real-world protocols (e.g., IEEE 802.11, LTE, 5G NR)
- 🧩 Modular design enables easy customization and protocol extension
- 🧪 Widely used in academic research for protocol analysis and performance comparison
- 🖥️ Real-time simulation support and integration with real networks (e.g., tap devices)
- 📚 Extensive documentation and example codes available for learning and reference

---

### ❌ **Cons of NS-3**

- 🧠 Steep learning curve due to required knowledge in C++ and networking fundamentals
- 🛠️ Complex environment setup and CLI-based workflow (no GUI)
- ⏱️ Scalability limitations for very large topologies (e.g., thousands of nodes)
- 🔌 Integration with real hardware or external controllers requires additional development effort

---

### 🧾 **Use Cases for NS-3**

- Academic research projects and theses (e.g., PhD, Master's)
- Simulation and evaluation of wireless technologies (WiFi, LTE, 5G, mmWave)
- Internet of Things (IoT) and smart city communication modeling
- Software-Defined Networking (SDN) and Network Function Virtualization (NFV) testing
- Network Digital Twin development and performance analysis

---

## key abstract concepts

![image](https://hackmd.io/_uploads/rkECDmPJA.png)

### Node
* In ns-3 nodes is a abstract basic computing device 
* This abstraction is represented in C++ by the class Node.
* Nodes just like a computer in real world which you will add functionality, like applications, protocol stacks and peripheral cards with their associated drivers to enable the computer to do useful work.
* In main function of the ns-3 script. We can use ```NodeContainer``` to create the ns-3 Node.
### Application
* In ns-3 the basic abstraction for a user program that generates some activity to be simulated is the application.
* This abstraction is represented in C++ by the class Application.
* Developers are expected to specialize the Application class in the object-oriented programming sense to create new applications.
* In ns-3 script we can use specific application helper(like ```UdpEchoServerHelper```) to create an application and use ```ApplicationContainer``` to install in nodes.
### Channel
* In ns-3, one connects a Node to an object representing a communication channel. Here the basic communication subnetwork abstraction is called the channel and is represented in C++ by the class Channel.
* The Channel class provides methods for managing communication subnetwork objects and connecting nodes to them.(CsmaChannel, PointToPointChannel and WifiChannel etc.)
* The specialized Channel can also model things as complicated as a large Ethernet switch, or three-dimensional space full of obstructions in the case of wireless networks.
* In ns-3 script we can use specific channel helper(like ```PointToPointHelper```) to create a channel, and we can config it by using this channel class.
### Net Device
* If you wanted to connect a computer to a network, you had to buy a specific kind of network cable and a hardware device called a peripheral card that needed to be installed in your computer.
* In ns-3, a net device is installed in a Node in order to enable the Node to communicate with other Nodes in the simulation via Channels.
* A Node may be connected to **more than one Channel via multiple NetDevices**.
* The net device abstraction is represented in C++ by the class NetDevice. 
* The NetDevice class provides methods for managing connections to Node and Channel objects; and may be specialized by developers in the object-oriented programming sense.
* The specific NetDevice is designed to work with a specific channel
    * the PointToPointNetDevice is designed to work with a PointToPointChannel 
* In ns-3 script we can use ```NetDeviceContainer``` to create a NetDevice, and use ```<specific NetDevice name>.Install (nodes)``` install in nodes.
### Topology Helpers
* Since connecting NetDevices to Nodes, NetDevices to Channels, assigning IP addresses, etc., are such common tasks in ns-3, we provide what we call topology helpers to make this as easy as possible.
* The above NodeContainer、ApplicationContainer、NetDeviceContainer and PointToPointHelper etc. are all Topology Helpers.
![image](https://hackmd.io/_uploads/r184pnqy0.png)


## Logging Module

* Many large systems support some kind of message logging facility, and ns-3 is not an exception.
* It can log lots of message like error message、warning message or info message etc., but the output will be messy quickly. So ns-3 provides an optional multi-level message recording method.
* Logging can be disabled completely, enabled on a component-by-component basis, or enabled globally; and it provides selectable verbosity levels.
#### Log Level
* ns-3 defined seven levels of log messages of increasing verbosity.
    * Levels from low to high : 
```cpp=
LOG_ERROR — Log error messages (associated macro: NS_LOG_ERROR);
LOG_WARN — Log warning messages (associated macro: NS_LOG_WARN);
LOG_DEBUG — Log relatively rare, ad-hoc debugging messages (associated macro: NS_LOG_DEBUG);
LOG_INFO — Log informational messages about program progress (associated macro: NS_LOG_INFO);
LOG_FUNCTION — Log a message describing each function called (two associated macros: NS_LOG_FUNCTION, used for member functions, and NS_LOG_FUNCTION_NOARGS, used for static functions);
LOG_LOGIC – Log messages describing logical flow within a function (associated macro: NS_LOG_LOGIC);
LOG_ALL — Log everything mentioned above (no associated macro).
```
* Each level can be used individually or in multiple
* Or we can use ```LOG_LEVEL_<type>``` to enables logging of all the levels above it in addition to it’s level.
* ns-3 also provide an unconditional logging macro that is always displayed, irrespective of logging levels or component selection.
```cpp=
NS_LOG_UNCOND – Log the associated message unconditionally (no associated log level).
```
#### Log Prefix
* ns-3 also provides log prefix show some info(simulation time、log level etc.) infront of the log message.
```cpp=
LOG_PREFIX_FUNC — The function where the output log is located
LOG_PREFIX_TIME — The time when the output log is generated (this time is the simulation time)
LOG_PREFIX_NODE — The node number where the output log is located
LOG_PREFIX_LEVEL — Output log level
LOG_PREFIX_ALL — Output all the above information
```
* Log level and log prefix can be use singal or together.
#### Log usage
* First we need to define a log component before main function.
```cpp=
NS_LOG_COMPONENT_DEFINE("<component name>");
```
* After defined a log component, we can use in the script.
* We can use below functions to enable/disable log component, and use log level to set log level of output.
    * Line 1/3 enable/disable the ```<name>``` component to output at the specified log level.
    * Line 2/4 enable/disable all the component to output at the specified log level.
```cpp=
void LogComponentEnable (char const *name, enum LogLevel level);
void LogComponentEnableAll (enum LogLevel level);
void LogComponentDisable (char const *name, enum LogLevel level);
void LogComponentDisableAll (enum LogLevel level);
```
* After this you can use ```LOG_<type>()```  、 ```LOG_level_<type>()``` etc. to output the log.
