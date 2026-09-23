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

static unsigned long lastIdleUpdate = 0;

// ==========================================
// Display & Lock Helper Implementations
// ==========================================

void showStatusScreen(const String& title, const String& line1, const String& line2, const String& line3) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(title);
  display.drawLine(0, 11, 127, 11, SSD1306_WHITE);

  if (line1.length() > 0) {
    display.setCursor(0, 16);
    display.println(line1);
  }
  if (line2.length() > 0) {
    display.setCursor(0, 30);
    display.println(line2);
  }
  if (line3.length() > 0) {
    display.setCursor(0, 44);
    display.println(line3);
  }
  display.display();
}

void showIdleScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Top header bar
  display.setCursor(16, 2);
  display.println(F("[ FINGERDOOR ]"));
  display.drawLine(0, 13, 127, 13, SSD1306_WHITE);

  // Main instruction
  display.setCursor(18, 22);
  display.setTextSize(1);
  display.println(F("PLACE FINGER"));
  display.setCursor(26, 34);
  display.println(F("ON SENSOR"));

  display.drawLine(0, 48, 127, 48, SSD1306_WHITE);

  // Bottom status bar
  display.setCursor(0, 52);
  if (WiFi.status() == WL_CONNECTED) {
    display.print(F("WiFi: OK"));
  } else if (wifi_ssid.length() > 0) {
    display.print(F("WiFi: ..."));
  } else {
    display.print(F("WiFi: Off"));
  }

  display.setCursor(70, 52);
  display.print(F("[OK]:Menu"));

  display.display();
}

void showAccessGranted(int id, bool isAdmin) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(12, 2);
  display.println(F("ACCESS GRANTED!"));
  display.drawLine(0, 13, 127, 13, SSD1306_WHITE);

  display.setCursor(16, 24);
  if (isAdmin) {
    display.print(F("Admin Slot #"));
  } else {
    display.print(F("User Slot #"));
  }
  display.println(id);

  display.setCursor(18, 40);
  display.println(F("DOOR UNLOCKED"));

  display.display();
}

void showAccessDenied() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(14, 4);
  display.println(F("ACCESS DENIED!"));
  display.drawLine(0, 14, 127, 14, SSD1306_WHITE);

  display.setCursor(10, 26);
  display.println(F("Fingerprint Not"));
  display.setCursor(24, 40);
  display.println(F("Recognized"));

  display.display();
  delay(1800);
}

void unlockDoor(int id, bool isAdmin) {
  Serial.print(F("Actuating door lock for ID #"));
  Serial.println(id);

  // Trigger relay to unlock
  digitalWrite(RELAY_PIN, RELAY_ACTIVE_LEVEL);

  // Keep unlocked for duration
  delay(UNLOCK_DURATION_MS);

  // Lock door again
  digitalWrite(RELAY_PIN, !RELAY_ACTIVE_LEVEL);

  showStatusScreen(F("FingerDoor"), F("Door Locked"), F("System Secure"), F(""));
  delay(1000);
}

static void checkAdminMenuRequest() {
  showStatusScreen(F("Admin Auth"), F("Scan Admin Finger"), F("to access menu..."), F("[Swap]: Cancel"));

  unsigned long startTime = millis();
  while (millis() - startTime < 6000) {
    if (switchOutput(switchPinSwap)) {
      return;
    }

    uint8_t p = finger.getImage();
    if (p == FINGERPRINT_OK) {
      p = finger.image2Tz();
      if (p == FINGERPRINT_OK) {
        p = finger.fingerSearch();
        if (p == FINGERPRINT_OK) {
          if (isAdminID(finger.fingerID)) {
            runAdminMenu();
            return;
          } else {
            showStatusScreen(F("Access Denied"), F("User is not Admin!"), F(""));
            delay(1800);
            return;
          }
        } else {
          showStatusScreen(F("Denied"), F("Fingerprint unknown"), F(""));
          delay(1500);
          return;
        }
      }
    }
    delay(50);
  }
}

// ==========================================
// Main Setup & Loop
// ==========================================

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  Wire.begin();

  // Pin initialization
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, !RELAY_ACTIVE_LEVEL); // Ensure door starts LOCKED

  pinMode(switchPin, INPUT_PULLUP);
  pinMode(switchPinOk, INPUT_PULLUP);
  pinMode(switchPinSwap, INPUT_PULLUP);

  rotaryEncoder.setEncoderType(EncoderType::HAS_PULLUP);
  rotaryEncoder.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Welcome splash screen
  showStatusScreen(F("FINGERDOOR v2.0"), F("Biometric Access"), F("Initializing..."), F(""));
  delay(1500);

  // Load WiFi settings
  loadWifiCredentials();

  // If no WiFi is configured, prompt user to set it up or skip
  if (wifi_ssid.length() == 0) {
    showStatusScreen(F("WiFi Setup"), F("No WiFi configured"), F("[OK]: Setup WiFi"), F("[Swap]: Skip"));
    unsigned long waitStart = millis();
    while (millis() - waitStart < 4000) {
      if (switchOutput(switchPinOk) || switchOutput(switchPin)) {
        handleSSID();
        break;
      }
      if (switchOutput(switchPinSwap)) {
        break;
      }
      delay(20);
    }
  }

  // Initialize fingerprint sensor
  fingerprint_initialize();

  // First-run admin setup if required
  addFirstAdmin();

  showStatusScreen(F("System Ready"), F("All Systems Active"), F("Securing entrance"), F(""));
  delay(1200);
}

void loop() {
  // Periodically refresh idle screen every 1.5 seconds (or WiFi status change)
  if (millis() - lastIdleUpdate > 1500) {
    lastIdleUpdate = millis();
    showIdleScreen();
  }

  // Check if user requested the Admin Menu
  if (switchOutput(switchPinOk)) {
    checkAdminMenuRequest();
    lastIdleUpdate = 0; // Trigger immediate idle refresh
  }

  // Scan for fingerprints
  uint8_t matchResult = matchFingerprint();
  if (matchResult == 1) {
    lastIdleUpdate = 0; // Refresh after unlock
  }

  delay(20);
}
