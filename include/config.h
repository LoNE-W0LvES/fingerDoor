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
// Serial Configuration
// ==========================================
#define SERIAL_BAUD_RATE    115200
#define FINGERPRINT_BAUD    57600

// ==========================================
// Rotary Encoder Pin Definitions
// ==========================================
constexpr uint8_t DI_ENCODER_A   = 33;
constexpr uint8_t DI_ENCODER_B   = 25;
constexpr int8_t  DI_ENCODER_SW  = -1;
constexpr int8_t  DO_ENCODER_VCC = -1;

// ==========================================
// Push Button Pin Definitions
// ==========================================
constexpr int switchPin     = 26; // Select / Character Add
constexpr int switchPinSwap = 27; // Mode Swap
constexpr int switchPinOk   = 14; // Confirm / OK

// ==========================================
// OLED Display Configuration (I2C)
// ==========================================
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C

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
