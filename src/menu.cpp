#include "menu.h"
#include "admin_finger.h"
#include "wifi_setting.h"

const char* admin_menu_items[] = {
    "1. Add User",
    "2. Add Admin",
    "3. Delete Finger",
    "4. WiFi Setup",
    "5. Clear WiFi",
    "6. Exit Menu"
};

const size_t admin_menu_items_count = sizeof(admin_menu_items) / sizeof(admin_menu_items[0]);

void displayAdminMenu(int selectedIndex) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("=== ADMIN MENU ==="));

  // Determine scroll window (display fits 3 menu items at once)
  int startIdx = 0;
  if (selectedIndex > 1) {
    startIdx = selectedIndex - 1;
    if (startIdx + 3 > (int)admin_menu_items_count) {
      startIdx = admin_menu_items_count - 3;
    }
  }

  for (int i = 0; i < 3 && (startIdx + i) < (int)admin_menu_items_count; ++i) {
    int itemIdx = startIdx + i;
    display.setCursor(0, 16 + (i * 14));
    if (itemIdx == selectedIndex) {
      display.print(F("> "));
    } else {
      display.print(F("  "));
    }
    display.println(admin_menu_items[itemIdx]);
  }

  display.setCursor(0, 56);
  display.print(F("[OK]:Select  [SW]:Exit"));
  display.display();
}

static void handleDeleteFingerMenu() {
  rotaryEncoder.setBoundaries(ADMIN_SLOT_START, USER_SLOT_END, true);
  rotaryEncoder.setEncoderValue(USER_SLOT_START);
  long oldSlot = -1;

  while (true) {
    long curSlot = rotaryEncoder.getEncoderValue();
    if (curSlot != oldSlot) {
      oldSlot = curSlot;
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println(F("-- Delete Slot --"));
      display.setCursor(0, 20);
      display.print(F("Target Slot: #"));
      display.println(curSlot);
      display.setCursor(0, 36);
      if (isAdminID(curSlot)) {
        display.println(F("[ADMIN SLOT]"));
      } else {
        display.println(F("[USER SLOT]"));
      }
      display.setCursor(0, 52);
      display.println(F("[OK]: Del  [Swap]: Back"));
      display.display();
    }

    if (switchOutput(switchPinOk) || switchOutput(switchPin)) {
      deleteFingerprint(curSlot);
      break;
    }

    if (switchOutput(switchPinSwap)) {
      break;
    }
    delay(10);
  }
}

void runAdminMenu() {
  showStatusScreen(F("Admin Mode"), F("Access Granted"), F("Entering menu..."), F(""));
  delay(1000);

  rotaryEncoder.setBoundaries(0, admin_menu_items_count - 1, true);
  rotaryEncoder.setEncoderValue(0);
  long oldMenuVal = -1;

  while (true) {
    long curMenuVal = rotaryEncoder.getEncoderValue();
    if (curMenuVal != oldMenuVal) {
      oldMenuVal = curMenuVal;
      displayAdminMenu(curMenuVal);
    }

    if (switchOutput(switchPinOk) || switchOutput(switchPin)) {
      switch (curMenuVal) {
        case 0: // Add User
          addUser();
          break;
        case 1: // Add Admin
          addAdmin();
          break;
        case 2: // Delete Finger
          handleDeleteFingerMenu();
          break;
        case 3: // WiFi Setup
          handleSSID();
          break;
        case 4: // Clear WiFi
          delete_cred();
          break;
        case 5: // Exit
          showStatusScreen(F("Admin Menu"), F("Exiting..."), F(""));
          delay(800);
          return;
      }
      // Reset boundaries when returning from submenus
      rotaryEncoder.setBoundaries(0, admin_menu_items_count - 1, true);
      rotaryEncoder.setEncoderValue(curMenuVal);
      oldMenuVal = -1;
    }

    if (switchOutput(switchPinSwap)) {
      // Exit menu
      break;
    }

    delay(10);
  }

  showStatusScreen(F("FingerDoor"), F("Lock Active"), F(""));
  delay(800);
}
