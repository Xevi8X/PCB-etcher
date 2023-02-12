#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>

#include "index.h"
#include "WiFiCredentials.h"
#define STEP 2.0f
#define MEASURE_PERIOD_IN_MILLIS 1000


//Configuration
//==================================================
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
float hysteresis = 1.0f;
//==================================================


OneWire oneWire(0);
DallasTemperature sensors(&oneWire);

ESP8266WebServer server(80);

float targetTemp = 40.0f;
float actualTemp = 0.0f;
Heating heating = Heating::OFF;

unsigned long previousMillis = 0;


void connectToWiFi()
{
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.setHostname("Wytrawiarka");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  if (MDNS.begin("wytrawiarka")) Serial.println("MDNS started.");
  server.begin();
}

void handleRoot() {
 server.send(200, "text/html", MAIN_page); //Send web page
}

void setTargetHandler()
{
   targetTemp = server.arg("temp").toFloat();
   server.send(200);
}

void changeTarget(float diff)
{
  targetTemp+=diff;
  server.send(200);
}




void getData()
{
  const int cap = JSON_OBJECT_SIZE(20);
  char output[128]; 
  StaticJsonDocument<cap> doc;
  doc["actual"] = actualTemp;
  doc["target"] = targetTemp;
  doc["state"] = heatingToString(heating);
  serializeJson(doc,output,128);
  server.send(200,"application/json",output);
}

void setHandlers()
{
  server.on("/", handleRoot);
  server.on("/target/set", HTTPMethod::HTTP_POST , setTargetHandler);
  server.on("/target/increase", HTTPMethod::HTTP_POST ,[](){changeTarget(STEP);});
  server.on("/target/decrease", HTTPMethod::HTTP_POST , [](){changeTarget(-STEP);});
  server.on("/data", HTTPMethod::HTTP_GET , getData);
}

void execHeating()
{
  
}

void measure()
{
  sensors.requestTemperatures();
  actualTemp = sensors.getTempCByIndex(0);
  if(actualTemp == DEVICE_DISCONNECTED_C) 
  {
    heating = Heating::ERROR;
    actualTemp = 0.0f;
  }
  else
  {
    if(heating == Heating::ERROR && actualTemp < 100.0f && actualTemp > 0.0f) heating = Heating::OFF;
  }
}

void controllerWork()
{
  //simple bang-bang controller

  if(heating == Heating::ERROR)
  {
     execHeating();
     return;
  }

  float error = targetTemp - actualTemp;

  if(heating == Heating::OFF  && error > hysteresis)
  {
     heating = Heating::ON;
     execHeating();
     return;
  }
  if(heating == Heating::ON && error < -hysteresis)
  {
     heating = Heating::OFF;
     execHeating();
     return;
  }
}

void setup() 
{
  Serial.begin(115200);
  connectToWiFi();
  setHandlers();
  server.begin();
  sensors.begin();              
  Serial.println("HTTP server started");
}

void loop() 
{
  server.handleClient();

  unsigned long now = millis();
  if(now - previousMillis > MEASURE_PERIOD_IN_MILLIS)
  {
    Serial.println("Measure...");
    previousMillis = now;
    measure();
    controllerWork();
  }

}