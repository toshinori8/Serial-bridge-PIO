#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>

#include <ESP8266HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <espnow.h>
#include "uMQTTBroker.h"

#include "main.h"
#include "crc32.h"
#include "parser.h"
#include "processJson.h"
#include "mqttBroker.h"

#include <time.h>
#include <sys/time.h>

/*
   BASED ON ESP8266 MQTT Wifi Client to Serial Bridge with NTP rkubera https://github.com/rkubera/
   Author: Adam Karski
   
*/

IPAddress ip;
WiFiClient espGateway;
//PubSubClient client(espGateway);

//NTP
#define TZ 0     // (utc+) TZ in hours
#define DST_MN 0 // use 60mn for summer time in some countries
#define TZ_MN ((TZ)*60)
#define TZ_SEC ((TZ)*3600)
#define DST_SEC ((DST_MN)*60)
timeval cbtime; // time set in callback
bool cbtime_set = false;



// ASYNC WEBSERVER CONFIG
#define TEMPLATE_PLACEHOLDER '^'
#include "webPage.h"

//WIFIsettings
String ssid = "oooooio";
String password = "pmgana921";
int status = WL_IDLE_STATUS;

#define STASSID "oooooio"
#define STAPSK "pmgana921"

bool wificonnected = false;

String mqtt_server = "192.168.8.150";
int mqtt_port = 1883;
String mqtt_user = "mqtt";
String mqtt_pass = STAPSK;
String mqtt_allSubscriptions = "home/MQTTGateway/#";

//BUFFER
#define BUFFER_SIZE 1024
uint8_t myBuffer[BUFFER_SIZE];
int bufIdx = 0;

//MQTT payload
long lastMsg = 0;
char msg[1024];
long int value = 0;

void time_is_set_cb(void)
{
  gettimeofday(&cbtime, NULL);
  cbtime_set = true;
  if (ssid != "" && WiFi.status() == WL_CONNECTED)
  {
    cbtime_set = true;
  }
  else
  {
    cbtime_set = false;
  }
}

void mqtt_cb(char *topic, byte *payload, unsigned int length)
{
  CRC32_reset();
  for (size_t i = 0; i < strlen(topic); i++)
  {
    CRC32_update(topic[i]);
  }

  CRC32_update(' ');

  for (size_t i = 0; i < length; i++)
  {
    CRC32_update(payload[i]);
  }
  uint32_t checksum = CRC32_finalize();

  Serial.print(checksum);
  Serial.print(" ");
  Serial.print(topic);
  Serial.print(" ");
  Serial.write(payload, length);
  Serial.println();
  if ((char)payload[0] == '1')
  {
    //digitalWrite(LED_, LOW);   // Turn the LED on (Note that LOW is the voltage level
  }
  else
  {
    //digitalWrite(LED_, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}

/////  ESP NOW
struct dataStruct {


 /// Sensors DATA 
    String name;
  int temp;
  int humidity;
  long lastSeen;
  String macAddr;
  int battery;



  
};
dataStruct dataBeam;
  
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {

  
  memcpy(&dataBeam, incomingData, sizeof(dataBeam));

Serial.println("RECIVIG DATA ON ESP_NOW");
 
  Serial.println("dataBeam.temp");
  Serial.println(String(dataBeam.temp));


 
}


void initESP_NOW(){
  
if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }else{
        Serial.println("ESP-NOW OK");

    }
  
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);  
  
  
  }
  






void initServices() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.print("Connecting WIFI");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
    commandLoop(); 
    initWebserver();
    initMQTTserver();
    initESP_NOW(); 
    ip = WiFi.localIP();
    Serial.println(ip);
}
    
    


void sendCommand(String myCommand, int raw)
{

  if (raw == 1)
  {
    Serial.print(myCommand);
  }
  else
  {
    CRC32_reset();
    CRC32_update('[');
    for (size_t i = 0; i < myCommand.length(); i++)
    {
      CRC32_update(myCommand[i]);
    }
    CRC32_update(']');
    uint32_t checksum = CRC32_finalize();

    Serial.print(checksum);
    Serial.print(" ");
    Serial.print("[");
    Serial.print(myCommand);
    Serial.println("]");
  }
}

