#include "enroll_finger.h"

uint8_t getFingerprintEnroll(int id) {
  int p = -1;
  Serial.print(F("Waiting for valid finger to enroll as #"));
  Serial.println(id);

  showStatusScreen("Enroll #" + String(id), F("Step 1 of 2:"), F("Place Finger on"), F("sensor..."));

  // Step 1: Wait for finger and take image
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println(F("Finger found"));
        break;
      case FINGERPRINT_NOFINGER:
        delay(50);
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println(F("Communication error"));
        showStatusScreen(F("Enroll Error"), F("Comm Error"), F("Check wiring"));
        delay(1500);
        return false;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println(F("Imaging error"));
        showStatusScreen(F("Enroll Error"), F("Imaging Error"), F("Clean sensor"));
        delay(1500);
        return false;
      default:
        Serial.println(F("Unknown error"));
        delay(50);
        break;
    }
  }

  // Convert image to template 1
  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.println(F("Failed converting sample 1"));
    showStatusScreen(F("Enroll Error"), F("Image messy / bad"), F("Try again"));
    delay(1500);
    return false;
  }

  Serial.println(F("Sample taken #1. Remove finger"));
  showStatusScreen("Enroll #" + String(id), F("Sample #1 OK!"), F("Please REMOVE"), F("finger..."));
  delay(1000);

  // Wait for finger release
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
    delay(50);
  }

  // Step 2: Place same finger again
  Serial.println(F("Place same finger again"));
  showStatusScreen("Enroll #" + String(id), F("Step 2 of 2:"), F("Place SAME finger"), F("again..."));
  p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println(F("Sample taken #2"));
        break;
      case FINGERPRINT_NOFINGER:
        delay(50);
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println(F("Communication error"));
        showStatusScreen(F("Enroll Error"), F("Comm Error"), F(""));
        delay(1500);
        return false;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println(F("Imaging error"));
        showStatusScreen(F("Enroll Error"), F("Imaging Error"), F(""));
        delay(1500);
        return false;
      default:
        Serial.println(F("Unknown error"));
        delay(50);
        break;
    }
  }

  // Convert image to template 2
  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    Serial.println(F("Failed converting sample 2"));
    showStatusScreen(F("Enroll Error"), F("Image #2 messy"), F("Try again"));
    delay(1500);
    return false;
  }

  // Create model from 1 and 2
  showStatusScreen("Enroll #" + String(id), F("Comparing prints..."), F("Matching features"), F(""));
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println(F("Prints matched!"));
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println(F("Fingerprints did not match"));
    showStatusScreen(F("Enroll Failed"), F("Prints did not match!"), F("Try again"));
    delay(2000);
    return false;
  } else {
    Serial.println(F("Model creation failed"));
    showStatusScreen(F("Enroll Failed"), F("Feature error"), F(""));
    delay(2000);
    return false;
  }

  // Store model at ID
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println(F("Stored!"));
    showStatusScreen("Enroll #" + String(id), F("Enrolled OK!"), "Saved in Slot #" + String(id), F(""));
    delay(1500);
    return true;
  } else {
    Serial.println(F("Store error"));
    showStatusScreen(F("Enroll Failed"), F("Flash write error"), F(""));
    delay(2000);
    return false;
  }
}
