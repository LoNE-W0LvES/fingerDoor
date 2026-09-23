#include "wifi_setting.h"

bool switchOutput(int sw) {
  if (digitalRead(sw) == LOW) {
    delay(50); // Debounce
    unsigned long start = millis();
    while (digitalRead(sw) == LOW) {
      if (millis() - start > 1500) {
        break; // Safety timeout prevents infinite freeze
      }
      delay(10);
    }
    return true;
  }
  return false;
}

void handleSSID() {
  showStatusScreen(F("WiFi Setup"), F("Scanning 2.4GHz..."), F("Please wait"), F(""));
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  int n = WiFi.scanNetworks();
  long oldValue = 999;
  rotaryEncoder.setBoundaries(0, n, true);
  rotaryEncoder.setEncoderValue(0);

  while (true) {
    val = rotaryEncoder.getEncoderValue();
    int page = val / 4;

    if (n == 0) {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println(F("-- WiFi Scan --"));
      display.println(F("No networks found."));
      display.println(F("Press OK to rescan"));
      display.display();

      if (switchOutput(switchPinOk) || switchOutput(switchPin)) {
        showStatusScreen(F("WiFi Setup"), F("Scanning again..."), F(""));
        n = WiFi.scanNetworks();
        rotaryEncoder.setBoundaries(0, n, true);
        rotaryEncoder.setEncoderValue(0);
        oldValue = 999;
      }
    } else {
      if (oldValue != val) {
        oldValue = val;
        display.clearDisplay();
        if ((page * 4) <= n) {
          for (int i = 0; i < 4; ++i) {
            int j = (page * 4) + i;
            display.setCursor(0, (i * 16));
            if (j == n) {
              display.print(F("Scan Again"));
            } else if (j < n) {
              display.print(j + 1);
              display.print(F(":"));
              display.print(WiFi.SSID(j).substring(0, 14));
              display.print((WiFi.encryptionType(j) == WIFI_AUTH_OPEN) ? " " : "*");
            }
          }
          display.setCursor(118, ((val % 4) * 16));
          display.print(F("<"));
        }
        display.display();
      }

      if (switchOutput(switchPin) || switchOutput(switchPinOk)) {
        if (val == n) {
          showStatusScreen(F("WiFi Setup"), F("Rescanning..."), F(""));
          n = WiFi.scanNetworks();
          rotaryEncoder.setBoundaries(0, n, true);
          rotaryEncoder.setEncoderValue(0);
          oldValue = 999;
        } else {
          wifi_ssid = WiFi.SSID(val);
          wifi_password = (WiFi.encryptionType(val) == WIFI_AUTH_OPEN) ? "" : handlePassword(wifi_ssid);
          saveWifiCredentials(wifi_ssid, wifi_password);
          connectToWiFi(wifi_ssid, wifi_password);
          break;
        }
      }
    }

    if (switchOutput(switchPinSwap)) {
      // Allow exiting WiFi setup
      showStatusScreen(F("WiFi Setup"), F("Cancelled"), F(""));
      delay(1000);
      break;
    }

    delay(10);
  }
  isEnteringSSID = false;
}

String handlePassword(String ssid) {
  int okSelectCount = 0;
  long oldValue = 999;
  passwordInput = "";
  textInput = "A";
  mode = 0;
  rotaryEncoder.setBoundaries(0, 25, true);
  rotaryEncoder.setEncoderValue(0);

  while (true) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(F("SSID: "));
    display.println(ssid.substring(0, 14));

    if (switchOutput(switchPinOk)) {
      okSelectCount += 1;
    }

    if (okSelectCount == 0) {
      display.setCursor(0, 14);
      if (switchOutput(switchPinSwap)) {
        rotaryEncoder.setEncoderValue(0);
        mode = (mode + 1) % 4;
        oldValue = 999;
      }

      val = rotaryEncoder.getEncoderValue();
      if (mode == 1) {
        rotaryEncoder.setBoundaries(0, 25, true);
        display.print(F("Mode: Lowercase"));
      } else if (mode == 2) {
        rotaryEncoder.setBoundaries(0, 9, true);
        display.print(F("Mode: Numbers"));
      } else if (mode == 3) {
        rotaryEncoder.setBoundaries(0, 31, true);
        display.print(F("Mode: Symbols"));
      } else {
        mode = 0;
        rotaryEncoder.setBoundaries(0, 25, true);
        display.print(F("Mode: Uppercase"));
      }

      if (oldValue != val) {
        oldValue = val;
        char gg = key[mode][val];
        textInput = String(gg);
      }

      // Button to append char or backspace
      if (digitalRead(switchPin) == LOW) {
        delay(50);
        rotaryEncoder.setEncoderValue(0);
        unsigned long pressStart = millis();
        while (digitalRead(switchPin) == LOW) {
          if (millis() - pressStart > 1000) {
            // Long press = Backspace
            if (passwordInput.length() > 0) {
              passwordInput.remove(passwordInput.length() - 1);
            }
            showStatusScreen(F("Backspace"), passwordInput, F(""));
            delay(300);
            break;
          }
          delay(10);
        }
        if (millis() - pressStart <= 1000) {
          // Short press = Append selected character
          passwordInput += textInput;
        }
      }

      display.setCursor(0, 30);
      display.print(F("Char: [ "));
      display.print(textInput);
      display.print(F(" ]"));

      display.setCursor(0, 46);
      display.print(F("Pass: "));
      int startIdx = (passwordInput.length() > 14) ? (passwordInput.length() - 14) : 0;
      display.print(passwordInput.substring(startIdx));
      display.print(F("_"));
    } else if (okSelectCount == 1) {
      if (switchOutput(switchPinSwap)) {
        okSelectCount -= 1; // Back to editing
      }
      display.setCursor(0, 16);
      display.println(F("Confirm Password:"));
      display.setCursor(0, 32);
      display.println(passwordInput);
      display.setCursor(0, 48);
      display.println(F("[OK]: Save [Swap]: Edit"));
    } else if (okSelectCount >= 2) {
      break;
    }
    display.display();
    delay(10);
  }
  return passwordInput;
}

void saveWifiCredentials(String ssid, String password) {
  preferences.begin("wifi", false);
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.end();
  Serial.println(F("WiFi credentials saved. Restarting..."));
  showStatusScreen(F("Saved!"), F("Credentials saved"), F("Restarting ESP32..."));
  delay(1500);
  ESP.restart();
}

void loadWifiCredentials() {
  preferences.begin("wifi", true);
  wifi_ssid = preferences.getString("ssid", "");
  wifi_password = preferences.getString("password", "");
  preferences.end();

  if (wifi_ssid.length() > 0) {
    Serial.println("Loaded WiFi SSID: " + wifi_ssid);
    connectToWiFi(wifi_ssid, wifi_password);
  } else {
    Serial.println(F("No saved WiFi credentials found."));
  }
}

void connectToWiFi(String ssidC, String passwordC) {
  if (ssidC.length() == 0) return;
  if (WiFi.status() == WL_CONNECTED) return;

  if (time_wifi == 0 || (millis() - time_wifi) > 10000) {
    Serial.print(F("Connecting to WiFi: "));
    Serial.println(ssidC);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssidC.c_str(), passwordC.c_str());
    time_wifi = millis();
  }
}

void delete_cred() {
  preferences.begin("wifi", false);
  preferences.clear();
  preferences.end();
  Serial.println(F("Credentials deleted. Restarting..."));
  showStatusScreen(F("WiFi Reset"), F("Credentials cleared"), F("Restarting..."));
  delay(1500);
  ESP.restart();
}
