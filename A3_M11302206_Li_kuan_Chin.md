# ASSIGNMENT 3

- [ASSIGNMENT 3](#assignment-3)
- [Requirements](#requirements)
- [Log-1](#log-1)
  - [L1 - Basic Questions](#l1---basic-questions)
  - [L1 - Advanced Questions (to gain extra points)](#l1---advanced-questions-to-gain-extra-points)
- [Log-2](#log-2)
  - [L2 - Basic Questions](#l2---basic-questions)
  - [L2 - Advanced Questions (to gain extra points)](#l2---advanced-questions-to-gain-extra-points)
- [KS-WS Filter Book](#ks-ws-filter-book)

# Requirements

1. Install **KS-wireshark** (aka KS-WS) software on the laptop PC.
    **Note**: Use **KS-WS** to open any `*.lsu` logs.
2. Install **Wireshark** software on the laptop PC.
    **Note**: Use **Wireshark** to open any `*.pcap` logs.
3. To answer all the questions below, show us:
   1. The filter to find the package
   2. Locate and expand the protocol subtree in the packat details pane, and then copy & paste into markdown text.

   **Example**:

   Q: Identify the AMF-UE-NGAP-ID of the logs.
   A:
      - AMF_UE_NGAP_ID == 0
      - filter: `ngap.AMF_UE_NGAP_ID == 0`

      ```text
      Frame 1998: 102 bytes on wire (816 bits), 102 bytes captured (816 bits) on interface unknown, id 0
      Ethernet II, Src: Meiko_02:06:94 (00:00:fc:02:06:94), Dst: VMware_89:8a:28 (00:0c:29:89:8a:28)
      Internet Protocol Version 4, Src: 192.168.50.102, Dst: 192.168.50.100
      Stream Control Transmission Protocol, Src Port: 38412 (38412), Dst Port: 38412 (38412)
      NG Application Protocol (PDUSessionResourceSetupResponse)
         NGAP-PDU: successfulOutcome (1)
            successfulOutcome
                  procedureCode: id-PDUSessionResourceSetup (29)
                  criticality: reject (0)
                  value
                     PDUSessionResourceSetupResponse
                        protocolIEs: 3 items
                              Item 0: id-AMF-UE-NGAP-ID
                                 ProtocolIE-Field
                                    id: id-AMF-UE-NGAP-ID (10)
                                    criticality: ignore (1)
                                    value
                                          AMF-UE-NGAP-ID: 0
      ```

---

# Log-1

- `2022_0411--E2E logs 3GPP TS33.511 4.2.2.1.1--DUT PASS`
- `2022_0411--UElog Reg ok PDU ok Ping complete.lsu`

---

## L1 - Basic Questions

**Total: 15 points**

1. What is the **cell PLMN** in logs? (**1.5 points**)

2. What is the **cell FR1 band** in logs? (**1.5 points**)

3. How to check if the **gNB** is **cellBarred** or **notBarred**? (**1.5 points**)

4. What is the **MSIN of UE/Subscriber** in logs? (**1.5 points**)

5. What is the **registration type** and (Follow-on request) **FOR**? (**1.5 points**)

6. What is the **DNN**? (**1.5 points**)

7. What is the **SCC mode** in 5G? (**1.5 points**)  
   Reference: [SSC Modes in 5G - Techplayon](https://www.techplayon.com/ssc-modes-session-and-service-continuity-in-5g/)

8. What is the **PDU address**? (**1.5 points**)

9. What is the **5QI** for this session? (**1.5 points**)

10. What is the **destination address** of **ICMP**? (**1.5 points**)

---

## L1 - Advanced Questions (to gain extra points)

**Total: 31 points**

1. What is the **period of MIB or SIB1**? (**4 points**)

2. What is the **registration type** and **FOR**? (**4 points**)

3. What is the **Network Slicing (S-NSSAI)** in UE? (**4 points**)

4. What is the **Periodic Registration Update timer**?  (**10 points**)  
   Reference: [Periodic Registration Timer - Cisco](https://www.cisco.com/c/en/us/td/docs/wireless/ucc/amf/2021-04/config-and-admin/b_ucc-5g-amf-config-and-admin-guide_2021-04/5G-AMF_Config_Guide_chapter_0111.pdf)

5. What is the **AMBR in 5GC** for this UE? (**7 points**)

6. How many **ICMP missing replies**? (**2 points**)

---

# Log-2

- 2022_0411--5GCsimlog Reg ok PDU ok Ping is less but ok

## L2 - Basic Questions

**Total: 10 points**

1. What is the **cell AllowedNSSAI** in logs? (**1.5 points**)

2. What is the **RRCEstablishmentCause**? (**1.5 points**)

3. What is the PDU address? (**1.5 points**)

4. What is the MSIN of UE/Subscriber in 5GC logs? (**1.5 points**)

---

## L2 - Advanced Questions (to gain extra points)

**Total: 44 points**

1. Can this **5GC support Voice fallback to VoLTE**? (**5 points**)

2. How to find the following **QosFlowSetupRequestList** in 5GC logs? (**9 points**)

3. Show all **supportedBandListNR** for this UE in logs. (**5 points**)

4. Since we got the IP address of PDU session in 5G RAN side, what is the associated TransportLayerAddress as well as the gtp-TEID in 5G CORE side? (**10 points**)

5. It is important to show the” TransportLayerAddress”, “GTP-TEID”, ”TEID”, “QFI” on same screen for both CP (Control Plane) and UP (User Plane). It would be easier for people to analyse the issues inside call flow.
Make sure to leave your digital signature on the filter list. (**15 points**)


---

# KS-WS Filter Book
version: 2025_0421

```text
# UL AMD PDU with Padding
(((nr_mac.sdu == 3f)) && (nr5g.chn_type == 5)) && (nr_mac.ft == 5)

# UL AMD PDU
(nr_rlc.lct == 6) && (nr5g.chn_type == 5)

# to show ** OVERLOAD ** in KS-WS
frame.len == 0

# 5G_RRC_NAS_Ping_UDG_CRC error_DCI_UEID
((nas_5gs || nr-rrc||icmp||udg) && lsu.userid==1)||(nr5g.CrcFlag == 0)||(nr5g.pkt_type == 2 and lsu.userid==1)

# 5G_RRC_NAS_Ping_UDG_CRC error_UEID
((nas_5gs || nr-rrc||icmp||udg) && lsu.userid==1)||(nr5g.CrcFlag == 0)

# CRC flag=CRC Fail
nr5g.CrcFlag == 0

#DCI & UEID
nr5g.pkt_type == 2 and lsu.userid==1

# UDG & UEID
udg and lsu.userid==1

#DCI, RRC, UDG & UEID
((nr5g.pkt_type == 2 || (nas_5gs || nr-rrc))&&lsu.userid==1) || udg
((nr5g.pkt_type == 2 || (nas_5gs || nr-rrc))&&lsu.userid==1) || udg ||icmp

# DL MAC - CRC Error
nr5g.crc_error

# MSG1-MSG2-MSG3-MSG4-MSG5
nr_mac.ft == 4 or (nr5g.pkt_type == 4) or (nr_mac.ft == 1) or (nr-rrc.rrcSetupRequest_element) or (nr-rrc.rrcSetupComplete_element) or (nr-rrc.rrcSetup_element)

# MSG1-MSG2-MSG3-MSG4-MSG5
((nr5g.pkt_type == 4 || nr-rrc)&& lsu.userid==1) || nr_mac.ft == 4

# 5G_RRC_NAS_Ping_UEID
((nas_5gs || nr-rrc||icmp) && lsu.userid==1)

# 5G_VoNR
(nas_5gs || nr-rrc||sip) && !lsu.userid==65535

# UE_ID and MSG2:
nr_mac.ft == 4 or lsu.userid==1
(nr_mac.ft == 4 ) || (lsu.userid==101)

# 5G_RRC_NAS_Ping_FTP_UEID
((nas_5gs || nr-rrc||icmp||ftp||ftp-data||tcp) && lsu.userid==1)

# SIB1
nr-rrc.c1 == 1

# MIB
nr5g.lct == 1

# RRC, NAS and UEID_1
(nas_5gs || nr-rrc) && lsu.userid==1

# RRC, NAS, Ping, UDG and UEDI_1
((nas_5gs || nr-rrc||icmp||udg) && lsu.userid==1)

# UDG with DL_PDCP
(udg) && (nr5g.pkt_type == 11)
```
