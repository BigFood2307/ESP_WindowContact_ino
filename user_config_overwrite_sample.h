
#ifndef __USER_CONFIG_OVERWRITE_H
#define __USER_CONFIG_OVERWRITE_H

#include "credentials.h"

//---WIFI-----------------------------------

#undef  CONFIG_WIFI_SSID
#define CONFIG_WIFI_SSID    WIFI_SSID
#undef  CONFIG_WIFI_PASS
#define CONFIG_WIFI_PASS    WIFI_PASS

//---MQTT----------------------------------

#undef  CONFIG_MQTT_SERVER
#define CONFIG_MQTT_SERVER  MQTT_SERVER
#undef  CONFIG_MQTT_PORT
#define CONFIG_MQTT_PORT    MQTT_PORT
#undef  CONFIG_MQTT_LOGIN
#define CONFIG_MQTT_LOGIN   MQTT_LOGIN
#undef  CONFIG_MQTT_PASS
#define CONFIG_MQTT_PASS    MQTT_PASS

#undef  CONFIG_CLIENT_ID
#define CONFIG_CLIENT_ID      "BM_LU_Contact1"
#undef  CONFIG_BASE_TOPIC
#define CONFIG_BASE_TOPIC     "home/BM/Lukas/contact1/"

#endif
