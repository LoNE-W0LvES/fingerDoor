#include "admin_finger.h"
#include "enroll_finger.h"

void addAdmin() {
  for (size_t i = 1; i < 10; i++) {
    uint8_t c = finger.loadModel(i);
    if (c != FINGERPRINT_OK) {
      while (!getFingerprintEnroll(i)) {
        delay(100);
      }
      break;
    }
  }
}

void addFirstAdmin() {
  noAdmin = 1;
  for (size_t i = 1; i < 10; i++) {
    uint8_t c = finger.loadModel(i);
    if (c == FINGERPRINT_OK) {
      noAdmin = 0;
      adminID = i;
      break;
    }
  }
  if (noAdmin == 1) {
    adminID = 1;
    Serial.println(F("No admin fingerprint found. Enrolling first admin (ID #1)..."));
    while (!getFingerprintEnroll(adminID)) {
      delay(100);
    }
    noAdmin = 0;
  }
}
