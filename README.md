# <center>Multimedia Wireless Networks (ET5907701) </center>

![bmw-logo](./assets/lab-logo.jpeg)

- [Multimedia Wireless Networks (ET5907701) ](#multimedia-wireless-networks-et5907701-)
  - [1. Introduction](#1-introduction)
    - [1.1. Person in Charge](#11-person-in-charge)
    - [1.2. Course Detail](#12-course-detail)
    - [1.3 Initial ToDo](#13-initial-todo)
  - [2. Course Outline](#2-course-outline)
    - [2.1. Objectives](#21-objectives)
    - [2.2. Schedule](#22-schedule)
    - [2.3. Evaluation](#23-evaluation)
    - [2.4. Assignments](#24-assignments)
      - [A1](#a1)
        - [A1-old](#a1-old)
        - [A1-new](#a1-new)
      - [A2](#a2)
      - [A3](#a3)
    - [2.5. Evaluation Criteria](#25-evaluation-criteria)
  - [References](#references)
  - [Samples](#samples)

## 1. Introduction

### 1.1. Person in Charge

- **Instructor**: Prof. Ray-Guang Cheng (<crg@gapps.ntust.edu.tw>)
- **Teaching Assistants**:
  1. Ian Joseph Chandra (<d11202805@gapps.ntust.edu.tw>)

### 1.2. Course Detail

1. [LINE group](https://line.me/ti/g/ykdO07UluN):
    <br>![line-group](./assets/LINE%20group%20Small.jpeg)
2. [Teaching materials](./Teaching%20Materials/).
3. Youtube [Playlist](https://www.youtube.com/playlist?list=PLNxlEcQdkOt_qHBq-W7T_eMtIJe3Ebryz) of course recordings.
4. [Teams Online course](https://www.microsoft.com/en-us/microsoft-teams/join-a-meeting):
    <br>**Note**: Register your [NTUST MS student account](https://www.cc.ntust.edu.tw/p/404-1050-83384.php?Lang=en) to get the course transcript & recordings.
   - Meeting ID: 484 576 276 678
   - Passcode: r4L9cd2U
5. [Q&A Google Form](https://forms.gle/FCPydtpv225nNDhz6).
  
  > [!IMPORTANT]  
  >
  > - This form is only opened during the course session (automatically closed outside the course session).
  > - Submit your Q&A **immediately** after raising or answering question in class.
  > - We won't count your Q&A outside the course session.

### 1.3 Initial ToDo

1. Create a [GitHub account](https://github.com/join).
2. Provide your information & gmail to be used in this course in this [Google form](https://forms.gle/szQXJkVMSAYxoKrD6).
3. Download [Visual Studio Code](https://code.visualstudio.com/download) (for study notes writing).
4. Install several VScode extensions:
   1. [Markdown All in One](https://marketplace.visualstudio.com/items?itemName=yzhang.markdown-all-in-one)
      1. Automatically generate & update the [table of content (ToC)](https://marketplace.visualstudio.com/items?itemName=yzhang.markdown-all-in-one#table-of-contents).
      2. [GitHub markdown features](https://docs.github.com/en/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax) information.
   2. Markdown Preview (Choose one):
      1. [Markdown Preview GitHub](http://marketplace.visualstudio.com/items?itemName=bierner.markdown-preview-github-styles)
      2. [Markdown Preview Enhanced](https://marketplace.visualstudio.com/items?itemName=shd101wyy.markdown-preview-enhanced)
   3. [GitHub Copilot](https://marketplace.visualstudio.com/items?itemName=GitHub.copilot)
   <br>Get free Copilot from [GitHub education](https://education.github.com/discount_requests/application).
   4. [GitHub Copilot Chat](https://marketplace.visualstudio.com/items?itemName=GitHub.copilot-chat)
5. Create your own branch in this repository using this format: `[year]-[student ID]-[English Name]`.
    <br>Ex: `2024-d11202805-Ian Joseph Chandra`
6. Copy the content of the [branch template](https://github.com/bmw-ece-ntust/multimedia-wireless-networks/tree/template) to your own branch & update your profile information in your README.md file.

<!-- ## Announcement (NEW!)
Dear all,
The slides for opening and Part-I are uploaded. Please refer to [Schedule](#Schedule).
Ray -->

## 2. Course Outline

### 2.1. Objectives

1. This course introduces the **basic knowledge of Quality-of-Service (QoS)**. Four main issues will be addressed:
    1. Multimedia Applications
    2. QoS Fundamentals
    3. QoS Mechanisms
    4. Selected Standards

  > [!NOTE]
  >We give an overview of the multimedia application. The fundamentals of Quality of Service (QoS) is then introduced. Some commonly used QoS Mechanisms will be given. Finally, we will use some popular wireless standards as examples to illustrate the usage of the QoS mechanisms. (The assessment for this part is through **Midterm Exam**).

2. **Interpersonal** & **teamwork** skills practice through 5 assignments.

  > [!TIP]
  > Readme First: [The jobs of the future – and two skills you need to get them](https://www.weforum.org/agenda/2016/09/jobs-of-future-and-skills-you-need)

  > [!NOTE]
  > **Conclussion**:
  > The study shows that workers who successfully combine mathematical and interpersonal skills in the knowledge-based economies of the future should find many rewarding and lucrative opportunities.

### 2.2. Schedule

| Date                   | Topic/Slide                                                                                                                                                                                                                                                                                                                                              | Presentations                                                                                    |
| :--------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------ |
| **Week 1**: 2/18       | [Opening](./Teaching%20Materials/2025_MultimediaWirelessNetworks.pdf)                                                                                                                                                                                                                                                                                                                                              |                                                                                                  |
| **Week 2**: 2/25       | [Part-I:](./Teaching%20Materials/Part-1_2024.pdf) Multimedia Applications                                                                                                                                                                                                                                                                                                                      |                                                                                                  |
| **Week 3**: 3/4        | QoS Fundamentals                                                                                                                                                                                                                                                                                                                                    | [Assignment 1](#a1-deadline-33-2359-am)                                                                                     |
| **Week 4**: 3/11       | QoS Fundamentals                                                                                                                                                                                                                                                                                                                                         |                                                                                                  |
| **Week 5**: 3/18       |  QoS Mechanisms                                                                                                                                                                                                                                                                                    |                                                                                                  |
| **Week 6**: 3/25       | QoS Mechanisms                                                                                                                                                                                                                                                                                                                                           | [Time Management](https://hackmd.io/YfiaqPIqSLO2Cj76zcUrrA)                                      |
| **Week 7**: 4/1        | QoS Mechanisms                                                                                                                                                                                                                                                                                                                                         |                                                                                                  |
| **Week 8**: 4/8        | QoS Mechanisms                                                                                                                                                                                                                                                                                                                                           | [Assignment 2](#a2-deadline-47-2359-am)                                                                                   |
| **Week 9**: 4/15       | Part-II: [Wireless Local Area Networks (WLANs)](), [Overview of 802.11 Networks](), [802.11 MAC]()                                                                                                                                                                                                                                                                                                                                           |                                                                                                  |
| **Week 10**: 4/22      | [802.11 MAC]()  | [Introduction to 802.11ax]()      |
| **Week 11**: 4/29      |                                                                                                                                                                                                                                                                                                                                              | 802.11 MAC                                                                                                 |
| **Week 12**: 5/6       | [802.11e](https://drive.google.com/file/d/1ik94M5TC_-1_C-LKtcRgO24c5oR-YDFq/view?usp=sharing)                                                                                                                                                                   |                                    |
| **Week 13**: 5/13      | 802.11e                                                                                                                                                                                                                                                    | [Assignment 3](#a3-deadline-513-2359-am)                                                                                   |
| **Week 14**: 5/20      | 5G QoS                                                                                                                                                                                                                                                                                                                              |                                                                                                  |
| **Week 15**: 5/27      |  5G QoS                                                                                                                                                                                                                                                                                                                                                |                                                                                                  |
| **Week 16**: 6/3       | **Final Exam**                                                                                                                                                                                                                                                                                                                                                   | |
| Juniper eLearning - I  | [Getting Started with Networking](https://learningportal.juniper.net/juniper/user_transcript.aspx)                                                                                                                                                                                                                                                       |                                                                                                  |
| Juniper eLearning - II | [Getting Started with Wi-Fi](https://learningportal.juniper.net/juniper/user_transcript.aspx)                                                                                                                                                                                                                                                            |                                                                                                  |

### 2.3. Evaluation

- **Q&A/Presentation: 15%**
  - Weekly: up to **15 pts** (1 pt/week)
- **[Assignments](#24-assignments): 45%**
- **Final Exam: 30%**. Materials:
  - QoS Fundamentals
  - QoS Mechanisms
  - 802.11/802.11e
  - 5G QoS
  - Assignments

>[!NOTE]
> The final score of this course can be adjusted to meet the requirements from our department:
>
> 研究所   標準平均：3.1 ~3.8    (百分數：77 ~ 85.67)
>
> Extra work for score adjustment will be given **after the Final Score is released**.

### 2.4. Assignments

#### A1

##### A1-old

- **Deadline**: 3/3 23.59 A.M.
- **Purpose**: Learn the background knowledge of interpersonal skills and apply it in the following assignments.
- **Interpersonal Skills**:
    1. 5-Minute Research Presentation ([[1]](https://gradcareers.cornell.edu/spotlights/tips-for-a-5-minute-research-presentation/),[[2]](https://www.youtube.com/watch?v=YVgS_opYacQ))
    2. [How to Read a Paper](https://web.stanford.edu/class/ee384m/Handouts/HowtoReadPaper.pdf)
    3. [How to Write a Report](https://www.openpolytechnic.ac.nz/current-students/study-tips-and-techniques/assignments/how-to-write-a-report/)
    4. [10 Tips for Better Teamwork](https://www.aces.edu/blog/topics/finance-career/10-tips-for-effective-teamwork-in-the-workplace/)
- **Score (15%)**:
  - Study note: 2 pt/item * 4 = **7 pt**;
  - Video (4~5 mins): 1 pt/item * 4 = **6 pt**;
  - Experience sharing (3~5 mins): **2 pt**

##### A1-new

- **Title:** Self-introduction
- **Deadline**: 3/3 23.59 A.M.
- **Purposes:**
  - Learn the background knowledge of interpersonal skills and apply it in the following assignments.
    - Introduce yourself (1pt),
    - Show us your goal (1pt),
    - Show us your study plan (1pt).
- **References:**
  - [How to Create a Study Schedule: 14 Steps](https://www.wikihow.com/Create-a-Study-Schedule)
  - [Time Management by Randy Pausch](https://www.youtube.com/watch?v=JKBFwR1HIFU)
- **Deliverables:**
  - Study note (40%)
  - Presentation video (5 mins) (40%)
  - Top 3 students (20%):
    <br>**Note** : [Mutual evaluation form](https://forms.gle/pmYkqqNMTczk3FW7A)
    - 1
    - 2
    - 3

#### A2

- **Title**: ns-3 Network Simulator
- **Deadline**: 4/7, 23.59 A.M.
- **Purposes:**
  - Part I: Study (30%)
    - Learn the basic concept of ns-3;
    - Install ns-3;
  - Part II: Simulate the LTE network using ns-3 (40%)
    - Execute an example for [LTE](https://www.nsnam.org/docs/models/html/lte-user.html)
    - Define and generate a network topology by yourself, e.g.
      - A square of 10 km x10 km.
      - The center point is (0,0)
      - 9 macro-cells are located in (5,5), (0, 5) (-5, 5), (5,0), (0,0),  (-5,0), (5,-5), (0, -5) (-5, -5). The cell radius of each macro cell is 5 km
      - 30 micro-cells, each has radius of 1 km, are randomly distributed in the square area
      - 50 UEs are randomly distributed in the square area
      - Explain the models you used in your simulation, e.g.,
        - mobility model
        - traffic model
        - channel model by yourself
      - Plot the performance metrics you simulated
- **References:**
  - [ns-3](https://www.nsnam.org/)
  - [ns-3 Tutorial](https://www.nsnam.org/docs/tutorial/html/)
  - [The ns-3 Network Simulator](https://intronetworks.cs.luc.edu/current/html/ns3.html)
  - [ns-3 LTE module](https://www.nsnam.org/docs/models/html/lte-user.html)
  - [3GPP NR ns-3 module](https://github.com/QiuYukang/5G-LENA)
- **Deliverables:**
  - **Part I (30%):**
    - Weekly Plan (Setup timeline for you to finish the goal above)
    - Network topology
    - Studying note
    - Presentation video
  - **Part II (40%):**
    - Demo video: Run a simulation for LTE Network
  - **Top 3 students (30%):**
    <br>**Note**: [Mutual evaluation form](https://forms.gle/pmYkqqNMTczk3FW7A)
    - 1
    - 2
    - 3

#### A3

- **Title**: ns-3 Open RAN
- **Deadline**: 5/13, 23.59 A.M.
- **References:**
  - Part I (30%): Study
    - Study the the [paper](https://arxiv.org/pdf/2209.14171.pdf),
      - Problems
      - Challenges and importances of the problem
      - Assumptions
      - Performance metrics
      - Basic concepts adopted to solve the problem
      - Results used to demonstrate that the problem is solved
  - Part II (40%): Use the tool [ns3-o-ran-e2](https://github.com/o-ran-sc/sim-ns3-o-ran-e2) to duplicate/demo the simulation scenario
- **Deliverables:**
  - **Part I (30%):**
    - Weekly Plan
    - Network topology
    - Studying note
    - Presentation video
    - [Mutual evaluation form](https://forms.gle/pmYkqqNMTczk3FW7A) ()
  - **Part II (40%):**
    - Demo video: Run a simulation for 5G NR
  - Top 3 (30%):
    <br>[Mutual evaluation form](https://forms.gle/pmYkqqNMTczk3FW7A)
    - 1
    - 2
    - 3

<!-- # 3. Paper Studies

## 3.1. Potential Topics for 
1. WiFi Simmulation using ns-3
2. Joint Communication and Sensing in WiFi

Reference:
1. [Learning the 802.11 Standard](https://blogs.arubanetworks.com/solutions/learning-the-802-11-standard/)
2. [RTI DDS Document](https://community.rti.com/static/documentation/connext-dds/5.2.0/doc/manuals/connext_dds/html_files/RTI_ConnextDDS_CoreLibraries_UsersManual/index.htm#UsersManual/AvailableDocs.htm%3FTocPath%3D_____2) -->

### 2.5. Evaluation Criteria

![image](https://hackmd.io/_uploads/S1lZwBZc1x.png)

## References

- [IEEE 802.11 Documents](https://mentor.ieee.org/802.11/documents?is_dcn=Random%20access&is_group=00be&is_options=1&is_year=2021)
- [IEEE 802.11be D 1.31](https://www.ieee802.org/11/private/Draft_Standards/11be/Draft%20P802.11be_D1.31.pdf)
- [Learning the 802.11 Standard](https://blogs.arubanetworks.com/solutions/learning-the-802-11-standard/)
- [Tricks to Read and Understand 3GPP Specifications](http://www.3glteinfo.com/tricks-to-read-and-understand-3gpp-specifications/)
- [Introduction to 802.11ax](https://www.tele.soumu.go.jp/resource/j/equ/mra/pdf/30/e/15.pdf)
- [Wi-Fi 6 OFDMA: Resource unit (RU) allocations and mappings](https://blogs.cisco.com/networking/wi-fi-6-ofdma-resource-unit-ru-allocations-and-mappings)
- [GitHub Markdown Features](https://docs.github.com/en/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax)
- [Ultralearning by Scott Young](https://www.scotthyoung.com/blog/ultralearning/)
- [Time Management by Randy Pausch](https://www.youtube.com/watch?v=JKBFwR1HIFU)

## Samples

- [2019](https://hackmd.io/PT5GnfcETu-eMZWV_DHWPQ?view&fbclid=IwAR3cCQZOzJSczTz4s53SW3EXwmHf_aFAmeC0ClxvgH5bB5KVIvMuCvh3nuk#Proposal-of-final-project)
- [2020](https://hackmd.io/74Xn924wT9mlqwTU4sYB1A#Final-Project)
