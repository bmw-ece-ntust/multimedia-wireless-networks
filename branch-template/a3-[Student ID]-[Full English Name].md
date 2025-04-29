# ASSIGNMENT 3

- [ASSIGNMENT 3](#assignment-3)
- [Requirements](#requirements)
- [Log-1](#log-1)
  - [Basic Questions](#basic-questions)
  - [Advanced Questions (to gain extra points)](#advanced-questions-to-gain-extra-points)
- [Log-2](#log-2)
  - [Basic Questions](#basic-questions-1)
  - [Advanced Questions (to gain extra points)](#advanced-questions-to-gain-extra-points-1)

# Requirements

1. Install **KS-wireshark** (aka KS-WS) software on the laptop PC.
    **Note**: Use **KS-WS** to open any `*.lsu` logs.
2. Install **Wireshark** software on the laptop PC.
    **Note**: Use **Wireshark** to open any `*.pcap` logs.

---

# Log-1

- `2022_0411--E2E logs 3GPP TS33.511 4.2.2.1.1--DUT PASS`
- `2022_0411--UElog Reg ok PDU ok Ping complete.lsu`

---

## Basic Questions

1. What is the **cell PLMN** in logs?  
   _Please show the way to get the answer._

2. What is the **cell FR1 band** in logs?  
   _Please show the way to get the answer._

3. How to check if the **gNB** is **cellBarred** or **notBarred**?

4. What is the **MSIN of UE/Subscriber** in logs?

5. What is the **registration type** and **FOR**?  
   _Please show the reference of this answer._

6. What is the **DNN**?  
   _Please show the reference of this answer._

7. What is the **SCC mode** in 5G?  
   _Please show the way to get the answer._  
   Reference: [SSC Modes in 5G - Techplayon](https://www.techplayon.com/ssc-modes-session-and-service-continuity-in-5g/)

8. What is the **PDU address**?  
   _Please show the way to get the answer._

9. What is the **5QI** for this session?  
   _Please show the way to get the answer._

10. What is the **destination address** of **ICMP**?

---

## Advanced Questions (to gain extra points)

- What is the **period of MIB or SIB1**?  
  _Please show the way to get the answer._

- What is the **registration type** and **FOR**?  
  _Please show the way to get the answer._

- What is the **Network Slicing (S-NSSAI)** in UE?

- What is the **Periodic Registration Update timer**? (gain points ×3)  
  Reference: [Periodic Registration Timer - Cisco](https://www.cisco.com/c/en/us/td/docs/wireless/ucc/amf/2021-04/config-and-admin/b_ucc-5g-amf-config-and-admin-guide_2021-04/5G-AMF_Config_Guide_chapter_0111.pdf)

- What is the **AMBR in 5GC** for this UE? (gain points ×2)

- How many **ICMP missing replies**?  
  _Please show the way to get the answer._

---

# Log-2

- 2022_0411--5GCsimlog Reg ok PDU ok Ping is less but ok

## Basic Questions

1. What is the **cell AllowedNSSAI** in logs?  
   _Please show the way to get the answer._

2. What is the **RRCEstablishmentCause**?  
   _Please show the way to get the answer._

3. Show all **supportedBandListNR** for this UE in logs.  
   _Please show the way to get the answer._

---

## Advanced Questions (to gain extra points)

- Can this **5GC support Voice fallback to VoLTE**?  
  _Please show the way to get the answer._

- How to find the following **QosFlowSetupRequestList** in 5GC logs?  
  _Please show the way to get the answer._ (gain points ×2)
