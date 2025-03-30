/*
 * PacketStorm
 * An ESP8266-based WiFi deauthentication tool with OLED display and button controls.
 * Author - WireBits
 */

#include <Wire.h>
#include "scan.h"
#include "deauth.h"
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BTN_UP 14
#define BTN_DOWN 12
#define BTN_SELECT 13
#define OLED_RESET -1
#define SCREEN_HEIGHT 64
#define SCREEN_WIDTH 128

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int scrollOffset = 0;
int currentOption = 0;
int selectMenuOption = 0;
int deauthMenuOption = 0;
bool inDeauthMenu = false;
bool inSelectMenu = false;
String selectedNetwork = "";
const char* options[] = {"SCAN", "DEAUTH"};

void showMainMenu() {
    display.clearDisplay();
    int totalOptions = 2;
    int menuHeight = totalOptions * 10;
    int startY = (SCREEN_HEIGHT - menuHeight) / 2;
    for (int i = 0; i < totalOptions; i++) {
        display.setCursor(5, startY + (i * 10));
        display.print(i == currentOption ? "* " : "  ");
        display.println(options[i]);
    }
}

void showSelectMenu() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    if (networkCount == 0) {
        display.setCursor(22, 28);
        display.println("No Networks Found!");
        display.display();
        return;
    }
    static unsigned long lastScrollTime = 0;
    static int scrollPos = 0;
    unsigned long currentTime = millis();
    int maxVisibleChars = 17;
    int endIndex = min(scrollOffset + 4, networkCount);
    for (int i = scrollOffset; i < endIndex; i++) {
        String wifiName = scannedNetworks[i].ssid;
        display.setCursor(5, 10 + (i - scrollOffset) * 10);
        display.print((i == selectMenuOption) ? "* " : "  ");
        if (i == selectMenuOption && wifiName.length() > maxVisibleChars) { 
            if (currentTime - lastScrollTime > 400) {
                scrollPos = (scrollPos + 1) % (wifiName.length() - maxVisibleChars + 1);
                lastScrollTime = currentTime;
            }
            display.print(wifiName.substring(scrollPos, scrollPos + maxVisibleChars));
        } else {
            display.print(wifiName.substring(0, maxVisibleChars));
        }
    }
    display.setCursor(5, 10 + (endIndex - scrollOffset) * 10);
    display.print(selectMenuOption == networkCount ? "* " : "  ");
    display.println("BACK");
    display.display();
}

void showDeauthMenu() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    if (selectedNetwork == "") {
        display.setCursor(22, 28);
        display.println("No Network Selected!");
        display.display();
        return;
    }
    static unsigned long lastScrollTime = 0;
    static int scrollPos = 0;
    unsigned long currentTime = millis();
    int maxVisibleChars = 16;
    String displaySSID = selectedNetwork;
    if (selectedNetwork.length() > maxVisibleChars) {
        if (currentTime - lastScrollTime > 400) {
            scrollPos = (scrollPos + 1) % (selectedNetwork.length() + 1);
            lastScrollTime = currentTime;
        }
        int start = scrollPos % (selectedNetwork.length() + 1);
        displaySSID = selectedNetwork.substring(start, start + maxVisibleChars);
    }
    int textWidth = displaySSID.length() * 6;
    int xPos = (SCREEN_WIDTH - textWidth) / 2;
    if (xPos < 0) xPos = 0;
    display.setCursor(xPos, 10);
    display.println(displaySSID);
    int startY = 30;
    display.setCursor(5, startY);
    display.print(deauthMenuOption == 0 ? "* " : "  ");
    display.print("START");
    if (deauthRunning) display.print(" <");
    display.println();
    display.setCursor(5, startY + 10);
    display.print(deauthMenuOption == 1 ? "* " : "  ");
    display.print("STOP");
    if (!deauthRunning) display.print(" <");
    display.println();
    display.setCursor(5, startY + 20);
    display.print(deauthMenuOption == 2 ? "* " : "  ");
    display.println("BACK");
    display.display();
}

void setup() {
    pinMode(BTN_UP, INPUT_PULLUP);
    pinMode(BTN_DOWN, INPUT_PULLUP);
    pinMode(BTN_SELECT, INPUT_PULLUP);
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        while(true);
    }
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(30, 28);
    display.println("PacketStorm");
    display.display();
    delay(2000);
}

void loop() {
    display.clearDisplay();
    if (inSelectMenu) {
        showSelectMenu();
    } else if (inDeauthMenu) {
        showDeauthMenu();
    } else {
        showMainMenu();
    }
    display.display();
    bool upPressed = digitalRead(BTN_UP) == LOW;
    bool downPressed = digitalRead(BTN_DOWN) == LOW;
    bool selectPressed = digitalRead(BTN_SELECT) == LOW;
    if (downPressed || upPressed) {
        int step = downPressed ? 1 : -1;
        if (inSelectMenu) {
            selectMenuOption = constrain(selectMenuOption + step, 0, networkCount);
            scrollOffset = constrain(selectMenuOption - 3, 0, max(0, networkCount - 4));
        } else if (inDeauthMenu) {
            deauthMenuOption = (deauthMenuOption + step + 3) % 3;
        } else {
            currentOption = (currentOption + step + 2) % 2;
        }
        delay(200);
    }
    if (selectPressed) {
        if (inSelectMenu) {
            if (selectMenuOption == networkCount) {
                inSelectMenu = false;
            } else {
                selectedNetwork = scannedNetworks[selectMenuOption].ssid;
                memcpy(targetBSSID, scannedNetworks[selectMenuOption].bssid, 6);
                targetChannel = scannedNetworks[selectMenuOption].channel;
                inSelectMenu = false;
                inDeauthMenu = true;
                deauthMenuOption = 0;
            }
        } else if (inDeauthMenu) {
            switch (deauthMenuOption) {
                case 0: startDeauth(selectMenuOption); break;
                case 1: if (deauthRunning) stopDeauth(); break;
                case 2: inDeauthMenu = false; break;
            }
        } else {
            switch (currentOption) {
                case 0: startWiFiScan(); break;
                case 1: inSelectMenu = true; selectMenuOption = scrollOffset = 0; break;
            }
        }
        delay(500);
    }
    if (deauthRunning) sendDeauth();
}
