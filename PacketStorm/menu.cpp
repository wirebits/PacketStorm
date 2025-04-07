/*
 * Menu CPP File
 * Author - WireBits
 */

#include "menu.h"
#include "scan.h"
#include <ESP8266WiFi.h>

void showMainMenu(int currentOption) {
    display.clearDisplay();
    const char* options[] = {"SCAN", "DEAUTH"};
    int totalOptions = 2;
    int menuHeight = totalOptions * 10;
    int startY = (display.height() - menuHeight) / 2;
    for (int i = 0; i < totalOptions; i++) {
        display.setCursor(5, startY + (i * 10));
        display.print(i == currentOption ? "* " : "  ");
        display.println(options[i]);
    }
    display.display();
}

void showSelectMenu(int selectMenuOption, int scrollOffset, int networkCount) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    if (networkCount == 0) {
        display.setCursor(12, 28);
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

void showDeauthMenu(const String& selectedNetwork, int deauthMenuOption, bool deauthRunning) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    if (selectedNetwork == "") {
        display.setCursor(12, 28);
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
    int xPos = (display.width() - textWidth) / 2;
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
