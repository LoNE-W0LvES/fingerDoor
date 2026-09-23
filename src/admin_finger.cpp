#include "admin_finger.h"
#include "enroll_finger.h"

bool isAdminID(int id) {
  return (id >= ADMIN_SLOT_START && id <= ADMIN_SLOT_END);
}

void addAdmin() {
  int targetSlot = -1;
  for (int i = ADMIN_SLOT_START; i <= ADMIN_SLOT_END; i++) {
    uint8_t c = finger.loadModel(i);
    if (c != FINGERPRINT_OK) {
      targetSlot = i;
      break;
    }
  }

  if (targetSlot == -1) {
    showStatusScreen(F("Admin Full"), F("Slots 1-9 occupied"), F("Delete an admin first"));
    delay(2500);
    return;
  }

  showStatusScreen(F("Add Admin"), "Enrolling Slot #" + String(targetSlot), F("Follow prompts"));
  delay(1500);

  bool success = getFingerprintEnroll(targetSlot);
  if (success) {
    showStatusScreen(F("Success!"), "Admin #" + String(targetSlot) + " Enrolled", F("Ready for use"));
  } else {
    showStatusScreen(F("Enroll Failed"), F("Please try again"), F(""));
  }
  delay(2000);
}

void addUser() {
  int targetSlot = -1;
  for (int i = USER_SLOT_START; i <= USER_SLOT_END; i++) {
    uint8_t c = finger.loadModel(i);
    if (c != FINGERPRINT_OK) {
      targetSlot = i;
      break;
    }
  }

  if (targetSlot == -1) {
    showStatusScreen(F("Storage Full"), F("Slots 10-127 full"), F("Delete users first"));
    delay(2500);
    return;
  }

  showStatusScreen(F("Add User"), "Enrolling Slot #" + String(targetSlot), F("Follow prompts"));
  delay(1500);

  bool success = getFingerprintEnroll(targetSlot);
  if (success) {
    showStatusScreen(F("Success!"), "User #" + String(targetSlot) + " Enrolled", F("Access granted"));
  } else {
    showStatusScreen(F("Enroll Failed"), F("Please try again"), F(""));
  }
  delay(2000);
}

void deleteFingerprint(int id) {
  if (id < ADMIN_SLOT_START || id > USER_SLOT_END) {
    showStatusScreen(F("Invalid ID"), "Slot: " + String(id), F("Out of range"));
    delay(2000);
    return;
  }

  uint8_t p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK) {
    showStatusScreen(F("Deleted!"), "Slot #" + String(id) + " cleared", F(""));
  } else {
    showStatusScreen(F("Delete Error"), "Slot #" + String(id), F("Could not delete"));
  }
  delay(2000);
}

void addFirstAdmin() {
  noAdmin = 1;
  for (int i = ADMIN_SLOT_START; i <= ADMIN_SLOT_END; i++) {
    uint8_t c = finger.loadModel(i);
    if (c == FINGERPRINT_OK) {
      noAdmin = 0;
      adminID = i;
      break;
    }
  }

  if (noAdmin == 1) {
    adminID = ADMIN_SLOT_START;
    Serial.println(F("No admin fingerprint found. Enrolling first admin (ID #1)..."));

    showStatusScreen(F("FIRST SETUP"), F("No Admin Found"), F("Registering Admin #1"), F("Place Finger"));
    delay(2500);

    while (!getFingerprintEnroll(adminID)) {
      showStatusScreen(F("Retry Admin #1"), F("Place Finger firmly"), F("on sensor..."));
      delay(1500);
    }

    noAdmin = 0;
    showStatusScreen(F("Admin #1 Ready"), F("Initial setup complete!"), F("Door is secure"));
    delay(2500);
  }
}
