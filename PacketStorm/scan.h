/*
 * Scanning Networks Header File
 * Author - WireBits
 */

#ifndef SCAN_H
#define SCAN_H

#include <ESP8266WiFi.h>

struct WiFiNetwork {
    String ssid;
    uint8_t bssid[6];
    int channel;
};

extern WiFiNetwork scannedNetworks[50];
extern int networkCount;

void startWiFiScan();
String cleanWiFiName(String ssid);

#endif
