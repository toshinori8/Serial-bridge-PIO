#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include "uMQTTBroker.h"


#include "main.h"
#include "crc32.h"
#include "parser.h"
#include "processJson.h"
#include "mqttBroker.h"
#include "esp-now.h"

#include <time.h>
#include <sys/time.h>     
/*
   ESP8266 MQTT Wifi Client to Serial Bridge with NTP
   Author: rkubera https://github.com/rkubera/
   
*/




IPAddress ip;  
WiFiClient espGateway;
PubSubClient client(espGateway);



//NTP
#define TZ              0       // (utc+) TZ in hours
#define DST_MN          0      // use 60mn for summer time in some countries
#define TZ_MN           ((TZ)*60)
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)
timeval cbtime;      // time set in callback
bool cbtime_set = false;


// ASYNC WEBSERVER CONFIG
#define TEMPLATE_PLACEHOLDER '^'


#include "webPage.h"



//WIFIsettings
String ssid = "oooooio";
String  password = "pmgana921";
#ifndef STASSID
#define STASSID "oooooio"
#define STAPSK  "pmgana921"
#endif

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



void time_is_set_cb(void) {
  gettimeofday(&cbtime, NULL);
  cbtime_set = true;
  if (ssid != "" && WiFi.status() == WL_CONNECTED) {
    cbtime_set = true;
  }
  else {
    cbtime_set = false;
  }
}

void mqtt_cb(char* topic, byte* payload, unsigned int length) {
  CRC32_reset();
  for (size_t i = 0; i < strlen(topic); i++) {
    CRC32_update(topic[i]);
  }

  CRC32_update(' ');

  for (size_t i = 0; i < length; i++) {
    CRC32_update(payload[i]);
  }
  uint32_t checksum = CRC32_finalize();

  Serial.print(checksum);
  Serial.print(" ");
  Serial.print(topic);
  Serial.print (" ");
  Serial.write (payload, length);
  Serial.println();
  if ((char)payload[0] == '1') {
    //digitalWrite(LED_, LOW);   // Turn the LED on (Note that LOW is the voltage level
  } else {
    //digitalWrite(LED_, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}



void reSubscribe() {
  int start = 0;
  int lineIdx;
  String sub;
  do {
    lineIdx = mqtt_allSubscriptions.indexOf('\n', start);
    if (lineIdx > -1) {
      sub = mqtt_allSubscriptions.substring(start, lineIdx);
      start = lineIdx + 1;
      sub.trim();
    }
    else {
      sub = "";
    }
    if (sub.length() > 0) {
      client.subscribe(sub.c_str());
    }
  }
  while (lineIdx > -1);
}
bool reconnect() {
  // Create a random client ID
  String clientId = "ESP8266Client-";
  clientId += String(random(0xffff), HEX);
  // Attempt to connect
  delay(100);
  if (mqtt_user != "") {
    if (client.connect(clientId.c_str(), mqtt_user.c_str(), mqtt_pass.c_str())) {
      sendCommand("mqtt connected", 0);
      return true;
    }
  }
  else {
    if (client.connect(clientId.c_str())) {
      sendCommand("mqtt connected", 0);
      return true;
    }
  }
  return false;
}


void loopWifi(){


  if (WiFi.status() == WL_CONNECTED) {


  }

  if (ssid != "" && WiFi.status() != WL_CONNECTED) {

    wificonnected = false;
    WiFi.begin(ssid.c_str(), password.c_str());
    for (int i = 0; i < 1000; i++) {
      if ( WiFi.status() != WL_CONNECTED ) {
        delay(10);
        commandLoop();
        
      }
      else {
        sendCommand("wifi connected", 0);
        wificonnected = true;

    delay(1000);
   
 ip = WiFi.localIP();
    Serial.println(ip);

  
   
        

        if (mqtt_server != "") {
          if (reconnect()) {
            reSubscribe();
          }
        }
        break;
      }
    }
  }
  else if (ssid != "") {
    if (wificonnected == true) {
      if (!client.connected() && mqtt_server != "") {
        if (reconnect()) {
          reSubscribe();
        }
      }
      else {
        client.loop();
      }
    }
  }


};


void sendCommand(String myCommand, int raw)
{

  digitalWrite(LED_BUILTIN, LOW);
delay(50);
  digitalWrite(LED_BUILTIN, HIGH);
delay(100);
  digitalWrite(LED_BUILTIN, LOW);
delay(50);
  digitalWrite(LED_BUILTIN, HIGH);




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
      http.begin(espGateway,payload_B); // Request URL
      int httpCode = http.GET();
      Serial.println("httpCode  = " + httpCode);
      if (httpCode > 0)
      { //Check the returning code
        Hpayload = http.getString(); //Get the request response payload
      }
      sendCommand(Hpayload, 1);
      http.end();
    }
  }else if (payload == "timestamp")
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

    http.begin(espGateway,"http://api.openweathermap.org/data/2.5/forecast?lat=49.8808919&lon=19.5607773&appid=f055d509de51700a688e61d5f8e3da76&units=metric&cnt=3"); //Specify request destination
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
    if (client.connected())
    {
      sendCommand("mqtt connected", 0);
    }
    else
    {
      sendCommand("mqtt not connected", 0);
    }
  }
  else if (payload == "ssid")
  {
    sendCommand(ssid, 0);
  }
  else if (payload == "mqttserver")
  {
    sendCommand(mqtt_server, 0);
  }
  else if (payload == "mqttport")
  {
    sendCommand((String)mqtt_port, 0);
  }
  else if (payload == "mqttuser")
  {
    sendCommand(mqtt_user, 0);
  }
  else
    sendCommand("error", 0);
}

