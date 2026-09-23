# 🚪 FingerDoor - ESP32 Biometric Fingerprint Door Lock

[![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-orange.svg?logo=platformio)](https://platformio.org/)
[![ESP32](https://img.shields.io/badge/Microcontroller-ESP32-red.svg?logo=espressif)](https://www.espressif.com/)
[![Framework](https://img.shields.io/badge/Framework-Arduino-blue.svg?logo=arduino)](https://www.arduino.cc/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

An intelligent, secure biometric access control and door lock system powered by the **ESP32** microcontroller. **FingerDoor** combines optical fingerprint recognition, a 0.96" SSD1306 OLED graphical interface, a rotary encoder with multi-button input for navigation, physical relay door lock actuation, and persistent WiFi network management stored in non-volatile flash memory (NVS).

---

## ✨ Features

- 🔒 **Biometric Authentication & Relay Actuation**: High-speed optical fingerprint matching via hardware UART2 (`Serial2`). Upon positive match, triggers an active-HIGH door strike relay (`GPIO 4`) for 4 seconds.
- 👑 **Admin vs. User Role Segregation**:
  - **Admin Slots (`1–9`)**: Authorized to unlock the door AND access the protected on-screen Admin Menu.
  - **User Slots (`10–127`)**: Authorized only to unlock the door.
- 🎛️ **Interactive Admin Menu**:
  - Protected by biometric admin authentication.
  - **Add User**: Automatically assigns the next available slot (`10–127`).
  - **Add Admin**: Automatically assigns the next available admin slot (`2–9`).
  - **Delete Finger**: Visual rotary encoder selector to erase any template slot (`1–127`).
  - **WiFi Setup**: Scans and configures 2.4 GHz WiFi networks with on-screen keyboard.
  - **Clear WiFi**: Resets stored NVS WiFi credentials and reboots.
- 🖥️ **Rich OLED UI Feedback**:
  - Real-time idle screen with WiFi connection status and scanning prompts.
  - Step-by-step visual enrollment feedback (`Place Finger`, `Sample Taken`, `Remove Finger`, `Place Again`, `Success`).
  - Instant access status screens (`ACCESS GRANTED - User/Admin #ID` vs `ACCESS DENIED`).
- 📶 **Interactive On-Screen WiFi Setup**:
  - Live scanning of surrounding 2.4 GHz WiFi networks with signal strength indicators.
  - Multi-mode on-screen keyboard (uppercase letters, lowercase letters, numbers, and symbols) driven by rotary encoder with long-press backspace.
  - WiFi credentials saved persistently using ESP32 `Preferences` (NVS flash memory).
- 🛡️ **Fault Tolerance & Safety**:
  - Non-blocking button debouncing with a safety timeout (prevents lockups from stuck buttons or disconnected pins).
  - Sensor fault detection with on-screen diagnostic prompts and auto-recovery.
  - WiFi connection stability logic that prevents reconnection thrashing.

---

## 🛠️ Hardware Requirements

| Component | Description | Recommended Model |
| :--- | :--- | :--- |
| **Microcontroller** | ESP32 Development Board (30-pin or 38-pin) | ESP32-WROOM-32 / DevKit V1 |
| **Fingerprint Sensor** | Optical Fingerprint Reader Module (UART) | R307 / FPM10A / DY50 / AS608 |
| **Display** | 0.96" Monochrome I2C OLED Display (128x64) | SSD1306 (Address: `0x3C`) |
| **Door Actuator** | Solenoid Door Strike / Magnetic Lock + Relay | 5V Relay Module + 12V Solenoid |
| **Rotary Encoder** | Incremental Rotary Encoder with Push Button | KY-040 or EC11 |
| **Push Buttons** | Momentary tactile push buttons (Active LOW) | 3x Push Buttons (Ok, Swap, Select) |
| **Power Supply** | Regulated 5V DC Power Source | 5V / 2A Power Adapter |

---

## 📌 Pinout & Wiring Connections

### 1. Complete Pin Assignment Table
| Function / Component | ESP32 Pin | Logic Level | Notes |
| :--- | :--- | :--- | :--- |
| **Door Relay / Solenoid** | `GPIO 4` | Active HIGH (3.3V / 5V) | Triggers lock mechanism for 4s |
| **Fingerprint Sensor TX** | `GPIO 16` (RX2) | 3.3V TTL | Connects to Sensor Green wire (TX) |
| **Fingerprint Sensor RX** | `GPIO 17` (TX2) | 3.3V TTL | Connects to Sensor White wire (RX) |
| **OLED I2C SCL** | `GPIO 22` | 3.3V | Clock line |
| **OLED I2C SDA** | `GPIO 21` | 3.3V | Data line |
| **Rotary Encoder A (CLK)** | `GPIO 33` | Internal Pullup | Quadrature signal A |
| **Rotary Encoder B (DT)** | `GPIO 25` | Internal Pullup | Quadrature signal B |
| **Button 1 (Select / Char)** | `GPIO 26` | Internal Pullup | Short: Select / Char, Long: Backspace |
| **Button 2 (Swap Mode / Back)** | `GPIO 27` | Internal Pullup | Toggles input modes (A/a/0/!) or Exits |
| **Button 3 (OK / Menu)** | `GPIO 14` | Internal Pullup | Confirms input / Launches Admin Menu |

---

## 🗂️ Project Structure

```
fingerDoor/
├── include/
│   ├── admin_finger.h      # Admin & user slot management and deletion
│   ├── config.h            # Pinouts, slot boundaries, hardware & UI prototypes
│   ├── enroll_finger.h     # 2-step fingerprint enrollment engine with OLED UI
│   ├── finger_scan.h       # Sensor initialization, matching & access control
│   ├── menu.h              # Admin menu navigation and execution logic
│   └── wifi_setting.h      # WiFi scanning, keyboard input & NVS storage
├── src/
│   ├── admin_finger.cpp    # Admin vs User slot logic & first-run provisioning
│   ├── enroll_finger.cpp   # Guided multi-step enrollment state machine
│   ├── finger_scan.cpp     # Sensor communication & unlock triggering
│   ├── main.cpp            # Setup(), loop(), relay control & OLED dashboards
│   ├── menu.cpp            # Interactive rotary encoder admin menu
│   └── wifi_setting.cpp    # WiFi management with safe debouncing
├── .gitignore              # Git ignore rules for build artifacts & IDE files
├── platformio.ini          # PlatformIO build configuration & library dependencies
└── README.md               # Project documentation
```

---

## 🎮 How to Operate

### 1. First Boot (Initial Setup)
1. **Power on** the ESP32.
2. The OLED displays a welcome screen: `FINGERDOOR v2.0`.
3. If no WiFi credentials exist, you will be prompted:
   - Press **[OK]** (`GPIO 14`) to scan and connect to WiFi immediately.
   - Press **[Swap]** (`GPIO 27`) to skip and proceed to door operations.
4. **Admin Setup**: If no administrator fingerprints are registered in slots 1–9, the system guides you to enroll **Admin #1**:
   - `Step 1/2`: Place your finger on the optical sensor.
   - `Sample Taken`: Lift your finger off the sensor.
   - `Step 2/2`: Place the same finger again to confirm.
   - `Success`: Admin #1 is stored, and the door lock is armed!

### 2. Normal Scanning & Unlocking
- The display shows: `READY TO SCAN - PLACE FINGER ON SENSOR`.
- Place any registered finger on the sensor.
- If recognized:
  - Display flashes: `ACCESS GRANTED - User/Admin #ID - DOOR UNLOCKED`.
  - The relay on `GPIO 4` activates for 4 seconds, unlocking the door.
- If not recognized:
  - Display flashes: `ACCESS DENIED - Fingerprint Not Recognized`.

### 3. Entering the Admin Menu
1. Press the **OK** button (`GPIO 14`) during idle mode.
2. The display prompts: `Scan Admin Finger to access menu...`.
3. Place a finger registered in slots `1–9`.
4. The **Admin Menu** opens:
   - Rotate the encoder to scroll through options (`Add User`, `Add Admin`, `Delete Finger`, `WiFi Setup`, `Clear WiFi`, `Exit`).
   - Press **Select** (`GPIO 26`) or **OK** (`GPIO 14`) to execute.
   - Press **Swap** (`GPIO 27`) anytime to exit back to the lock screen.

---

## 🚀 Building & Flashing

### Prerequisites
- [PlatformIO IDE](https://platformio.org/install/ide?install=vscode) in VS Code or [PlatformIO Core CLI](https://docs.platformio.org/page/core.html).

### Build Firmware
```bash
pio run
```

### Upload to ESP32
```bash
pio run --target upload
```

### Serial Monitor
```bash
pio device monitor --baud 115200
```

---

## 📦 Library Dependencies

Configured automatically via `platformio.ini`:
- [`adafruit/Adafruit Fingerprint Sensor Library`](https://github.com/adafruit/Adafruit-Fingerprint-Sensor-Library)
- [`adafruit/Adafruit SSD1306`](https://github.com/adafruit/Adafruit_SSD1306)
- [`adafruit/Adafruit GFX Library`](https://github.com/adafruit/Adafruit-GFX-Library)
- [`maffooclock/ESP32RotaryEncoder`](https://github.com/MaffooClock/ESP32RotaryEncoder)

---

## 📄 License

This project is open-source under the [MIT License](LICENSE).
