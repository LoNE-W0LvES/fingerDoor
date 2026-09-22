#include "finger_scan.h"

void fingerprint_initialize() {
  finger.begin(FINGERPRINT_BAUD);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println(F("Found fingerprint sensor!"));
  } else {
    Serial.println(F("Did not find fingerprint sensor :("));
    while (1) {
      delay(1);
    }
  }

  Serial.println(F("Reading sensor parameters"));
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
      Serial.println(F("No finger detected"));
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

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println(F("Image too messy"));
      return 0;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println(F("Communication error"));
      return 0;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println(F("Could not find fingerprint features"));
      return 0;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println(F("Could not find fingerprint features"));
      return 0;
    default:
      Serial.println(F("Unknown error"));
      return 0;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println(F("Found a print match!"));
    Serial.print(F("Found ID #"));
    Serial.print(finger.fingerID);
    Serial.print(F(" with confidence of "));
    Serial.println(finger.confidence);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println(F("Communication error"));
    return 0;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println(F("Fingerprint not found"));
    return 0;
  } else {
    Serial.println(F("Unknown error"));
    return 0;
  }
  return 1;
}
