#include <ESP8266WiFi.h>
#include <MQTT.h>

#include "user_config_defaults.h"
#include "version.h"
#include "permanent_config.h"

#define DEBUG_LEVEL 0
#include "debugtool.h"

void vDataCb(String& asTopic, String& asData);
void vPublishedCb();
void vDisconnectedCb();
void vConnectedCb();

MQTT *oMqtt;

struct
{
  char acWifiSSID[31] = CONFIG_WIFI_SSID;
  char acWifiPassword[31] = CONFIG_WIFI_PASS;
  unsigned int uiWifiTimeout = CONFIG_WIFI_TO;

  char acMqttServer[31] = CONFIG_MQTT_SERVER;
  unsigned int uiMqttPort = CONFIG_MQTT_PORT;
  char acMqttLogin[21] = CONFIG_MQTT_LOGIN;
  char acMqttPassword[31] = CONFIG_MQTT_PASS;
  unsigned int uiMqttTimeout = CONFIG_MQTT_TO;
  char acMqttClientId[31] = CONFIG_CLIENT_ID;
  char acMqttBaseTopic[31] = CONFIG_BASE_TOPIC;
  char acMqttLastWillTopic[11] = CONFIG_LWT_TOPIC;
  char acMqttContactTopic[11] = CONFIG_CONTACT_TOPIC;
  char acMqttLastWillMessage[21] = CONFIG_LWT_MSG;
  unsigned char ucContactPin = CONFIG_CONTACT_PIN;
  char acContactMessageOpen[11] = CONFIG_CONTACT_MSG_OPEN;
  char acContactMessageClosed[11] = CONFIG_CONTACT_MSG_CLOSED;
} oConfigData;

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

  vInitPermanentConfig(CONFIG_HOLDER, sizeof(oConfigData));
  vLoadConfig((void *)(&oConfigData));

  String sLwtTopic(oConfigData.acMqttBaseTopic);
  sLwtTopic.concat(oConfigData.acMqttLastWillTopic);
  
  oMqtt = new MQTT(oConfigData.acMqttClientId, oConfigData.acMqttServer, oConfigData.uiMqttPort, sLwtTopic.c_str(), 0, true, oConfigData.acMqttLastWillMessage);
  
  pinMode(oConfigData.ucContactPin, INPUT);
  bContact = !digitalRead(oConfigData.ucContactPin);
  
  vConnectWifi();
  vConnectMqtt();  
}

void loop() {
  // put your main code here, to run repeatedly:
  if(millis()-ulWaitStartTime > oConfigData.uiMqttTimeout)
  {
    DEBUG_L1(Serial.println("Going to deepsleep."));
    ESP.deepSleep(0);
  }
  yield();
}

void vConnectWifi()
{
  DEBUG_L1(Serial.println("Connecting WiFi..."));
  
  WiFi.hostname(oConfigData.acMqttClientId);  
  WiFi.mode(WIFI_STA);
  WiFi.begin(oConfigData.acWifiSSID, oConfigData.acWifiPassword);
  ulWaitStartTime = millis();
  while (WiFi.status() != WL_CONNECTED) 
  {  
    if(millis()-ulWaitStartTime > oConfigData.uiWifiTimeout)
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
  oMqtt->setUserPwd(oConfigData.acMqttLogin, oConfigData.acMqttPassword);
  ulWaitStartTime = millis();
  oMqtt->connect();
}

String sFormatMessage()
{
  String sDuration(millis()-ulStartupTime);

  #ifdef CONFIG_CONTACT_NC
  String sContact = oConfigData.acContactMessageOpen;
  if(bContact)
  {
    sContact = oConfigData.acContactMessageClosed;
  }
  #else
  String sContact = oConfigData.acContactMessageClosed;
  if(bContact)
  {
    sContact = oConfigData.acContactMessageOpen;
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
  
  String sContactTopic(oConfigData.acMqttBaseTopic);
  sContactTopic.concat(oConfigData.acMqttContactTopic);
  
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
