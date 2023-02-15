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

//PINOUT
//==================================================
#define ONE_WIRE_PIN 2
#define TRIAC_PIN 0
#define ZERO_DETECTION_PIN 3
//==================================================


//Configuration
//==================================================
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
float hysteresis = 1.0f;
//==================================================


OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

ESP8266WebServer server(80);

float targetTemp = 40.0f;
float actualTemp = 0.0f;
Heating heating = Heating::OFF;

unsigned long previousMillis = 0;

int power = 0;

void connectToWiFi()
{
  // Connect to Wi-Fi network with SSID and password
  WiFi.setHostname("Wytrawiarka");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  MDNS.begin("wytrawiarka");
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

void getDebugData()
{
  const int cap = JSON_OBJECT_SIZE(20);
  char output[128]; 
  StaticJsonDocument<cap> doc;
  doc["state"] = "OK";
  serializeJson(doc,output,128);
  server.send(200,"application/json",output);
}

void setPower()
{
   power = server.arg("power").toInt();
   server.send(200);
}

void setHandlers()
{
  server.on("/", handleRoot);
  server.on("/target/set", HTTPMethod::HTTP_POST , setTargetHandler);
  server.on("/target/increase", HTTPMethod::HTTP_POST ,[](){changeTarget(STEP);});
  server.on("/target/decrease", HTTPMethod::HTTP_POST , [](){changeTarget(-STEP);});
  server.on("/data", HTTPMethod::HTTP_GET , getData);
  server.on("/debug", HTTPMethod::HTTP_GET , getDebugData);
  server.on("/setPower", HTTPMethod::HTTP_POST , setPower);
}

void execHeating()
{
  if(heating == Heating::ON) power = 127;
  else power = 0;
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


void IRAM_ATTR zeroCrossingISR()
{
  digitalWrite(TRIAC_PIN,HIGH);
  if(power == 0) return;
  uint32_t ticks = 12 * (255-power) + 5; // (10ms/255)/3.2us ~= 12
  timer1_write(ticks);
}

void IRAM_ATTR timerISR()
{
  digitalWrite(TRIAC_PIN,LOW);
}

void setup() 
{
  //===WIFI===
  connectToWiFi();
  setHandlers();
  server.begin();
  //===OI===
  pinMode(TRIAC_PIN, OUTPUT);
  digitalWrite(TRIAC_PIN,HIGH);
  pinMode(ZERO_DETECTION_PIN, FUNCTION_3);
  attachInterrupt(digitalPinToInterrupt(ZERO_DETECTION_PIN),zeroCrossingISR,FALLING);
  sensors.setResolution(12);
  sensors.begin(); 
  //===TIMER===
  timer1_isr_init();
  timer1_attachInterrupt(timerISR);        
  timer1_enable(TIM_DIV256, TIM_EDGE, TIM_SINGLE);
}

void loop() 
{
  server.handleClient();
  unsigned long now = millis();
  if(now - previousMillis > MEASURE_PERIOD_IN_MILLIS)
  {
    previousMillis = now;
    measure();
    controllerWork();
  }
}