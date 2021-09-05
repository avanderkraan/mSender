#ifndef HANDLEHTTPCLIENT_H    // To make sure you don't declare the function more than once by including the header multiple times.
#define HANDLEHTTPCLIENT_H
#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <ESP8266HTTPClient.h> 
#include "settings.h"
#include "WiFiSettings.h"
#include "asyncHTTPrequest.h"

/* contains the html that is send to the targetServer */
String getSendData(Settings * pSettings, String macddress, uint32_t revolutions, uint32_t bladesPerMinute);
/* contains the html that is send to the targetServer */
String getSendInfo(Settings * pSettings, WiFiSettings* pWifiSettings, String macddress, uint32_t revolutions, uint32_t bladesPerMinute);
/* sends data to the targetServer */
void sendContentToTarget(asyncHTTPrequest* pRequest, WiFiClient wifiClient, Settings * pSettings, WiFiSettings* pWifiSettings, String macAddress, uint32_t revolutions, uint32_t bladesPerMinute, bool withInfo);
/* returns response from the targetServer */
String getAsyncResponse(asyncHTTPrequest* pRequest);

#endif