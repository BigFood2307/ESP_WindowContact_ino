#include <ESP8266WiFi.h>
#include <MQTT.h>

#include "user_config_defaults.h"
#include "version.h"

#define DEBUG_LEVEL 0
#include "debugtool.h"

void vDataCb(String& asTopic, String& asData);
void vPublishedCb();
void vDisconnectedCb();
void vConnectedCb();

MQTT *oMqtt;

byte bContact = 0;

unsigned long ulStartupTime;
unsigned long ulWaitStartTime = 0;

void vConnectWifi();
void vConnectMqtt();
  
ADC_MODE(ADC_VCC);

void setup() {
  // put your setup code here, to run once:

  DEBUG_L1(Serial.begin(115200,SERIAL_8N1,SERIAL_TX_ONLY));
  DEBUG_L1(Serial.println());
  DEBUG_L1(Serial.println("Startup..."));

  ulStartupTime = millis();

  String sLwtTopic(CONFIG_BASE_TOPIC);
  sLwtTopic.concat(CONFIG_LWT_TOPIC);
  
  oMqtt = new MQTT(CONFIG_CLIENT_ID, CONFIG_MQTT_SERVER, CONFIG_MQTT_PORT, sLwtTopic.c_str(), 0, true, CONFIG_LWT_MSG);
  
  pinMode(CONFIG_CONTACT_PIN, INPUT);
  bContact = !digitalRead(CONFIG_CONTACT_PIN);
  
  vConnectWifi();
  vConnectMqtt();  
}

void loop() {
  // put your main code here, to run repeatedly:
  if(millis()-ulWaitStartTime > CONFIG_MQTT_TO)
  {
    DEBUG_L1(Serial.println("Going to deepsleep."));
    ESP.deepSleep(0);
  }
  yield();
}

void vConnectWifi()
{
  DEBUG_L1(Serial.println("Connecting WiFi..."));
  WiFi.hostname(CONFIG_CLIENT_ID);
  WiFi.begin(CONFIG_WIFI_SSID, CONFIG_WIFI_PASS);
  ulWaitStartTime = millis();
  while (WiFi.status() != WL_CONNECTED) 
  {  
    if(millis()-ulWaitStartTime > CONFIG_WIFI_TO)
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
}

void vConnectMqtt()
{
  DEBUG_L1(Serial.println("Connecting MQTT..."));
  oMqtt->onConnected(vConnectedCb);
  oMqtt->onDisconnected(vDisconnectedCb);
  oMqtt->onPublished(vPublishedCb);
  oMqtt->onData(vDataCb);
  oMqtt->setUserPwd(CONFIG_MQTT_LOGIN, CONFIG_MQTT_PASS);
  ulWaitStartTime = millis();
  oMqtt->connect();
}

String sFormatMessage()
{
  String sDuration(millis()-ulStartupTime);

  #ifdef CONFIG_CONTACT_NC
  String sContact = CONFIG_CONTACT_MSG_OPEN;
  if(bContact)
  {
    sContact = CONFIG_CONTACT_MSG_CLOSED;
  }
  #else
  String sContact = CONFIG_CONTACT_MSG_CLOSED;
  if(bContact)
  {
    sContact = CONFIG_CONTACT_MSG_OPEN;
  }
  #endif

  float fVoltage = ESP.getVcc();

  String sBattery(fVoltage);

  String sMessage("{\"Contact\":");
  sMessage += sContact;
  sMessage += ",\"Duration\":";
  sMessage += sDuration;
  sMessage += ",\"Battery\":";
  sMessage += sBattery;
  sMessage += ",\"SW-Version\":";
  sMessage += SW_VERSION;
  sMessage += "}";

  return sMessage;
}

void vConnectedCb()
{  
  DEBUG_L1(Serial.println("Connected MQTT"));

  String sMessage = sFormatMessage();
  
  String sContactTopic(CONFIG_BASE_TOPIC);
  sContactTopic.concat(CONFIG_CONTACT_TOPIC);
  
  DEBUG_L1(Serial.print(sContactTopic));
  DEBUG_L1(Serial.print(": "));
  DEBUG_L1(Serial.println(sMessage));

  // publish value to topic
  boolean bResult = oMqtt->publish(sContactTopic.c_str(), sMessage, 1, 1);
}

void vDisconnectedCb()
{
  //Serial.println("disconnected. try to reconnect...");
  DEBUG_L1(Serial.println("Going to deepsleep."));
  ESP.deepSleep(0);
}

void vPublishedCb()
{
  //Serial.print("Publishing took ms: ");
  //Serial.println(millis() - milli);
  DEBUG_L1(Serial.println("Going to deepsleep."));
  ESP.deepSleep(0);
}

void vDataCb(String& asTopic, String& asData)
{  
  //Serial.print(topic);
  //Serial.print(": ");
  //Serial.println(data);
}
