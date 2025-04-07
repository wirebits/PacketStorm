/*
 * Menu Header File
 * Author - WireBits
 */

#ifndef MENU_H
#define MENU_H

#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;

void showMainMenu(int currentOption);
void showSelectMenu(int selectMenuOption, int scrollOffset, int networkCount);
void showDeauthMenu(const String& selectedNetwork, int deauthMenuOption, bool deauthRunning);

#endif
