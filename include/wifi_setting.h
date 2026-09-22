#pragma once

#include "config.h"

bool switchOutput(int sw);
void handleSSID();
String handlePassword(String ssid);
void saveWifiCredentials(String ssid, String password);
void loadWifiCredentials();
void connectToWiFi(String ssidC, String passwordC);
void delete_cred();

// Backward compatibility alias for lowercase 'i'
#define connectToWifi connectToWiFi
