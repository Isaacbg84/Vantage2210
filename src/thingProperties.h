// Code generated by Arduino IoT Cloud, DO NOT EDIT.

#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "secrets.h"

const char DEVICE_LOGIN_NAME[]  = "0be567cc-b526-432d-9846-bb8c71cf4412";

const char SSID[]               = SECRET_SSID;    // Network SSID (name)
const char PASS[]               = SECRET_OPTIONAL_PASS;    // Network password (use for WPA, or use as key for WEP)
const char DEVICE_KEY[]  =        SECRET_DEVICE_KEY;    // Secret device password

void onSwitch1Change();

bool switch1;

void initProperties(){

  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);
  ArduinoCloud.addProperty(switch1, READWRITE, ON_CHANGE, onSwitch1Change);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