void commandLoop()
{
  if (getSerialBuffer((char *)myBuffer, bufIdx))
  {
    parseLines((char *)myBuffer);
  }
}

void getCommand(String payload)
{
  int xtmpIdx;
  xtmpIdx = payload.indexOf('*');
  if (xtmpIdx > -1)
  {
    String payload_A = payload.substring(0, xtmpIdx);
    String payload_B = payload.substring(xtmpIdx + 1);
    if (payload_A == "url")
    {
      String Hpayload;
      HTTPClient http;
      http.begin(espGateway, payload_B); // Request URL
      int httpCode = http.GET();
      Serial.println("httpCode  = " + httpCode);
      if (httpCode > 0)
      {                              //Check the returning code
        Hpayload = http.getString(); //Get the request response payload
      }
      sendCommand(Hpayload, 1);
      http.end();
    }
  }
  else if (payload == "timestamp")
  {
    uint32_t mytimestamp = 0;
    mytimestamp = time(nullptr);
    String command = "timestamp ";
    if (cbtime_set == true)
    {
      mytimestamp = time(nullptr);
      if (mytimestamp < 1000000)
      {
        mytimestamp = 0;
      }
    }
    command = command + (String)mytimestamp;
    sendCommand(command, 0);
  }
  else if (payload == "echo")
  {
    sendCommand("echo", 0);
  }
  else if (payload == "ip")
  {
    IPAddress ip = WiFi.localIP();
    String command =
        String(ip[0]) + "." +
        String(ip[1]) + "." +
        String(ip[2]) + "." +
        String(ip[3]);
    sendCommand(command, 0);
  }

  /// GET FORECAST BY SERIAL CONSOLE

  else if (payload == "forecast_5h")
  {
    String Hpayload;
    HTTPClient http;

    http.begin(espGateway, "http://api.openweathermap.org/data/2.5/forecast?lat=49.8808919&lon=19.5607773&appid=f055d509de51700a688e61d5f8e3da76&units=metric&cnt=3"); //Specify request destination
    int httpCode = http.GET();

    //Serial.println("httpCode  = " + httpCode);

    if (httpCode > 0)
    { //Check the returning code

      Hpayload = http.getString(); //Get the request response payload
    }

    //Serial.print(Hpayload);

    processJson(Hpayload);
    //sendCommand(Hpayload, 1);
    http.end();
  }
  else if (payload == "wifistatus")
  {
    if (wificonnected == true)
    {
      sendCommand("wifi connected", 0);
    }
    else
    {
      sendCommand("wifi not connected", 0);
    }
  }
  else if (payload == "mqttstatus")
  {
    // if (client.connected())
    // {
    //   sendCommand("mqtt connected", 0);
    // }
    // else
    // {
    //   sendCommand("mqtt not connected", 0);
    // }
  }
  else if (payload == "ssid")
  {
    sendCommand(ssid, 0);
  }
  else
    sendCommand("error", 0);
}

void connectCommand(String payload)
{
  int tmpIdx;
  tmpIdx = payload.indexOf(':');
  if (tmpIdx > -1)
  {
    //AP ssid and password
    ssid = payload.substring(0, tmpIdx);
    password = payload.substring(tmpIdx + 1);
  }
  else
  {
    //open AP, only ssid
    ssid = payload;
  }
  //client.disconnect();
  WiFi.disconnect();
  wificonnected = false;
  sendCommand("connecting to wifi", 0);
}

void setup()
{
  delay(1000);
  //ESP.eraseConfig();
  pinMode(LED_BUILTIN, OUTPUT);
  // Initialize the BUILTIN_LED pin as an output

  //Serial
  Serial.begin(9600);

  Serial.println();
  delay(200);
  sendCommand("ready", 0);

  WiFi.mode(WIFI_AP_STA);
  delay(120);
  //WIFI
  WiFi.mode(WIFI_AP_STA);
  
  initServices();

  configTime(TZ_SEC, DST_SEC, "pool.ntp.org");



  
}

void loop()
{
  if ((WiFi.status() != WL_CONNECTED)){

    Serial.println("wifi lost");
      WiFi.reconnect();

  };
  ArduinoOTA.handle();
  commandLoop();
  yield();
}
