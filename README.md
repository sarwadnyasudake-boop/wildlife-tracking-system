# 🐾 WILDLIFE TRACKING & SAFETY SYSTEM

<p align="center">
  <strong>TECHNOLOGY • NATURE • PROTECTION</strong>
</p>

<p align="center">
  <a href="#"><img src="https://img.shields.io/badge/PROJECT-WILDLIFE%20SAFETY-2E8B57?style=for-the-badge" /></a>
  <a href="#"><img src="https://img.shields.io/badge/DOMAIN-IoT-00A86B?style=for-the-badge" /></a>
  <a href="#"><img src="https://img.shields.io/badge/PLATFORM-ARDUINO-00979D?style=for-the-badge&logo=arduino&logoColor=white" /></a>
</p>

<p align="center">
  *Smart monitoring for a safer coexistence between humans and wildlife.*
</p>

---

## 01 / PROJECT OVERVIEW

> **The Challenge:** Monitoring wildlife movement over large forest areas is difficult. Timely location information can help researchers and forest departments improve animal monitoring and conservation efforts.

The **Wildlife Tracking & Safety System** is an embedded IoT project that combines GPS positioning and GSM communication to obtain animal location information and transmit it to a registered mobile number.

| Project Information | Details |
|:---|:---|
| Project Type | Embedded System / IoT |
| Main Controller | Arduino Uno |
| Communication | GSM |
| Positioning | GPS |
| Application | Wildlife Monitoring |
| Programming | Embedded C / Arduino |

## 02 / SYSTEM ARCHITECTURE

### How the system works

```mermaid
flowchart LR
    A["🛰️ GPS Module"] --> B["Arduino Uno"]
    B --> C["GSM Module"]
    C --> D["📱 Registered Mobile"]
    D --> E["📍 Location Information"]

    style A fill:#163D2A,color:#FFFFFF,stroke:#42D392
    style B fill:#163D2A,color:#FFFFFF,stroke:#42D392
    style C fill:#163D2A,color:#FFFFFF,stroke:#42D392
    style D fill:#163D2A,color:#FFFFFF,stroke:#42D392
    style E fill:#163D2A,color:#FFFFFF,stroke:#42D392
```

### Operational sequence

| Stage | Operation |
|:---:|:---|
| 01 | GPS receives satellite signals |
| 02 | Coordinates are obtained |
| 03 | Arduino processes location data |
| 04 | GSM transmits coordinates through SMS |
| 05 | Receiver accesses the location information |

## 03 / HARDWARE COMPONENTS

| Component | Function |
|:---|:---|
| Arduino Uno | Main processing unit |
| GPS Module | Latitude and longitude acquisition |
| SIM800L GSM | SMS communication |
| Antenna | Wireless signal reception/transmission |
| Power Supply | Provides electrical power |

## 04 / CORE CAPABILITIES

<details>
<summary><strong>📍 GPS-Based Location Monitoring</strong></summary>

Obtains geographical coordinates using GPS satellite signals to determine the monitored animal's location.
</details>

<details>
<summary><strong>📡 GSM Communication</strong></summary>

Uses the GSM network to transmit location information to a registered mobile number.
</details>

<details>
<summary><strong>📩 SMS Notifications</strong></summary>

Provides a simple communication method for sharing location information without requiring a dedicated mobile application.
</details>

<details>
<summary><strong>🌳 Wildlife Conservation Support</strong></summary>

Explores an affordable technology-based approach to wildlife monitoring and conservation.
</details>

## 05 / TECHNOLOGY STACK

<p align="center">
  <strong>HARDWARE</strong><br/>
  Arduino Uno &nbsp; • &nbsp; GPS &nbsp; • &nbsp; GSM SIM800L
</p>

<p align="center">
  <strong>SOFTWARE</strong><br/>
  Arduino IDE &nbsp; • &nbsp; Embedded C
</p>

<p align="center">
  <strong>COMMUNICATION</strong><br/>
  GPS Positioning &nbsp; • &nbsp; GSM Network &nbsp; • &nbsp; SMS
</p>

## 06 / FUTURE DEVELOPMENT

| Enhancement | Planned Capability |
|:---|:---|
| 🗺️ Google Maps | Direct location visualization |
| 📱 Mobile App | Convenient monitoring interface |
| 🚧 Geo-fencing | Boundary-crossing notifications |
| ☁️ Cloud Dashboard | Centralized monitoring |
| 🤖 AI Integration | Animal behaviour analysis |
| 🔋 Power Optimization | Longer operating duration |

## 07 / PROJECT VISION

```text
        MONITOR
           ↓
        LOCATE
           ↓
        COMMUNICATE
           ↓
        PROTECT
```

Our goal is to explore how embedded systems and IoT can contribute to wildlife conservation and safer human–wildlife coexistence.

---

## 08 / DEVELOPER

**SARVADNYA SUDAKE**

Electronics & Computer Engineering  
Embedded Systems | IoT | Digital Electronics

<p align="center">
  <a href="https://github.com/sarwadnyasudake-boop">
    <img src="https://img.shields.io/badge/GitHub-181717?style=for-the-badge&logo=github&logoColor=white"/>
  </a>
  <a href="https://www.linkedin.com/in/sarvadnya-sudake-717769358/">
    <img src="https://img.shields.io/badge/LinkedIn-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white"/>
  </a>
</p>

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  🌿 <em>Building technology to protect the natural world.</em> 🌿
</p>
