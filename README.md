# 🚪 FingerDoor - ESP32 Biometric Fingerprint Door Lock

[![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-orange.svg?logo=platformio)](https://platformio.org/)
[![ESP32](https://img.shields.io/badge/Microcontroller-ESP32-red.svg?logo=espressif)](https://www.espressif.com/)
[![Framework](https://img.shields.io/badge/Framework-Arduino-blue.svg?logo=arduino)](https://www.arduino.cc/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

An intelligent, secure biometric access control and door lock system powered by the **ESP32** microcontroller. **FingerDoor** combines optical fingerprint recognition, a 0.96" SSD1306 OLED graphical interface, a rotary encoder with multi-button input for navigation, and persistent WiFi network management stored in non-volatile flash memory (NVS).

---

## ✨ Features

- 🔒 **Biometric Authentication**: Fast optical fingerprint enrollment, verification, and template matching using high-speed hardware UART (`Serial2`).
- 👑 **Admin & User Role Management**: Automated first-run admin provisioning (Slot #1) and multi-slot enrollment routines.
- 🎛️ **Rotary Encoder User Interface**: Interactive on-screen menu navigation and character selection using interrupt-driven rotary encoder control.
- 🖥️ **OLED Graphical Display**: High-contrast 128x64 I2C display showcasing system status, network scan results, and input prompts.
- 📶 **Interactive On-Screen WiFi Setup**:
  - Live scanning of surrounding 2.4 GHz WiFi networks with signal strength indicators.
  - Multi-mode on-screen keyboard (uppercase letters, lowercase letters, numbers, and special symbols) driven by rotary encoder.
  - WiFi credentials saved persistently using ESP32 `Preferences` (NVS flash memory, surviving reboots and power outages).
- ⚡ **Modular PlatformIO Architecture**: Migrated from legacy Arduino IDE sketch files into clean, decoupled C++ source and header files.

---

## 🛠️ Hardware Requirements

| Component | Description | Recommended Model |
| :--- | :--- | :--- |
| **Microcontroller** | ESP32 Development Board (30-pin or 38-pin) | ESP32-WROOM-32 / DevKit V1 |
| **Fingerprint Sensor** | Optical Fingerprint Reader Module (UART) | R307 / FPM10A / DY50 / AS608 |
| **Display** | 0.96" Monochrome I2C OLED Display (128x64) | SSD1306 (Address: `0x3C`) |
| **Rotary Encoder** | Incremental Rotary Encoder with Push Button | KY-040 or EC11 |
| **Push Buttons** | Momentary tactile push buttons (Active LOW) | 3x Push Buttons (Ok, Swap, Select) |
| **Door Actuator** | Solenoid Lock / Relay Module *(Optional)* | 12V Solenoid Strike + 5V Relay |
| **Power Supply** | Regulated 5V DC Power Source | 5V / 2A Power Adapter |

---

## 📌 Pinout & Wiring Connections

### 1. Fingerprint Sensor (UART2)
| Sensor Pin | ESP32 Pin | Notes |
| :--- | :--- | :--- |
| **VCC** | `5V` (or `3.3V`) | Check sensor operating voltage |
| **GND** | `GND` | Common ground |
| **TX** | `GPIO 16` (RX2) | Sensor transmit to ESP32 receive |
| **RX** | `GPIO 17` (TX2) | ESP32 transmit to sensor receive |

### 2. SSD1306 OLED Display (I2C)
| Display Pin | ESP32 Pin | Notes |
| :--- | :--- | :--- |
| **VCC** | `3.3V` | 3.3V supply |
| **GND** | `GND` | Common ground |
| **SCL** | `GPIO 22` | Default I2C Clock |
| **SDA** | `GPIO 21` | Default I2C Data |

### 3. Rotary Encoder & Input Buttons
| Component | Function / Label | ESP32 Pin | Config Setting |
| :--- | :--- | :--- | :--- |
| **Rotary Encoder** | Channel A (CLK) | `GPIO 33` | `DI_ENCODER_A` |
| **Rotary Encoder** | Channel B (DT) | `GPIO 25` | `DI_ENCODER_B` |
| **Push Button 1** | Select / Add Character | `GPIO 26` | `switchPin` (Internal Pullup) |
| **Push Button 2** | Mode Swap (A/a/0/!) | `GPIO 27` | `switchPinSwap` (Internal Pullup) |
| **Push Button 3** | OK / Confirm / Advance | `GPIO 14` | `switchPinOk` (Internal Pullup) |

---

## 🗂️ Project Structure

```
fingerDoor/
├── include/
│   ├── admin_finger.h      # Admin enrollment & validation routines
│   ├── config.h            # Pin assignments, hardware objects & shared state
│   ├── enroll_finger.h     # 2-step fingerprint enrollment engine
│   ├── finger_scan.h       # Sensor initialization & template verification
│   ├── menu.h              # Admin menu rendering & item declarations
│   └── wifi_setting.h      # WiFi scanning, credential entry & NVS storage
├── src/
│   ├── admin_finger.cpp    # Admin fingerprint logic
│   ├── enroll_finger.cpp   # Enrollment state machine
│   ├── finger_scan.cpp     # Scanning & matching implementation
│   ├── main.cpp            # Setup(), loop(), and global object definitions
│   ├── menu.cpp            # Menu UI rendering
│   └── wifi_setting.cpp    # WiFi UI & Preferences storage implementation
├── .gitignore              # Git ignore rules for build artifacts & IDE files
├── platformio.ini          # PlatformIO build configuration & library dependencies
└── README.md               # Project documentation
```

---

## 🔧 Refactoring & Fixes Applied

During the migration from legacy Arduino IDE `.ino` files to PlatformIO:

1. **Fixed Function Naming & Typo Issues**:
   - `fingerprint_initilize()` corrected to `fingerprint_initialize()`.
   - `connectToWifi()` vs `connectToWiFi()` casing discrepancy unified across all files.
   - `matchFingerPrint()` unified to camelCase `matchFingerprint()`.
   - Corrected typo in debug output (`Fingerprinrt not found` -> `Fingerprint not found`).
   - Added backward-compatibility macro aliases to maintain compatibility.
2. **Resolved Duplicate Definitions**:
   - Removed duplicate definition of `handleSSID()` that previously collided between `menu.ino` and `wifi_setting.ino`.
3. **Repaired C++ Syntax & Missing Declarations**:
   - Replaced invalid `String admin_menu_items = {...}` syntax with a properly sized string array (`const char* admin_menu_items[]`).
   - Added explicit `#include <Preferences.h>`, `Preferences preferences;`, and `unsigned long time_wifi;` declarations.
4. **Standardized Serial Baud Rate**:
   - Corrected baud rate from non-standard `112500` to standard `115200`.

---

## 🚀 Getting Started

### Prerequisites

- [PlatformIO IDE](https://platformio.org/install/ide?install=vscode) (recommended via VS Code extension) or [PlatformIO Core CLI](https://docs.platformio.org/page/core.html).
- USB cable connecting your ESP32 board to your computer.

### 1. Clone the Repository

```bash
git clone https://github.com/LoNE-W0LvES/fingerDoor.git
cd fingerDoor
```

### 2. Build the Project

Using PlatformIO CLI:
```bash
pio run
```

Or click the **PlatformIO: Build** checkmark icon in the VS Code status bar.

### 3. Upload Firmware

Connect your ESP32 and run:
```bash
pio run --target upload
```

*(Note: On some ESP32 development boards, hold down the `BOOT` button during connection if the upload does not start automatically).*

### 4. Monitor Serial Output

Open the serial monitor at **115200 baud**:
```bash
pio device monitor --baud 115200
```

---

## 📖 How It Works

1. **Initialization (`setup`)**:
   - Initializes Serial communication at `115200 baud`.
   - Starts I2C communication and clears the SSD1306 OLED display.
   - Reads previously saved WiFi credentials from flash memory via `loadWifiCredentials()`.
   - Initializes the optical fingerprint sensor at `57600 baud`.
   - Calls `addFirstAdmin()`: Checks if an admin fingerprint exists (templates 1–9). If none are registered, prompts the user to enroll the first administrator fingerprint (Slot #1).
2. **Normal Operation (`loop`)**:
   - Continuously calls `matchFingerprint()` to scan for a valid finger placement.
   - Upon detecting a finger, converts the image and searches internal sensor flash templates.
   - If an enrolled template matches, outputs the user ID and confidence score.
3. **WiFi Configuration**:
   - If triggered, scans available 2.4 GHz SSIDs.
   - Use the rotary encoder to scroll through detected networks.
   - Press **Select** (`GPIO 26`) to choose an SSID.
   - If protected, enter the password character-by-character using the rotary encoder, toggling character groups (uppercase, lowercase, numbers, symbols) with the **Swap** button (`GPIO 27`), and confirming with **OK** (`GPIO 14`).
   - The credentials are saved to ESP32 NVS, and the device restarts automatically to connect.

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
