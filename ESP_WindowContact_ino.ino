#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "user_config_defaults.h"
#include "version.h"
#include "permanent_config.h"

#define DEBUG_LEVEL 0
#include "debugtool.h"

void vPublishedCb();
void vDisconnectedCb();
void vConnectedCb();

WiFiClient oWifiClient;
PubSubClient oMqtt(oWifiClient);

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

  if (WiFi.SSID() != oConfigData.acWifiSSID)
  {
    WiFi.hostname(oConfigData.acMqttClientId);  
    WiFi.mode(WIFI_STA);
    WiFi.begin(oConfigData.acWifiSSID, oConfigData.acWifiPassword);
    WiFi.persistent(true);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    DEBUG_L1(Serial.println(WiFi.status()));
  }
  else
  {
    DEBUG_L1(Serial.println("Setup still valid."));
    DEBUG_L1(Serial.println(WiFi.status()));
    WiFi.begin();
  }
  ulWaitStartTime = millis();
  while (WiFi.status() != WL_CONNECTED) 
  {  
    if(millis()-ulWaitStartTime > oConfigData.uiWifiTimeout)
    {      
      DEBUG_L1(Serial.println("Going to deepsleep."));
      WiFi.begin(oConfigData.acWifiSSID, oConfigData.acWifiPassword);
      delay(1000);
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
  
  String sLwtTopic(oConfigData.acMqttBaseTopic);
  sLwtTopic.concat(oConfigData.acMqttLastWillTopic);
  
  oMqtt.setServer(oConfigData.acMqttServer, oConfigData.uiMqttPort);
  ulWaitStartTime = millis();

  while (!oMqtt.connected()) {
    if (oMqtt.connect(oConfigData.acMqttClientId, oConfigData.acMqttLogin, oConfigData.acMqttPassword, sLwtTopic.c_str(), 0, true, oConfigData.acMqttLastWillMessage)) {
      vConnectedCb();
    } else {
      DEBUG_L1(Serial.print("failed, rc="));
      DEBUG_L1(Serial.print(oMqtt.state()));
      if((millis() - ulWaitStartTime) >= oConfigData.uiMqttTimeout)
      {
        DEBUG_L1(Serial.println());
        DEBUG_L1(Serial.println("MQTT TO"));
        DEBUG_L1(Serial.println("Going to deepsleep"));
        ESP.deepSleep(0);
      }
      DEBUG_L1(Serial.println(" try again in 0.2 seconds"));
      // Wait 0.2 seconds before retrying
      delay(200);
    }
  }
  
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
  boolean bResult = oMqtt.publish(sContactTopic.c_str(), sMessage.c_str(), true);
  delay(100);

  vPublishedCb();
}

void vPublishedCb()
{
  DEBUG_L1(Serial.println("Going to deepsleep."));
  ESP.deepSleep(0);
}
