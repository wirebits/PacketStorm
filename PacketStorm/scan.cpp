/*
 * Scanning Networks CPP File
 * Author - WireBits
 */

#include "scan.h"
#include <Wire.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;

WiFiNetwork scannedNetworks[50];
int networkCount = 0;

void startWiFiScan() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(38, 10);
    display.println("Scanning");
    display.display();
    WiFi.scanDelete();
    int totalSteps = 20;
    for (int i = 0; i <= totalSteps; i++) {
        display.fillRect(14, 30, (i * (100 / totalSteps)), 8, SSD1306_WHITE);
        display.fillRect(50, 45, 30, 10, SSD1306_BLACK);
        display.setCursor(50, 45);
        display.print(i * 5);
        display.print("%");
        display.display();
        delay(50);
    }
    networkCount = 0;
    int foundNetworks = WiFi.scanNetworks();
    for (int i = 0; i < foundNetworks && networkCount < 50; i++) {
        String cleanSSID = cleanWiFiName(WiFi.SSID(i));
        if (cleanSSID.length() > 0) {
            scannedNetworks[networkCount].ssid = cleanSSID;
            memcpy(scannedNetworks[networkCount].bssid, WiFi.BSSID(i), 6);
            scannedNetworks[networkCount].channel = WiFi.channel(i);
            networkCount++;
        }
    }
    display.clearDisplay();
    display.setCursor(22, 28);
    display.println("Scan Completed!");
    display.display();
}

String cleanWiFiName(String ssid) {
    String cleanSSID = "";
    for (int i = 0; i < ssid.length(); i++) {
        char c = ssid[i];
        if (isPrintable(c)) {
            cleanSSID += c;
        }
    }
    cleanSSID.trim();
    return cleanSSID;
}
