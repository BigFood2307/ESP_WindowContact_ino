#include <ESP8266WiFi.h>
#include <MQTT.h>

#include "user_config_defaults.h"
#include "version.h"

#define DEBUG_LEVEL 0
#include "debugtool.h"

void myDataCb(String& topic, String& data);
void myPublishedCb();
void myDisconnectedCb();
void myConnectedCb();

#define CONTACT_PIN 3

MQTT *myMqtt;

byte contact = 0;

unsigned long milli;

unsigned long startTime = 0;
  
ADC_MODE(ADC_VCC);

void setup() {
  // put your setup code here, to run once:

  DEBUG_L1(Serial.begin(115200,SERIAL_8N1,SERIAL_TX_ONLY));
  DEBUG_L1(Serial.println());
  DEBUG_L1(Serial.println("Startup..."));

  milli = millis();

  String lwtTopic(CONFIG_BASE_TOPIC);
  lwtTopic.concat(CONFIG_LWT_TOPIC);
  
  myMqtt = new MQTT(CONFIG_CLIENT_ID, CONFIG_MQTT_SERVER, CONFIG_MQTT_PORT, lwtTopic.c_str(), 0, true, CONFIG_LWT_MSG);
  
  pinMode(CONTACT_PIN, INPUT);
  contact = !digitalRead(CONTACT_PIN);
  
  DEBUG_L1(Serial.println("Connecting WiFi..."));

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  startTime = millis();
  while (WiFi.status() != WL_CONNECTED) 
  {  
    if(millis()-startTime > CONFIG_WIFI_TO)
    {      
      DEBUG_L1(Serial.println("Going to deepsleep."));
      ESP.deepSleep(0);
    }
    DEBUG_L4(Serial.print("."));
    DEBUG_L4(delay(100));
    yield();
  }
  DEBUG_L4(Serial.println());
  DEBUG_L1(Serial.println("WiFi connected!"));
  
  DEBUG_L1(Serial.println("Connecting MQTT..."));
  myMqtt->onConnected(myConnectedCb);
  myMqtt->onDisconnected(myDisconnectedCb);
  myMqtt->onPublished(myPublishedCb);
  myMqtt->onData(myDataCb);
  myMqtt->setUserPwd(CONFIG_MQTT_LOGIN, CONFIG_MQTT_PASS);
  startTime = millis();
  myMqtt->connect();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  yield();
  if(millis()-startTime > CONFIG_MQTT_TO)
  {
    DEBUG_L1(Serial.println("Going to deepsleep."));
    ESP.deepSleep(0);
  }
}

void myConnectedCb()
{  
  DEBUG_L1(Serial.println("Connected MQTT"));
  String contactStr = "OPEN";
  String durationStr(millis()-milli);

  if(contact)
  {
    contactStr = "CLOSED";
  }

  float voltage = ESP.getVcc();

  String batStr(voltage);

  String message("{\"Contact\":");
  message += contactStr;
  message += ",\"Duration\":";
  message += durationStr;
  message += ",\"Battery\":";
  message += batStr;
  message += ",\"SW-Version\":";
  message += SW_VERSION;
  message += "}";
  
  String contactTopic(CONFIG_BASE_TOPIC);
  contactTopic.concat(CONFIG_CONTACT_TOPIC);
  
  DEBUG_L1(Serial.print(contactTopic));
  DEBUG_L1(Serial.print(": "));
  DEBUG_L1(Serial.println(message));

  // publish value to topic
  boolean result = myMqtt->publish(contactTopic.c_str(), message, 1, 1);
}

void myDisconnectedCb()
{
  //Serial.println("disconnected. try to reconnect...");
  DEBUG_L1(Serial.println("Going to deepsleep."));
  ESP.deepSleep(0);
}

void myPublishedCb()
{
  //Serial.print("Publishing took ms: ");
  //Serial.println(millis() - milli);
  DEBUG_L1(Serial.println("Going to deepsleep."));
  ESP.deepSleep(0);
}

void myDataCb(String& topic, String& data)
{  
  //Serial.print(topic);
  //Serial.print(": ");
  //Serial.println(data);
}
