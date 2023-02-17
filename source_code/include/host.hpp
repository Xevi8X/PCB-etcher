#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "program_state.h"

#define STEP 2.0f


void startServer();
void handleClientsServer();