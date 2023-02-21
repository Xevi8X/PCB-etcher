#include <ArduinoJson.h>

#include "host.hpp"
#include "WiFiCredentials.h"
#include "index.h"
#include "controller.hpp"


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


const char* programStatusToString(ProgramStatus state)
{
    switch(state)
    {
        case(ProgramStatus::WORKING):
            return "WORKING";
        case(ProgramStatus::ERROR_TEMP_SENSOR):
            return "ERROR_TEMP_SENSOR";
        case(ProgramStatus::ERROR_OVERHEAT):
            return "ERROR_OVERHEAT";
    }
	return "UNKNOWN";
}

const char* controllerToString(Controllers controller)
{
    switch(controller)
    {
        case(Controllers::NONE):
            return "NONE";
        case(Controllers::BANG_BANG):
            return "BANG-BANG";
        case(Controllers::PID):
            return "PID";
    }
	return "UNKNOWN";
}

void getData()
{
  const int cap = JSON_OBJECT_SIZE(5);
  char output[256]; 
  StaticJsonDocument<cap> doc;
  doc["actual"] = state.actualTemp;
  doc["target"] = state.targetTemp;
  doc["power"] = ((int)state.power)*100/255;
  doc["status"] = programStatusToString(state.status);
  doc["controller"] = controllerToString(state.controller);
  serializeJson(doc,output,128);
  myserver.send(200,"application/json",output);
}

void getControllerData()
{
  const int cap = JSON_OBJECT_SIZE(8);
  char output[512]; 
  StaticJsonDocument<cap> doc;
  doc["controller"] = controllerToString(state.controller);
  doc["kp"] = state.controller_param.Kp;
  doc["ki"] = state.controller_param.Ki;
  doc["kd"] = state.controller_param.Kd;
  doc["antywindup"] = state.controller_param.antyWindUp;
  doc["hysteresis"] = state.controller_param.hysteresis;
  doc["power"] = ((int)state.power)*100/255;
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

void setParams()
{
  String tmp; 
  if(state.controller == Controllers::NONE)
  {
    tmp = myserver.arg("power");
    if(tmp != emptyString) state.power = tmp.toInt()*255/100;
  }
  if(state.controller == Controllers::BANG_BANG)
  {
    tmp = myserver.arg("hysteresis");
    if(tmp != emptyString) state.controller_param.hysteresis = tmp.toFloat();
  }
  if(state.controller == Controllers::PID)
  {
    tmp = myserver.arg("kp");
    if(tmp != emptyString) state.controller_param.Kp = tmp.toFloat();
    tmp = myserver.arg("ki");
    if(tmp != emptyString) state.controller_param.Ki = tmp.toFloat();
    tmp = myserver.arg("kd");
    if(tmp != emptyString) state.controller_param.Kd = tmp.toFloat();
    tmp = myserver.arg("antywindup");
    if(tmp != emptyString) state.controller_param.antyWindUp = !state.controller_param.antyWindUp; 
  }
  myserver.send(200);
}

void setType()
{
  int type = myserver.arg("type").toInt();
  switch(type)
  {
    case 1:
      state.controller = Controllers::PID;
    break;
    case 2:
      state.controller = Controllers::BANG_BANG;
    break;
    case 3:
      state.controller = Controllers::NONE;
    break;
  }
  clearControllers();
  myserver.send(200);
}

void handleChartPage() {
 myserver.send(200, "text/html", chart_page); //Send web page
}

void handleControllerPage() {
 myserver.send(200, "text/html", controller_page); //Send web page
}

void setHandlers()
{
  //===COMMON DATA===
  myserver.on("/target/set", HTTPMethod::HTTP_POST , setTargetHandler);
  myserver.on("/target/increase", HTTPMethod::HTTP_POST ,[](){changeTarget(STEP);});
  myserver.on("/target/decrease", HTTPMethod::HTTP_POST , [](){changeTarget(-STEP);});
  myserver.on("/data", HTTPMethod::HTTP_GET , getData);

  //===DEBUG===
  myserver.on("/debug/get", HTTPMethod::HTTP_GET , getDebugData);
  myserver.on("/debug/set", HTTPMethod::HTTP_POST , setDebugParam);

  //===CONTROLLER===
  myserver.on("/controller/data",HTTPMethod::HTTP_GET , getControllerData);
  myserver.on("/controller/setParams", HTTPMethod::HTTP_POST , setParams);
  myserver.on("/controller/setType", HTTPMethod::HTTP_POST , setType);

  //===PAGES===
  myserver.on("/", handleRoot);
  myserver.on("/chart", HTTPMethod::HTTP_GET , handleChartPage);
  myserver.on("/controller", HTTPMethod::HTTP_GET , handleControllerPage);
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