
#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H

//---OVERWRITE------------------------------

//Uncomment to use overwrite file
//#define USE_CONFIG_OVERWRITE

//---WIFI-----------------------------------

#define CONFIG_WIFI_SSID    "SSID"
#define CONFIG_WIFI_PASS    "PASSWORD"
#define CONFIG_WIFI_TO      10000

//---MQTT----------------------------------

#define CONFIG_MQTT_SERVER  "openhabianpi"
#define CONFIG_MQTT_PORT    1887
#define CONFIG_MQTT_LOGIN   "opehabian"
#define CONFIG_MQTT_PASS    "PASSWORD"
#define CONFIG_MQTT_TO      5000

#define CONFIG_CLIENT_ID      "BM_TE_Contact1"
#define CONFIG_BASE_TOPIC     "home/BM/test/contact1/"

#define CONFIG_LWT_TOPIC        "LWT"
#define CONFIG_CONTACT_TOPIC    "CONTACT"
#define CONFIG_LWT_MSG          "Offline"

//---CONTACT------------------------------

#define CONFIG_CONTACT_PIN        3
#define CONFIG_CONTACT_MSG_OPEN   "OPEN"
#define CONFIG_CONTACT_MSG_CLOSED "CLOSED"

//---OVERWRITE------------------------------

#ifdef USE_CONFIG_OVERWRITE
#include "user_config_overwrite.h"
#endif

#endif
