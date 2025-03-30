/*
 * Deauthentication Header File
 * Author - WireBits
 */

#ifndef DEAUTH_H
#define DEAUTH_H

#include <ESP8266WiFi.h>

extern uint8_t targetBSSID[6];
extern int targetChannel;
extern bool deauthRunning;

void startDeauth(int index);
void sendDeauth();
void stopDeauth();

#endif
