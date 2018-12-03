
#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H

#include "credentials.h"

//---OVERWRITE------------------------------

//Uncomment to use overwrite file
//#define USE_CONFIG_OVERWRITE

//---WIFI-----------------------------------

#define CONFIG_WIFI_SSID    WIFI_SSID
#define CONFIG_WIFI_PASS    WIFI_PASS
#define CONFIG_WIFI_TO      10000

//---MQTT----------------------------------

#define CONFIG_MQTT_SERVER  MQTT_SERVER
#define CONFIG_MQTT_PORT    MQTT_PORT
#define CONFIG_MQTT_LOGIN   MQTT_LOGIN
#define CONFIG_MQTT_PASS    MQTT_PASS
#define CONFIG_MQTT_TO      5000

#define CONFIG_CLIENT_ID      "BM_TE_Contact1"
#define CONFIG_BASE_TOPIC     "home/BM/test/contact1/"

#define CONFIG_LWT_TOPIC        "LWT"
#define CONFIG_CONTACT_TOPIC    "CONTACT"
#define CONFIG_LWT_MSG          "Offline"

#ifdef USE_CONFIG_OVERWRITE
#include "user_config_overwrite.h"
#endif

#endif
