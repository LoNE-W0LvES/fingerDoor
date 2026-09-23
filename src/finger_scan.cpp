#include "finger_scan.h"
#include "admin_finger.h"

void fingerprint_initialize() {
  Serial2.begin(FINGERPRINT_BAUD, SERIAL_8N1, FINGERPRINT_RX_PIN, FINGERPRINT_TX_PIN);
  finger.begin(FINGERPRINT_BAUD);
  delay(100);

  while (!finger.verifyPassword()) {
    Serial.println(F("Fingerprint sensor not detected. Retrying..."));
    showStatusScreen(F("Sensor Error"), F("Sensor Not Found!"), F("Check RX(16)/TX(17)"), F("Retrying in 3s..."));
    delay(3000);
  }

  Serial.println(F("Found fingerprint sensor!"));
  showStatusScreen(F("Sensor Ready"), F("Sensor detected!"), F("Reading parameters..."), F(""));
  delay(800);

  finger.getParameters();
  Serial.print(F("Status: 0x"));
  Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x"));
  Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: "));
  Serial.println(finger.capacity);
  Serial.print(F("Security level: "));
  Serial.println(finger.security_level);
  Serial.print(F("Device address: "));
  Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: "));
  Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: "));
  Serial.println(finger.baud_rate);
}

uint8_t matchFingerprint() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_NOFINGER:
      return 0;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println(F("Communication error"));
      return 0;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println(F("Imaging error"));
      return 0;
    default:
      Serial.println(F("Unknown error"));
      return 0;
  }

  showStatusScreen(F("Scanning..."), F("Analyzing print"), F("Verifying ID..."), F(""));

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println(F("Image too messy"));
      showStatusScreen(F("Scan Error"), F("Image messy"), F("Please try again"));
      delay(1000);
      return 0;
    case FINGERPRINT_PACKETRECIEVEERR:
    case FINGERPRINT_FEATUREFAIL:
    case FINGERPRINT_INVALIDIMAGE:
    default:
      Serial.println(F("Feature error"));
      showStatusScreen(F("Scan Error"), F("Could not read"), F("Please try again"));
      delay(1000);
      return 0;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.print(F("Found print match! ID #"));
    Serial.print(finger.fingerID);
    Serial.print(F(" Confidence: "));
    Serial.println(finger.confidence);

    bool isAdmin = isAdminID(finger.fingerID);
    showAccessGranted(finger.fingerID, isAdmin);
    unlockDoor(finger.fingerID, isAdmin);
    return 1;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println(F("Fingerprint not found"));
    showAccessDenied();
    return 0;
  } else {
    Serial.println(F("Search error"));
    showStatusScreen(F("Scan Error"), F("Search failed"), F("Please try again"));
    delay(1000);
    return 0;
  }
}
