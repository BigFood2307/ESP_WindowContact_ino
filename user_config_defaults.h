
#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H

//---OVERWRITE------------------------------

//Uncomment to use overwrite file
//#define USE_CONFIG_OVERWRITE

//---CONFIG_HOLDER--------------------------

#define CONFIG_HOLDER 0x1234

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

//If the following line is defined, the code assumes you are using an NC reed contact (Circuit closed, when window closed)
//To use an NO Type, undefine it in the overwrite. 
//NC is more secure, as cutting the cable will be registered as opening. NO usually uses less energy, as no current is flowing when the window is closed
#define CONFIG_CONTACT_NC

//---OVERWRITE------------------------------

#ifdef USE_CONFIG_OVERWRITE
#include "user_config_overwrite.h"
#endif

#endif
