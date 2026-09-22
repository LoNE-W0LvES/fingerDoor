#include "menu.h"

const char* admin_menu_items[] = {
    "Add User",
    "Add Admin",
    "Change Password"
};

const size_t admin_menu_items_count = sizeof(admin_menu_items) / sizeof(admin_menu_items[0]);

void displayAdminMenu(int selectedIndex) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("-- Admin Menu --"));

  for (size_t i = 0; i < admin_menu_items_count; ++i) {
    display.setCursor(10, 16 + (i * 14));
    if ((int)i == selectedIndex) {
      display.print(F("> "));
    } else {
      display.print(F("  "));
    }
    display.println(admin_menu_items[i]);
  }
  display.display();
}
