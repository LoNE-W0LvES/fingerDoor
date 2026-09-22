#include "enroll_finger.h"

uint8_t getFingerprintEnroll(int id) {
  int p = -1;
  Serial.print(F("Waiting for valid finger to enroll as #"));
  Serial.println(id);

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println(F("Finger found"));
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(F("."));
        delay(50);
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println(F("Communication error"));
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println(F("Imaging error"));
        break;
      default:
        Serial.println(F("Unknown error"));
        break;
    }
  }

  // OK success!
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println(F("Sample taken #1"));
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println(F("Image too messy"));
      return false;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println(F("Communication error"));
      return false;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println(F("Could not find fingerprint features"));
      return false;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println(F("Could not find fingerprint features"));
      return false;
    default:
      Serial.println(F("Unknown error"));
      return false;
  }

  Serial.println(F("Remove finger"));
  delay(1000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
    delay(50);
  }
  Serial.print(F("ID "));
  Serial.println(id);
  p = -1;
  Serial.println(F("Place same finger again"));
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println(F("Sample taken #2"));
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(F("."));
        delay(50);
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println(F("Communication error"));
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println(F("Imaging error"));
        break;
      default:
        Serial.println(F("Unknown error"));
        break;
    }
  }

  // OK success!
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println(F("Image converted"));
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println(F("Image too messy"));
      return false;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println(F("Communication error"));
      return false;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println(F("Could not find fingerprint features"));
      return false;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println(F("Could not find fingerprint features"));
      return false;
    default:
      Serial.println(F("Unknown error"));
      return false;
  }

  // OK converted!
  Serial.print(F("Creating model for #"));
  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println(F("Prints matched!"));
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println(F("Communication error"));
    return false;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println(F("Fingerprints did not match"));
    return false;
  } else {
    Serial.println(F("Unknown error"));
    return false;
  }

  Serial.print(F("ID "));
  Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println(F("Stored!"));
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println(F("Communication error"));
    return false;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println(F("Could not store in that location"));
    return false;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println(F("Error writing to flash"));
    return false;
  } else {
    Serial.println(F("Unknown error"));
    return false;
  }

  return true;
}
