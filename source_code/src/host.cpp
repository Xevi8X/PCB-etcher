#include <ArduinoJson.h>

#include "host.hpp"
#include "WiFiCredentials.h"
#include "index.h"


ESP8266WebServer myserver(80);
extern State state;

void startServer();
void handleClientsServer();

void connectToWiFi()
{
  // Connect to Wi-Fi network with SSID and password
  WiFi.setHostname("Wytrawiarka");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  MDNS.begin("wytrawiarka");
  myserver.begin();
}

void handleRoot() {
 myserver.send(200, "text/html", MAIN_page); //Send web page
}

void setTargetHandler()
{
   state.targetTemp = myserver.arg("temp").toFloat();
   myserver.send(200);
}

void changeTarget(float diff)
{
  state.targetTemp+=diff;
  myserver.send(200);
}


const char* programStatusToString(programStatus state)
{
    switch(state)
    {
        case(programStatus::WORKING):
            return "WORKING";
        case(programStatus::ERROR_TEMP_SENSOR):
            return "ERROR_TEMP_SENSOR";
        case(programStatus::ERROR_OVERHEAT):
            return "ERROR_OVERHEAT";
    }
	return "UNKNOWN";
}

void getData()
{
  const int cap = JSON_OBJECT_SIZE(4);
  char output[256]; 
  StaticJsonDocument<cap> doc;
  doc["actual"] = state.actualTemp;
  doc["target"] = state.targetTemp;
  doc["power"] = ((int)state.power)*100/255;
  doc["status"] = programStatusToString(state.status);
  serializeJson(doc,output,128);
  myserver.send(200,"application/json",output);
}

void getDebugData()
{
  const int cap = JSON_OBJECT_SIZE(3);
  char output[128]; 
  StaticJsonDocument<cap> doc;
  doc["state"] = "OK";
  serializeJson(doc,output,128);
  myserver.send(200,"application/json",output);
}

void setDebugParam()
{
   int param = myserver.arg("param").toInt();
   myserver.send(200);
}

void setPower()
{
   state.power = myserver.arg("power").toInt();
   myserver.send(200);
}

void handleChartPage() {
 myserver.send(200, "text/html", chart_temp); //Send web page
}

void setHandlers()
{
  myserver.on("/", handleRoot);
  myserver.on("/target/set", HTTPMethod::HTTP_POST , setTargetHandler);
  myserver.on("/target/increase", HTTPMethod::HTTP_POST ,[](){changeTarget(STEP);});
  myserver.on("/target/decrease", HTTPMethod::HTTP_POST , [](){changeTarget(-STEP);});
  myserver.on("/data", HTTPMethod::HTTP_GET , getData);
  myserver.on("/debug/get", HTTPMethod::HTTP_GET , getDebugData);
  myserver.on("/debug/set", HTTPMethod::HTTP_POST , setDebugParam);
  myserver.on("/setPower", HTTPMethod::HTTP_POST , setPower);

  myserver.on("/chart", HTTPMethod::HTTP_GET , handleChartPage);
}

void startServer()
{
  connectToWiFi();
  setHandlers();
  myserver.begin();
}

void handleClientsServer()
{
    myserver.handleClient();
}