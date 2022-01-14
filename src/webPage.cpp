#include <Arduino.h>
#include "webpage.h"
#include <ESPAsyncWebServer.h>
AsyncWebServer server(80);

String processor(const String& var)
{
  // if (var == "_setTemp_")
  //   return String(setTemp);

  // if (var == "_tempMin_")
  //   return String(tempMin);

  // if (var == "_tempMax_")
  //   return String(tempMax);
        return String(80);
}

const char* PARAM_INPUT_1 = "setTemp";
const char* PARAM_INPUT_2 = "fanControll";
int setTemp;
int tempMin = 10;
int tempMax = 50;
String fanControll;



void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void initWebserver(){

    server.begin();



  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", webpage, processor);
  });

  server.on("/setTemp", [](AsyncWebServerRequest * request) {
    if (request->hasParam(PARAM_INPUT_1)) {
      setTemp = request->getParam(PARAM_INPUT_1)->value().toInt();

    }
   // Serial.println(PARAM_INPUT_1);
   // Serial.println(setTemp);
    //EEPROM.write(0, setTemp);   // To store in 0th Location
    //EEPROM.commit();



    request->send_P(200, "text/html", "setTempOK");
  });

  server.on("/fanControll", [](AsyncWebServerRequest * request) {
    if (request->hasParam(PARAM_INPUT_2)) {
      fanControll = request->getParam(PARAM_INPUT_2)->value();

    }
    Serial.println(PARAM_INPUT_2);
    Serial.println(fanControll);


    request->send_P(200, "text/html", "fanControllOK");
  });





  server.on("/params", HTTP_GET, [](AsyncWebServerRequest * request) {
    String json = "";

    json += "{";
    //json += "\"temp\":\"" + String(temp) + "\"";
   // json += ",\"humi\":\"" + String(humi) + "\"";
    //json += ",\"fan\":\"" + fan + "\"";
    //json += ",\"setTemp\":\"" + String(setTemp) + "\"";
    //      json += ",\"secure\":"+String(WiFi.encryptionType(i));
    json += "}";

    json += "";
    request->send(200, "application/json", json);
    json = String();
  });

 

  server.onNotFound(notFound);


  Serial.println("HTTP Server Started");



};