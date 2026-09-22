#include "wifi_setting.h"

bool switchOutput(int sw) {
  if (digitalRead(sw) == LOW) {
    delay(50);
    while (true) {
      if (digitalRead(sw) == HIGH) {
        break;
      }
      delay(10);
    }
    return true;
  }
  return false;
}

void handleSSID() {
  int n = WiFi.scanNetworks();
  long oldValue = 999;
  rotaryEncoder.setBoundaries(0, n, true);

  while (true) {
    val = rotaryEncoder.getEncoderValue();
    int page = val / 4;
    if (n == 0) {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print(F("No networks found."));
      display.display();
    } else {
      if (oldValue != val) {
        oldValue = val;
        display.clearDisplay();
        if ((page * 4) <= n) {
          for (int i = 0; i < 4; ++i) {
            int j = (page * 4) + i;
            display.setCursor(0, (i * 17));
            if (j == n) {
              display.print(F("Scan Again"));
            } else if (j < n) {
              display.print(j + 1);
              display.print(F(": "));
              display.print(WiFi.SSID(j));
              display.print((WiFi.encryptionType(j) == WIFI_AUTH_OPEN) ? " " : "*");
            }
          }
          display.setCursor(120, ((val % 4) * 17));
          display.print(F("<"));
        }
        display.display();
      }

      if (switchOutput(switchPin)) {
        if (val == n) {
          n = WiFi.scanNetworks();
          rotaryEncoder.setBoundaries(0, n, true);
          rotaryEncoder.setEncoderValue(0);
          oldValue = 999;
        } else {
          oldValue = 999;
          wifi_ssid = WiFi.SSID(val);
          wifi_password = (WiFi.encryptionType(val) == WIFI_AUTH_OPEN) ? "" : handlePassword(wifi_ssid);
          saveWifiCredentials(wifi_ssid, wifi_password);
          connectToWiFi(wifi_ssid, wifi_password);
          break;
        }
      }
    }
    delay(10);
  }
  isEnteringSSID = false;
}

String handlePassword(String ssid) {
  int okSelectCount = 0;
  long oldValue = 999;

  while (true) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(F("SSID: "));
    display.print(ssid.substring(0, 15));

    if (switchOutput(switchPinOk)) {
      okSelectCount += 1;
    }

    if (okSelectCount == 0) {
      display.setCursor(0, 17);
      if (switchOutput(switchPinSwap)) {
        rotaryEncoder.setEncoderValue(0);
        mode += 1;
        oldValue = 99;
      }
      val = rotaryEncoder.getEncoderValue();
      if (mode == 1) {
        rotaryEncoder.setBoundaries(0, 25, true);
        display.print(F("Password(S alpha): "));
      } else if (mode == 2) {
        rotaryEncoder.setBoundaries(0, 9, true);
        display.print(F("Password(Number): "));
      } else if (mode == 3) {
        rotaryEncoder.setBoundaries(0, 31, true);
        display.print(F("Password(Symbol): "));
      } else {
        mode = 0;
        rotaryEncoder.setBoundaries(0, 25, true);
        display.print(F("Password(B alpha): "));
      }

      if (oldValue != val) {
        oldValue = val;
        char gg = key[mode][val];
        textInput = String(gg);
      }

      if (digitalRead(switchPin) == LOW) {
        delay(50);
        rotaryEncoder.setEncoderValue(0);
        while (true) {
          val = rotaryEncoder.getEncoderValue();
          if (digitalRead(switchPin) == HIGH) {
            if (val == 0) {
              passwordInput = passwordInput + textInput;
              rotaryEncoder.setEncoderValue(0);
            } else {
              String pass = passwordInput.substring(0, (passwordInput.length() - 1));
              passwordInput = pass;
            }
            break;
          }
          delay(10);
        }
      }
      display.setCursor(0, 34);
      display.print(passwordInput);
      display.print(textInput);
    } else if (okSelectCount == 1) {
      if (switchOutput(switchPinSwap)) {
        okSelectCount -= 1;
      }
      display.setCursor(0, 17);
      display.print(F("Password: "));
      display.setCursor(0, 34);
      display.print(passwordInput);
    } else if (okSelectCount == 2) {
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
  ESP.restart();
}

void loadWifiCredentials() {
  preferences.begin("wifi", true);
  wifi_ssid = preferences.getString("ssid", "");
  wifi_password = preferences.getString("password", "");
  preferences.end();
  connectToWiFi(wifi_ssid, wifi_password);
}

void connectToWiFi(String ssidC, String passwordC) {
  if (ssidC != "") {
    if ((millis() - time_wifi) > 3000) {
      WiFi.disconnect();
      Serial.println(F("Connecting to WiFi..."));
      WiFi.begin(ssidC.c_str(), passwordC.c_str());
      time_wifi = millis();
    }
  }
}

void delete_cred() {
  preferences.begin("wifi", false);
  preferences.putString("ssid", "");
  preferences.putString("password", "");
  preferences.end();
  Serial.println(F("Credentials deleted. Restarting..."));
  ESP.restart();
}
