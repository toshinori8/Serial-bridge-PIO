#include <Arduino.h>

extern bool wificonnected;

extern uint8_t myBuffer[1024];
extern int bufIdx;


extern String ssid;
extern String  password;


extern String mqtt_server;
extern int mqtt_port;
extern String mqtt_user;
extern String mqtt_pass;
extern String mqtt_allSubscriptions;


//NTP
#define TZ              0       // (utc+) TZ in hours
#define DST_MN          0      // use 60mn for summer time in some countries
#define TZ_MN           ((TZ)*60)
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)
extern timeval cbtime;      // time set in callback
extern bool cbtime_set;

void CRC32_reset();
void CRC32_update(const uint8_t& data);
uint32_t CRC32_finalize();
static  uint32_t _state;

void sendCommand(String myCommand, int raw);

void commandLoop();

void getCommand(String payload);

void connectCommand(String payload);

void mqttUserPassCommand(String payload);

void mqttServerCommand(String payload);

void subscribeCommand(String subscription);

void unsubscribeCommand(String subscription);


void mqttUserPassCommand(String payload);
void mqttServerCommand(String payload);
void subscribeCommand(String subscription);
void unsubscribeCommand(String subscription);
void sendCommand(String myCommand, int raw);
void commandLoop();
void getCommand(String payload);
void publishCommand(String payload);
void publishretainedCommand(String payload);

void loopWifi();


