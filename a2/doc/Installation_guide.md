
# ns-3 Installation Guide

---

- [ns-3 Installation Guide](#ns-3-installation-guide)
  - [Reference:](#reference)
  - [0. Environment](#0-environment)
  - [1. Install the Dependent Tools](#1-install-the-dependent-tools)
    - [1.1 Install ns-3 Prerequisites Tools](#11-install-ns-3-prerequisites-tools)
    - [1.2 Install PyViz visualizer prerequisites](#12-install-pyviz-visualizer-prerequisites)
  - [2. Install ns-3.42](#2-install-ns-342)
    - [Check ns-3 Release](#check-ns-3-release)
    - [2.1 Download source code](#21-download-source-code)
    - [2.2 Building ns-3](#22-building-ns-3)
    - [2.3 Configuration](#23-configuration)
    - [2.4 Build all targets](#24-build-all-targets)
  - [3. Install NR Module](#3-install-nr-module)
    - [3.1 Install Prerequisites Tools](#31-install-prerequisites-tools)
    - [3.2 Install NR Module](#32-install-nr-module)
    - [3.3 Configuration](#33-configuration)
    - [3.4 Build all targets](#34-build-all-targets)
  - [3. Run example and Test](#3-run-example-and-test)
    - [3.1 first.cc](#31-firstcc)
    - [3.2 NR Example](#32-nr-example)
    - [3.3 `hello-simulator.cc`](#33-hello-simulatorcc)
  - [4. Install gnuplot tools](#4-install-gnuplot-tools)


---

## Reference:

* [ns-3 Installation Guide](https://www.nsnam.org/docs/release/3.42/installation/html/index.html#ns-3-installation-guide)
* https://hackmd.io/@2xIzdkQiS9K3Pfrv6tVEtA/S1LpzA51p#-3-Installation

---

## 0. Environment
* Hardwarequests：
    * RAM：8G RAM
    * CPU：6 core
    * Disk：50G Storage
* Installation Environment：
    * Host：Windows 11
    * Hypervisor：VMware Workstation 17 Player
    * VM：Ubuntu 20.04 LTS (Focal Fossa)

---

## 1. Install the Dependent Tools

Update latest information and list of software : 
```shell=
sudo apt-get update
```

### 1.1 Install ns-3 [Prerequisites Tools](https://www.nsnam.org/docs/release/3.42/installation/html/quick-start.html#prerequisites)

As of the most recent ns-3 release (ns-3.42), **the following tools are needed to get started with ns-3:**
![image](https://hackmd.io/_uploads/SkR8G3v_C.png)

Install
```shell=
sudo apt-get install -y git vim g++ python3-pip cmake ninja-build
```

Check the tools version

| Tool    | Version Check Command |
|:------- |:--------------------- |
| g++     | ```g++ --version```   |
| python3 | ```python3 -V```      |
| cmake   | ```cmake --version``` |
| ninja   | ```ninja --version``` |

* g++ : 
    * **9.4.0**
    * ![Version_g++](https://hackmd.io/_uploads/BJyGWn5OC.png)
* python3
    * **3.8.10**
    * ![Version_python3](https://hackmd.io/_uploads/BkqGb25O0.png)
* cmake
    * **3.16.3**
    * ![Version_cmake](https://hackmd.io/_uploads/BJWmW35_R.png)
* ninja
    * **1.10.0**
    * ![Version_ninja](https://hackmd.io/_uploads/rJDX-29dC.png)

### 1.2 Install PyViz visualizer prerequisites
* NS-3 PyViz is a live simulation visualizer, meaning that it uses no trace files.
* It can be most useful for debugging purposes, i.e. to figure out if mobility models are what you expect, where packets are being dropped, etc.

```shell=
# PyViz visualizer prerequisites
sudo apt-get install gir1.2-goocanvas-2.0 python3-gi python3-gi-cairo python3-pygraphviz gir1.2-gtk-3.0 ipython3
# Python bindings prerequisites
python3 -m pip install --user cppyy
```


## 2. Install ns-3.42
### [Check ns-3 Release](https://www.nsnam.org/releases/)
### 2.1 Download source code
```shell=
cd
mkdir workspace
cd workspace
git clone https://gitlab.com/nsnam/ns-3-allinone.git
cd ns-3-allinone
python3 download.py -n ns-3.42
```

### 2.2 Building ns-3
```shell=
./build.py
```

### 2.3 Configuration
```shell=
cd ns-3.42
./ns3 configure --enable-examples --enable-tests --enable-python-bindings
```
Modules configured to be built (No NR Module): 

![build_no_nr](https://hackmd.io/_uploads/rkuuQT5dR.png)


### 2.4 Build all targets
```shell=
./ns3 build
```

## 3. [Install NR Module](https://5g-lena.cttc.es/)
* 5G-LENA is an open-source 5G New Radio (NR) network simulator, designed as a pluggable module to ns-3.
* The 5G-LENA simulator enables end-to-end simulations from the application layer down to the physical layer.

### 3.1 Install Prerequisites Tools
```shell=
sudo apt-get install -y libc6-dev sqlite sqlite3 libsqlite3-dev
```

### 3.2 Install NR Module
```shell=
cd ns-3.39/contrib
git clone https://gitlab.com/cttc-lena/nr.git
cd nr
git checkout 5g-lena-v3.1.y
```
NR version and ns-3 version : 

![NR_module_V](https://hackmd.io/_uploads/BJzpNT5d0.png)

### 3.3 Configuration
```shell=
./ns3 configure --enable-examples --enable-tests --enable-python-bindings
```
Modules configured to be built (With NR Module): 
![build_with_nr](https://hackmd.io/_uploads/HJmBIp5_A.png)


### 3.4 Build all targets
```shell=
./ns3 build
```





## 3. Run example and Test

### 3.1 first.cc
* Now we can run the ns3 example simulations to test ns-3
    * there we run the first.cc
    * we can run others examples

```shell=
./ns3 run /examples/tutoriol/first.cc
```

### 3.2 NR Example
* we can run the NR example to test ns-3 and NR module.

```shell=
./ns3 run traffic-generator-example
```

### 3.3 `hello-simulator.cc`
```shell=
./ns3 run examples/tutorial/hello-simulator.cc 
```

![image](https://hackmd.io/_uploads/SkwYws8RJx.png)


## 4. Install [gnuplot tools](http://www.gnuplot.info/)
* It created to allow scientists and students to visualize mathematical functions and data interactively.
* It also support many non-interactive uses such as web scripting.
* supports many different types of 2D and 3D plots.

```shell=
sudo apt-get install gnuplot
```
