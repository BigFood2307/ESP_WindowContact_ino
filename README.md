# ESP_WindowContact_ino
SW for an ESP + MQTT based WindowContact for your smart Home

For the necessary HW look at the other Repository (ESP_WindowContact_HW)

The SW only runs once whenever a change is detected. It then sends an MQTT message on the defined topic.

Adjust to your own settings in user_settings_overwrite.h (copy the sample for simplicity)
I use a seperate credentials.h library file to keep my credentials centrally stored for all projects and out of github. You can just define them in the file if you want however.

Settings are saved even over a SW update, unless the CONFIG_HOLDER is changed.

This SW was originally written for the ESP8266 arduino library 2.5.2.
I very much hope it still works on newer versions.

For more info just ask!
