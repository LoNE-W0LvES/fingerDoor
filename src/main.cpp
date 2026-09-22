#include "config.h"
#include "admin_finger.h"
#include "enroll_finger.h"
#include "finger_scan.h"
#include "wifi_setting.h"
#include "menu.h"

// Hardware instances
RotaryEncoder rotaryEncoder(DI_ENCODER_A, DI_ENCODER_B, DI_ENCODER_SW, DO_ENCODER_VCC);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);
Preferences preferences;

// Global State
String textInput = "";
String passwordInput = "";
String wifi_ssid = "";
String wifi_password = "";

int mode = 0;
bool isEnteringSSID = true;
char key[4][32] = {
    {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'},
    {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'},
    {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'},
    {'!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', ':', ';', '<', '=', '>', '?', '@', '[', '\\', ']', '^', '_', '`', '{', '|', '}', '~'}
};
long val = 0;
int noAdmin = 0;
int adminID = 1;
unsigned long time_wifi = 0;

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  Wire.begin();

  pinMode(switchPin, INPUT_PULLUP);
  pinMode(switchPinOk, INPUT_PULLUP);
  pinMode(switchPinSwap, INPUT_PULLUP);

  rotaryEncoder.setEncoderType(EncoderType::HAS_PULLUP);
  rotaryEncoder.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.display();

  loadWifiCredentials();
  fingerprint_initialize();
  addFirstAdmin();
}

void loop() {
  matchFingerprint();
}