void publishCommand(String payload)
{
  if (!client.connected())
  {
    sendCommand("mqtt not connected", 0);
    return;
  }
  int tmpIdx;
  tmpIdx = payload.indexOf(' ');
  if (tmpIdx > -1)
  {
    client.publish(payload.substring(0, tmpIdx).c_str(), payload.substring(tmpIdx + 1).c_str());
    sendCommand("published", 0);
  }
  else
  {
    sendCommand("wrong publish command", 0);
  }
}

void publishretainedCommand(String payload)
{
  if (!client.connected())
  {
    sendCommand("mqtt not connected", 0);
    return;
  }
  int tmpIdx;
  tmpIdx = payload.indexOf(' ');
  if (tmpIdx > -1)
  {
    client.publish(payload.substring(0, tmpIdx).c_str(), payload.substring(tmpIdx + 1).c_str(), true);
    sendCommand("published", 0);
  }
  else
  {
    sendCommand("wrong publish command", 0);
  }
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
  client.disconnect();
  WiFi.disconnect();
  wificonnected = false;
  sendCommand("connecting to wifi", 0);
}

void mqttUserPassCommand(String payload)
{
  int tmpIdx;
  tmpIdx = payload.indexOf(':');
  if (tmpIdx > -1)
  {
    //mqtt server and port
    mqtt_user = payload.substring(0, tmpIdx);
    mqtt_pass = payload.substring(tmpIdx + 1).toInt();
  }
  else
  {
    //mqtt only
    mqtt_user = payload;
  }
  client.disconnect();
  sendCommand("mqtt user and pass set", 0);
}

void mqttServerCommand(String payload)
{
  int tmpIdx;
  tmpIdx = payload.indexOf(':');
  if (tmpIdx > -1)
  {
    //mqtt server and port
    mqtt_server = payload.substring(0, tmpIdx);
    mqtt_port = payload.substring(tmpIdx + 1).toInt();
  }
  else
  {
    //mqtt only
    mqtt_server = payload;
  }
  client.disconnect();
  client.setServer(mqtt_server.c_str(), mqtt_port);
  sendCommand("connecting to mqtt server", 0);
}

void subscribeCommand(String subscription)
{
  if (!client.connected())
  {
    sendCommand("mqtt not connected", 0);
    return;
  }
  int start = 0;
  int lineIdx;
  String sub;
  bool found = false;
  do
  {
    lineIdx = mqtt_allSubscriptions.indexOf('\n', start);
    if (lineIdx > -1)
    {
      sub = mqtt_allSubscriptions.substring(start, lineIdx);
      start = lineIdx + 1;
      sub.trim();
    }
    else
    {
      sub = "";
    }
    if (sub.length() > 0)
    {
      if (sub == subscription)
      {
        found = true;
      }
    }
  } while (lineIdx > -1);
  if (found == false)
  {
    mqtt_allSubscriptions = mqtt_allSubscriptions + subscription + "\n";
    client.subscribe(subscription.c_str());
    sendCommand("subscription added", 0);
  }
  else
  {
    sendCommand("subscription exists", 0);
  }
}

void unsubscribeCommand(String subscription)
{
  if (!client.connected())
  {
    sendCommand("mqtt not connected", 0);
    return;
  }

  int start = 0;
  int lineIdx;
  String sub;
  String newAllSubscriptions = "";
  do
  {
    lineIdx = mqtt_allSubscriptions.indexOf('\n', start);
    if (lineIdx > -1)
    {
      sub = mqtt_allSubscriptions.substring(start, lineIdx);
      start = lineIdx + 1;
      sub.trim();
    }
    else
    {
      sub = "";
    }
    if (sub.length() > 0)
    {
      if (sub != subscription)
      {
        newAllSubscriptions = newAllSubscriptions + sub + "\n";
      }
    }
  } while (lineIdx > -1);
  mqtt_allSubscriptions = newAllSubscriptions;
  client.unsubscribe(subscription.c_str());
  sendCommand("subscription removed", 0);
}




void setup() {
  delay(1000);
  //ESP.eraseConfig();
  pinMode(LED_BUILTIN, OUTPUT);
  // Initialize the BUILTIN_LED pin as an output

  //Serial
  Serial.begin(9600);

  Serial.println();
  delay(200);
  sendCommand("ready", 0);


  

  WiFi.mode(WIFI_OFF);
  delay(120);
  //WIFI
  WiFi.mode(WIFI_STA);

   client.setCallback(mqtt_cb);
    

  Serial.println("Wifi Mode STA");
  //NTP
  //settimeofday_cb(time_is_set_cb);
  configTime(TZ_SEC, DST_SEC, "pool.ntp.org");


  /// OTA


    

    initWebserver();
    initMQTTserver();
  

}

void loop() {
  loopWifi();
  ArduinoOTA.handle();
  commandLoop();
  yield();


  subscribeMqttB();
   





}
