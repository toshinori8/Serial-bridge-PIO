#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 2
#define FANPIN 0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#include <EEPROM.h>

#include <ESP8266WiFi.h>





#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>

#include <WiFiUdp.h>
#include <ArduinoOTA.h>
//#include <espnow.h>
#define TEMPLATE_PLACEHOLDER '^'



/////// OLED DISPLAY
//#include <SPI.h>
//#include <Wire.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
//#define SCREEN_WIDTH 128
//#define SCREEN_HEIGHT 32
//#define OLED_RESET -1
//#define SCREEN_ADDRESS 0x3C
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



#include <Timers.h>
Timers <4> timers;

int temp, humi;
String fan = "checked";

void otaStart();

void termostatRules();
void setFan();
void readDHT();

AsyncWebServer server(80);





const char* ssid = "oooooio";
const char* password = "pmgana921";

const char* PARAM_INPUT_1 = "setTemp";
const char* PARAM_INPUT_2 = "fanControll";
int setTemp;
int tempMin = 10;
int tempMax = 50;
String fanControll;



#include "webPage.h"
String processor(const String& var)
{
  if (var == "_setTemp_")
    return String(setTemp);

  if (var == "_tempMin_")
    return String(tempMin);

  if (var == "_tempMax_")
    return String(tempMax);

}





void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup(void) {


//********** CHANGE PIN FUNCTION  TO GPIO **********
//GPIO 1 (TX) swap the pin to a GPIO.
//pinMode(1, FUNCTION_3); 
//GPIO 3 (RX) swap the pin to a GPIO.
//pinMode(3, FUNCTION_3); 
//**************************************************

  
  // Wire.begin(D5, D6);

  pinMode(FANPIN, OUTPUT);
  digitalWrite(FANPIN, HIGH);


  dht.begin();
  pinMode(0, OUTPUT); digitalWrite(0, HIGH);

  timers.attach(0, 3000, termostatRules);
  timers.attach(2, 4332, readDHT);
  /////////////////////////////////////     OLED DISOPLAY


  //   if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
  //    Serial.println(F("SSD1306 allocation failed"));
  //    for(;;); // Don't proceed, loop forever
  //    }
  //
  //
  //  displayData();
  /////////////////////////////////////     OLED DISOPLAY



  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.begin(115200);
  EEPROM.begin(512);
  setTemp = EEPROM.read(0);


  WiFi.begin(ssid, password);



  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("eeprom data:");
  Serial.print(setTemp);

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  if (MDNS.begin("Rancho")) {

    Serial.println("MDNS Responder Started");
    server.begin();
    //u8g2.begin();
  }
  otaStart();



  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", webpage, processor);
  });

  server.on("/setTemp", [](AsyncWebServerRequest * request) {
    if (request->hasParam(PARAM_INPUT_1)) {
      setTemp = request->getParam(PARAM_INPUT_1)->value().toInt();

    }
    Serial.println(PARAM_INPUT_1);
    Serial.println(setTemp);
    EEPROM.write(0, setTemp);   // To store in 0th Location
    EEPROM.commit();



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
    json += "\"temp\":\"" + String(temp) + "\"";
    json += ",\"humi\":\"" + String(humi) + "\"";
    json += ",\"fan\":\"" + fan + "\"";
    json += ",\"setTemp\":\"" + String(setTemp) + "\"";
    //      json += ",\"secure\":"+String(WiFi.encryptionType(i));
    json += "}";

    json += "";
    request->send(200, "application/json", json);
    json = String();
  });

  //  server.on("/temp", [](AsyncWebServerRequest *request){
  //    Serial.println("right");
  //    analogWrite(STEER_MOTOR_POWER, steeringPower);
  //    digitalWrite(STEER_MOTOR_DIRECTION, steerDirection);
  //    request->send_P(200, "text/html", "right");
  //  });
  //

  server.onNotFound(notFound);


  Serial.println("HTTP Server Started");
}



void loop(void) {



  timers.process();
  ArduinoOTA.handle();
}
