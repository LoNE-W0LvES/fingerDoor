#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <Preferences.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Fingerprint.h>
#include <ESP32RotaryEncoder.h>

// ==========================================
// Serial & UART Configuration
// ==========================================
#define SERIAL_BAUD_RATE    115200
#define FINGERPRINT_BAUD    57600
constexpr int FINGERPRINT_RX_PIN = 16; // ESP32 RX2 <- Sensor TX
constexpr int FINGERPRINT_TX_PIN = 17; // ESP32 TX2 -> Sensor RX

// ==========================================
// Door Relay / Lock Actuator Configuration
// ==========================================
constexpr int RELAY_PIN = 4;
constexpr bool RELAY_ACTIVE_LEVEL = HIGH; // HIGH triggers relay to unlock
constexpr unsigned long UNLOCK_DURATION_MS = 4000; // Unlock for 4 seconds

// ==========================================
// Rotary Encoder Pin Definitions
// ==========================================
constexpr uint8_t DI_ENCODER_A   = 33;
constexpr uint8_t DI_ENCODER_B   = 25;
constexpr int8_t  DI_ENCODER_SW  = -1;
constexpr int8_t  DO_ENCODER_VCC = -1;

// ==========================================
// Push Button Pin Definitions (Active LOW)
// ==========================================
constexpr int switchPin     = 26; // Select / Character Add
constexpr int switchPinSwap = 27; // Mode Swap
constexpr int switchPinOk   = 14; // Confirm / Menu / OK

// ==========================================
// OLED Display Configuration (I2C)
// ==========================================
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C

// ==========================================
// Slot ID Ranges
// ==========================================
constexpr int ADMIN_SLOT_START = 1;
constexpr int ADMIN_SLOT_END   = 9;
constexpr int USER_SLOT_START  = 10;
constexpr int USER_SLOT_END    = 127;

// ==========================================
// Global Shared Objects
// ==========================================
extern Adafruit_SSD1306 display;
extern RotaryEncoder rotaryEncoder;
extern Adafruit_Fingerprint finger;
extern Preferences preferences;

// ==========================================
// Global Shared State
// ==========================================
extern String textInput;
extern String passwordInput;
extern String wifi_ssid;
extern String wifi_password;
extern int mode;
extern bool isEnteringSSID;
extern char key[4][32];
extern long val;
extern int noAdmin;
extern int adminID;
extern unsigned long time_wifi;

// ==========================================
// Display & Lock Helper Functions
// ==========================================
void showStatusScreen(const String& title, const String& line1 = "", const String& line2 = "", const String& line3 = "");
void showIdleScreen();
void showAccessGranted(int id, bool isAdmin);
void showAccessDenied();
void unlockDoor(int id, bool isAdmin);
