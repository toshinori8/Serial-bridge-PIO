#include <Arduino.h>
// #include "main.h"

#include "uMQTTBroker.h"
#include "mqttBroker.h"

int counter;

myMQTTBroker myBroker;


void initMQTTserver(){


  // Start the broker
  Serial.println("Starting MQTT broker");
    myBroker.init();
    myBroker.subscribe("#");


}

void subscribeMqttB(){

myBroker.publish("broker/counter", (String)counter++);


}