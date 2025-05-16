#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

#include <WiFi.h>
#include <PubSubClient.h>

// Declarations only (no assignments here)
extern const char *ssid;
extern const char *password;
extern const char *mqtt_server;
extern bool online;
extern const unsigned Wifi_Delay;
extern int WifiCounter;

extern WiFiClient espClient;
extern PubSubClient client;

void setup_wifi();
void callback(char *topic, byte *message, unsigned int length);
void reconnect();

#endif
