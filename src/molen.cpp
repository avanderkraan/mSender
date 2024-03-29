#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal

#include "handlemDNS.h"
#include <WiFiUdp.h>

#include "updateOverHTTP.h"

#include "settings.h"
#include "handleWebServer.h"
#include "handleHTTPClient.h"
#include "WiFiSettings.h"

// WIFI URL: http://192.168.4.1/ or http://molen.local/
/////////////////////
// Pin Definitions //
/////////////////////

// D5 gives troubles when it is high at the start.

const uint8_t IR_RECEIVE_1 = D5;    // Digital pin to read an incoming signal
const uint8_t IR_RECEIVE_2 = D6;    // Digital pin to read an incoming signal

const uint8_t IR_SEND = D8;         // switch for IR send LED. 0 = off, 1 = on

const uint8_t BUTTON = D7;          // Digital pin to read button-push
const uint8_t PULSE_LED = D2;
const uint8_t ACCESSPOINT_LED = D3;
const uint8_t STATION_LED= D1;

// variables for reset to STA mode
unsigned long lastStartMillis = millis();
const uint32_t NO_STA_MAX_MILLIS = 300000;  // 300 000 millis leaves an interval of 5 minutes
const uint32_t NO_RESPONSE_MILLIS = 300000; // no response since the last 5 minutes? then try connect to WiFi
bool eepromStartModeAP = false;             // see setup, holds the startmode from eeprom

bool permissionToDetect = false;            // all sensors must have had a positive value 

uint32_t startPulse = millis();             // set the offset time for a passing a pulse
uint32_t pulsesPerMinute = 0;               // holds the value of pulses per minute
uint32_t revolutions = 0;                   // holds the value of revolutions of the first axis, calculated with ratio
uint32_t bladesPerMinute = 0;               // holds the value of ends per minute calculated with ratio
uint32_t pulseLedOnTime = millis();         // holds the last time a pulse was detected, used as flag for the pulse_led
Settings settings = Settings();
Settings* pSettings = &settings;

//////////////////////
// WiFi Definitions //
//////////////////////
WiFiSettings wifiSettings = WiFiSettings(pSettings);
WiFiSettings* pWifiSettings = &wifiSettings;

//////////////////////
// AsyncHTTPrequest //
//////////////////////
asyncHTTPrequest aRequest;
long lastSendMillis;

// lastResponseMillis keeps track if a response was received, otherwise try to connect to WiFi again
long lastResponseMillis;

// detectButtonFlag lets the program know that a network-toggle is going on
bool detectButtonFlag = false;

// detectUpdateFlag is True is an update from the server is requested
bool detectUpdateFlag = false;

// updateSucceeded is true is the update succeeded or if a restart is asked, so a restart can be done
bool updateSucceeded = false;

// detectInfoRequest is True if info is requested by the server
bool detectInfoRequest = false;

// Forward declaration
void setupWiFiAsAccessPoint();
void handleInfo();
void switchToAccessPoint();
void initServer();
void sendExample();

void IRAM_ATTR detectPulse();
void echoInterruptOn();
void echoInterruptOff();

void IRAM_ATTR detectButton();
void buttonInterruptOn();
void buttonInterruptOff();

void toggleWiFi();
/*
2^8 = 256
2^16 = 65536
2^32 = 4294967296
2^64 = 18446744073709551616
*/

ESP8266WebServer server(80);
WiFiClient wifiClient;

// start Settings and EEPROM stuff
void saveSettings() {
  pSettings->saveSettings();
  handleInfo();
}

void getSettings() {
  pSettings->getSettings();
  handleInfo();
}

void eraseSettings() {
  pSettings->eraseSettings();
  pSettings->getSettings();   // otherwise the previous values of Settings are used
  handleInfo();
}

void initSettings() {
  pSettings->initSettings();
  pSettings->getSettings();   // otherwise the previous values of Settings are used
  handleInfo();
}
// end Settings and EEPROM stuff

void setupWiFiAsAccessPoint(){
  echoInterruptOff();  // to prevent error with Delay
  digitalWrite(ACCESSPOINT_LED, HIGH);
  digitalWrite(STATION_LED, HIGH);

  WiFi.mode(WIFI_AP);

  String myssid = pWifiSettings->readAccessPointSSID();
  String mypass = pWifiSettings->readAccessPointPassword();


  if (myssid == "")
  {
    myssid = "ESP-" + WiFi.macAddress();
    pWifiSettings->setAccessPointSSID(myssid);
    pWifiSettings->saveAuthorizationAccessPoint();
  }

  IPAddress local_IP(192,168,4,1);
  IPAddress gateway(192,168,4,1);
  IPAddress subnet(255,255,255,0);


  Serial.println("Setting soft-AP ... ");
  // mypass needs minimum of 8 characters
  Serial.print("Setting soft-AP configuration: ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  Serial.print("Connecting to AP mode: ");
  Serial.println(WiFi.softAP(myssid,mypass,3,0) ? "Ready" : "Failed!");

  delay(500);
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
  Serial.println(WiFi.softAPmacAddress());

  digitalWrite(ACCESSPOINT_LED, HIGH);
  digitalWrite(STATION_LED, LOW);

  pSettings->beginAsAccessPoint(true);  // not saved in EEPROM
 
  echoInterruptOn();  // to prevent error with Delay

}

void setupWiFiAsStation () {
  bool networkConnected = false;
  echoInterruptOff();  // to prevent error with Delay

  digitalWrite(ACCESSPOINT_LED, HIGH);
  digitalWrite(STATION_LED, HIGH);

  String mynetworkssid = pWifiSettings->readNetworkSSID();
  if (mynetworkssid != "") {
    String mynetworkpass = pWifiSettings->readNetworkPassword();

    WiFi.mode(WIFI_STA);
    WiFi.begin(mynetworkssid, mynetworkpass); 

    Serial.print("Connecting to a WiFi Network");
    int toomuch = 30;  //gives 30 seconds to connect to a Wifi network
    while ((WiFi.status() != WL_CONNECTED) && (toomuch > 0))
    {
      delay(1000);
      Serial.print(".");
      toomuch -=1;
    }
    if (toomuch > 0) {
      Serial.println();

      Serial.print("Connected, IP address: ");
      Serial.println("local ip address");
      Serial.println(WiFi.localIP());
      Serial.println(WiFi.gatewayIP());
      Serial.println(WiFi.macAddress());
    
      echoInterruptOn();  // to prevent error with Delay
      networkConnected = true;
      pSettings->setLastNetworkIP(WiFi.localIP().toString());

      digitalWrite(ACCESSPOINT_LED, LOW);
      digitalWrite(STATION_LED, HIGH);
      pSettings->beginAsAccessPoint(false);  //not saved in EEPROM

      detectInfoRequest = true;  // info includes the correct ratio from the server
    }
  }
  if (networkConnected == false) {
    // no network found, fall back to access point
    Serial.println("no network SSID found/selected, fall back to AccessPoint mode");
    switchToAccessPoint();
  }
}

void resetWiFiManagerToFactoryDefaults () {
  echoInterruptOff();  // to prevent error with Delay

  // WiFi.disconnect(true);  // true argument should also erase ssid and password
  // https://www.pieterverhees.nl/sparklesagarbage/esp8266/130-difference-between-esp-reset-and-esp-restart
  Serial.println("try to disconnect works only when WiFi.begin() was successfully called");
  int toomuch = 2;
  while (toomuch > 0) {
    int getValue = WiFi.disconnect(true);
    if (getValue != 0) {
      toomuch = 0;
    }
    Serial.println(String(getValue));
    delay(3000);
    Serial.println("waited 3 seconds");
    toomuch -= 1;
  }

  echoInterruptOn();  // to prevent error with Delay
}

void switchToAccessPoint() {
  echoInterruptOff();  // to prevent error with Delay

  //pSettings->beginAsAccessPoint(!  pSettings->beginAsAccessPoint());  // toggle
  delay(pSettings->WAIT_PERIOD);

  server.close();
  resetWiFiManagerToFactoryDefaults();
  delay(pSettings->WAIT_PERIOD);

  setupWiFiAsAccessPoint();
  delay(pSettings->WAIT_PERIOD);

  initServer();

  // start domain name server check
  mDNSnotifyAPChange();
  // end domain name server check

  echoInterruptOn();  // to prevent error with Delay
}

void switchToNetwork() {
  echoInterruptOff();  // to prevent error with Delay

  delay(pSettings->WAIT_PERIOD);

  server.close();
  resetWiFiManagerToFactoryDefaults();
  delay(pSettings->WAIT_PERIOD);

  setupWiFiAsStation();
  delay(pSettings->WAIT_PERIOD);

  delay(pSettings->WAIT_PERIOD);
  initServer();

  // start domain name server check
  mDNSnotifyAPChange();
  // start domain name server check

  echoInterruptOn();  // to prevent error with Delay
}

void writeResult(WiFiClient wifiClient, String result) {
  wifiClient.print(result);
  wifiClient.flush();
}

void checkGlobalPulseInLoop() {
  // sets value to 0 after a period of time
  uint32_t elapsedTime;
  uint8_t stepDown = 8;
  if (millis() > startPulse) {  // check for overflow
    
    elapsedTime = millis() - startPulse;
    // fake a startPulse to gradually lower the speed, do not count pulses
    if (elapsedTime * pSettings->ratio > (pSettings->getSendPeriod() * 2)) {
      pulsesPerMinute = 0;
      if (bladesPerMinute > stepDown)
      {
        bladesPerMinute -= stepDown;
      }
      else{
        bladesPerMinute = 0;
      }
      startPulse = millis();   
    }   
  }
}

void setGlobalPulsesPerMinute() {
  /*
  start = millis();
  Returns the number of milliseconds since the Arduino board began
  running the current program. This number will overflow (go back to zero),
  after approximately 50 days.
  */
  // use previous value if an overflow of millis() occurs,
  // it does not have to be too precise
  uint32_t elapsedTime;
  if (millis() > startPulse) {  // check for overflow
    elapsedTime = millis() - startPulse;

    // measuremens shorter then the delay time are invalid (with an extra 50 ms to be sure)
    //if (elapsedTime > START_PERIOD) {
      if (elapsedTime > 0) {
        // get duration to get 1 pulse
        pulsesPerMinute = (uint32_t) round(60000 / elapsedTime);
      }
      else {
        pulsesPerMinute = 0;    // maybe slow movement, but rounded to 0
      }
   //}
    startPulse = millis();
  }
  else
  {
    startPulse = millis();  // reset after overflow, skip setting pulsesPeminute
  }
}

void delayInMillis(uint8_t ms)
{
  for (uint8_t i = 0; i <= ms; i++)
  {
    delayMicroseconds(250);   // delay in the loop could cause an exception (9) when using interrupts
    delayMicroseconds(250);   // delay in the loop could cause an exception (9) when using interrupts
    delayMicroseconds(250);   // delay in the loop could cause an exception (9) when using interrupts
    delayMicroseconds(250);   // delay in the loop could cause an exception (9) when using interrupts
  }
}

void IRAM_ATTR detectButton() {  // IRAM_ATTR is voor interrupts
  // this function is called after a change of pressed button  
  buttonInterruptOff();  // to prevent exception

  delayInMillis(10);      // prevent bounce
  
  if (digitalRead(BUTTON) == LOW)
  {
    detectButtonFlag = true;
    // only toggle between AP and STA by using the button, not saving in EEPROM
  }
  buttonInterruptOn();  // to prevent exception
}

void buttonInterruptOn() {
  attachInterrupt(digitalPinToInterrupt(BUTTON), detectButton, FALLING);
}

void buttonInterruptOff() {
  detachInterrupt(BUTTON);
}

void IRAM_ATTR detectPulse() {  // IRAM_ATTR is voor interrupts
  // this function is called after a change of every sensor-value
  // wait until both sensors are true, then permissionToDetect = true
  // if both sensors are false and permissionToDetect == true then it counts as a valid pulse
  // after a valid pulse the value of permissionToDetect is set to false to start over again
  echoInterruptOff();

  if ( (digitalRead(IR_RECEIVE_1) == true) && 
      (digitalRead(IR_RECEIVE_2) == true) &&
      (permissionToDetect == true) )
  {
    permissionToDetect = false;
    digitalWrite(PULSE_LED, HIGH);
    pulseLedOnTime = millis();
  }

  if ( (digitalRead(IR_RECEIVE_1) == false) && 
      (digitalRead(IR_RECEIVE_2) == false) && 
      (permissionToDetect == false) )
  {
    permissionToDetect = true;  // start over again

    pSettings->setCounter(pSettings->getCounter() + 1); // added 1 pulse
    setGlobalPulsesPerMinute();
    // calculate with ratio
    if (pSettings->blades < 1) {
      pSettings->blades = 1;
    }

    bladesPerMinute = round(pulsesPerMinute / pSettings->pulseFactor);

    // revolutions depend on the number of blades
    revolutions = floor(pSettings->getCounter() / (pSettings->pulseFactor * pSettings->blades));

    // the ratioArgument should be changed with a server given value
    // and therefore not be the same as the ratioFactoryArgumnent
    if (pSettings->getRatioArgument() == pSettings->getFactoryRatioArgument())
    { // NOT the correct ratio
      bladesPerMinute = 0;
      revolutions = 0;
    }
  }
  echoInterruptOn();
}

void echoInterruptOn() {
  // 0 = ir_light, 1 is no ir_light
  attachInterrupt(IR_RECEIVE_1, detectPulse, CHANGE);
  attachInterrupt(IR_RECEIVE_2, detectPulse, CHANGE);
}

void echoInterruptOff() {
  detachInterrupt(IR_RECEIVE_1);
  detachInterrupt(IR_RECEIVE_2);
}

void handleWiFi() {
  if (pSettings->getLanguage() == "NL")
  {
    wifi_nl(server, pSettings, pWifiSettings);
  }
  else
  {
    wifi(server, pSettings, pWifiSettings);
  }
}

void handleInfo() {
  if (pSettings->getLanguage() == "NL")
  {
    info_nl(server, pSettings, pWifiSettings);
  }
  else
  {
    info(server, pSettings, pWifiSettings);
  }

  Serial.print("readAccessPointSSID: ");
  Serial.println(pWifiSettings->readAccessPointSSID());
  Serial.print("readNetworkSSID: ");
  Serial.println(pWifiSettings->readNetworkSSID());

  Serial.print("Ratio: ");
  Serial.println(pSettings->getRatioArgument());

  Serial.print("Start as ");
  Serial.println(pSettings->beginAsAccessPoint() ? "Access Point" : "Network Station");

  Serial.print("Chip ID: ");
  Serial.println(ESP.getFlashChipId());
 
  Serial.print("Chip Real Size: ");
  Serial.println(ESP.getFlashChipRealSize());
 
  Serial.print("Chip Size: ");
  Serial.println(ESP.getFlashChipSize());
 
  Serial.print("Chip Speed: ");
  Serial.println(ESP.getFlashChipSpeed());
 
  Serial.print("Chip Mode: ");
  Serial.println(ESP.getFlashChipMode());

  Serial.print("firmware version: ");
  Serial.println(pSettings->getFirmwareVersion());

  Serial.print("device key: ");
  Serial.println(pSettings->getDeviceKey());

  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
}

String updateFirmware(String requestedVersion)
{
  echoInterruptOff();
  buttonInterruptOff();

  digitalWrite(STATION_LED, HIGH);
  digitalWrite(ACCESSPOINT_LED, HIGH);

  String serverUrl = pSettings->getTargetServer();
  uint16_t serverPort = pSettings->getTargetPort();
  String uploadScript = "/update/updateFirmware/?device=sender&version=" + requestedVersion;
  String version = pSettings->getFirmwareVersion();
  String result = updateOverHTTP(wifiClient, serverUrl, serverPort, uploadScript, version);

  if (result == UPDATEOVERHTTP_OK)
  {
    updateSucceeded = true;  // causes an ESP restart in the loop
  }
  else
  {
    // restore settings
    echoInterruptOn();
    buttonInterruptOn();
    if (WiFi.getMode() == WIFI_STA)
    {
      digitalWrite(STATION_LED, HIGH);
      digitalWrite(ACCESSPOINT_LED, LOW);    
    }
    else
    {
      digitalWrite(STATION_LED, LOW);    
      digitalWrite(ACCESSPOINT_LED, HIGH);    
    }
  }
  return result;
}

void handleVersion() {
  echoInterruptOff();    // otherwise the processor could be too busy handling interrupts

  uint8_t argumentCounter = 0;
  String result = "";
  String result_nl = "";

  if (server.method() == HTTP_POST)
  {
    argumentCounter = server.args();
    String name = "";
    for (uint8_t i=0; i< server.args(); i++){
      if (server.argName(i) == "name") {
        name = server.arg(i);
      }
    }
    // search name 
    if (name == "update")
    {
      if (WiFi.getMode() == WIFI_STA)
      {
        detectInfoRequest = true;
      }
      if (argumentCounter > 0)
      {
        result = updateFirmware("latest");
        if (result == UPDATEOVERHTTP_OK)
        {
          updateSucceeded = true;
        }
      }
    }
  }
  if (pSettings->getLanguage() == "NL")
  {
    if (result == UPDATEOVERHTTP_FAILED)
    {
      result_nl = "[update] Update mislukt";
    }
    if (result == UPDATEOVERHTTP_NO_UPDATE)
    {
      result_nl = "[update] Geen update aanwezig";
    }
    if (result == UPDATEOVERHTTP_NO_INTERNET)
    {
      result_nl = "[update] Geen connectie met de server aanwezig";
    }
    if (result == UPDATEOVERHTTP_OK)
    {
      result_nl = "[update] Update ok";
    }
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Connection", "keep-alive");
    server.sendHeader("Pragma", "no-cache");
    server.send(200, "text/html", result_nl);
  }
  else
  {
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Connection", "keep-alive");
    server.sendHeader("Pragma", "no-cache");
    server.send(200, "text/html", result);
  }
}

void handleRestart() {
  echoInterruptOff();    // otherwise the processor could be too busy handling interrupts

  uint8_t argumentCounter = 0;
  String result = "";
  String result_nl = "";

  if (server.method() == HTTP_POST)
  {
    argumentCounter = server.args();
    String name = "";
    for (uint8_t i=0; i< server.args(); i++){
      if (server.argName(i) == "name") {
        name = server.arg(i);
      }
    }
    // search name 
    if (name == "restart")
    {
      if (argumentCounter > 0)
      {
        updateSucceeded = true;
        result = "Restart completed";
        result_nl = "Restart compleet";
      }
    }
  }
  if (pSettings->getLanguage() == "NL")
  {
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Connection", "keep-alive");
    server.sendHeader("Pragma", "no-cache");
    server.send(200, "text/html", result_nl);
  }
  else
  {
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Connection", "keep-alive");
    server.sendHeader("Pragma", "no-cache");
    server.send(200, "text/html", result);
  }
}

void handleConnect() {
  uint8_t argumentCounter = 0;

  argumentCounter = server.args();
  if (argumentCounter < 4) {  // prevent to many arguments
    String targetServer = pSettings->getFactoryTargetServer();
    String targetPort = String(pSettings->getFactoryTargetPort());
    String targetPath = pSettings->getFactoryTargetPath();
    for (uint8_t i=0; i< server.args(); i++){
      if (server.argName(i) == "server") {
        targetServer = server.arg(i);
      }
      if (server.argName(i) == "port") {
        targetPort = server.arg(i);
      }
      if (server.argName(i) == "path") {
        targetPath = server.arg(i);
      }
    }

    pSettings->setTargetServer(targetServer);
    pSettings->setTargetPort(targetPort);
    pSettings->setTargetPath(targetPath);
    pSettings->saveTargetServerStuff();
  }
  handleInfo();
}

/*
void handleHelp() {
  if (pSettings->getLanguage() == "NL")
  {
    help_nl(server, pSettings);
  }
  else
  {
    help(server, pSettings);
  }
}
*/

void handleLanguage() {
  uint8_t argumentCounter = 0;
  String result = "";
  String result_nl = "";

  if (server.method() == HTTP_POST)
  {
    argumentCounter = server.args();  // if argumentCounter > 0 then save
    String name = "";
    String language = "";
    for (uint8_t i=0; i< server.args(); i++){
      if (server.argName(i) == "name") {
        name = server.arg(i);
      }
      if (server.argName(i) == "language") {
        language = server.arg(i);
      }
    }
    // search name 
    if (name == "help")
    {
      if (argumentCounter > 0)
      {
        pSettings->setLanguage(language);
      }
    }
  }
  if (pSettings->getLanguage() == "NL")
  {
    server.send(200, "text/plain", result_nl);
  }
  else
  {
    server.send(200, "text/plain", result);
  }
}

void handleNetworkSSID() {
  // creates a list of {ssid, including input field , dBm}
  // to prevent an error, the single quote will be replace here with a question mark
  //                      and restored within the handleWebServer  
  String result = "";
  int numberOfNetworks = WiFi.scanNetworks();
  for(int i =0; i<numberOfNetworks; i++){ 
    String foundSSID = WiFi.SSID(i);
    foundSSID.replace("'", "?");
    if (i > 0) {
      result += ",";
    }
    result += "{ssid:";
    result += "'<span><input type=\"radio\" name=\"networkSSID\" onclick=\"selectNetworkSSID(this)\" value=\"";
    result += foundSSID;
    result += "\">";
    result += foundSSID;
    result += "</span>";
    result += "',";
    result += "dBm:'";
    result += WiFi.RSSI(i);
    result += "'}";
  }
  server.sendHeader("Cache-Control", "no-cache");
  server.sendHeader("Connection", "keep-alive");
  server.sendHeader("Pragma", "no-cache");
  server.send(200, "text/html", result);
}

void handleWifiConnect() {
  uint8_t argumentCounter = 0;
  String result = "";
  String result_nl = "";

  if (server.method() == HTTP_POST)
  {
    argumentCounter = server.args();  // if argumentCounter > 0 then save
    String name = "";
    String ssid = "";
    String password = "";
    String target = "";               // for action Erase
    for (uint8_t i=0; i< server.args(); i++){
      if (server.argName(i) == "name") {
        name = server.arg(i);
      }
      if (server.argName(i) == "ssid") {
        ssid = server.arg(i);
      }
      if (server.argName(i) == "password") {
        password = server.arg(i);
      }
      if (server.argName(i) == "target") {
        target = server.arg(i);
      }
    }
    // zoek name (is ap of network en dan de ssid en password)
    if (name == "ap")
    {
      pWifiSettings->setAccessPointSSID(ssid);
      pWifiSettings->setAccessPointPassword(password);
      if (argumentCounter > 0) {
        pWifiSettings->saveAuthorizationAccessPoint();
        result += "Access Point data has been saved\n";
        result_nl += "Access Point gegevens zijn opgeslagen\n";
      }
    }
    if (name == "network")
    {
      pWifiSettings->setNetworkSSID(ssid);
      pWifiSettings->setNetworkPassword(password);

      if (argumentCounter > 0) {
        pWifiSettings->saveAuthorizationNetwork();
        result += "Network connection data has been saved\n";
        result_nl += "Netwerk verbindingsgegevens zijn opgeslagen\n";
      }
    }
    if (name == "erase")
    {
      if (argumentCounter > 0) {
        if (target == "eraseAPData")
        {
          pWifiSettings->eraseAccessPointSettings();
          result += "Access Point data has been erased\n";
          result_nl += "Access Point gegevens zijn gewist\n";
        }
        if (target == "eraseNetworkData")
        {
          pWifiSettings->eraseNetworkSettings();
          result += "Network connection data has been erased\n";
          result_nl += "Netwerk verbindingsgegevens zijn gewist\n";
        }
        if (target == "eraseWiFiData")
        {
          pWifiSettings->eraseWiFiSettings();
          result += "Access Point data and Network connection data has been erased\n";
          result_nl += "Access Point gegevens en Netwerk verbindingsgegevens zijn gewist\n";
        }
      }
    }
  }
  if (pSettings->getLanguage() == "NL")
  {
    server.send(200, "text/plain", result_nl);
  }
  else
  {
    server.send(200, "text/plain", result);
  }
  Serial.println(result);
}

String getValueFromJSON(String key, String responseData)
{
  int16_t keyIndex = responseData.indexOf(key);
  if (keyIndex > -1)
  {
    int16_t start = responseData.indexOf(":", keyIndex);
    int16_t end = responseData.indexOf(",", start);
    if (end == -1) {
      end = responseData.indexOf("}", start);
    }
    String value = responseData.substring(start + 1, end);
    value.trim();
    value.replace("\"", "");
    return value;
  }
  return "";
}

void processServerData(String responseData)
{
  String proposedUUID = getValueFromJSON("pKey", responseData);
  if ((proposedUUID != "") && (pSettings->getDeviceKey() != proposedUUID))
  {
    pSettings->setDeviceKey(proposedUUID);
    pSettings->saveDeviceKey(); // save to EEPROM
  }

  String requestInterval = getValueFromJSON("t", responseData);
  if ((requestInterval != ""))
  {
    pSettings->setRequestInterval(requestInterval);
  }

  String proposedRatio = getValueFromJSON("pR", responseData);
  // check ratio
  bool ratioOK = true;
  String charOK = "0123456789.-";
  if (proposedRatio.indexOf("--") > -1)
  {
    ratioOK = false;
  }
  if (proposedRatio.indexOf(".-") > -1)
  {
    ratioOK = false;
  }
  if (proposedRatio.indexOf("-.") > -1)
  {
    ratioOK = false;
  }
  if (proposedRatio.indexOf("..") > -1)
  {
    ratioOK = false;
  }
  for (uint8_t i = 0; i < proposedRatio.length(); i++)
  {
    if (charOK.indexOf(proposedRatio.charAt(i)) == -1)
    {
      ratioOK = false;
    }
  }

  if ((ratioOK) && (proposedRatio != "") && (pSettings->getRatioArgument() != proposedRatio))
  {
    pSettings->setRatioArgument(proposedRatio);
    pSettings->saveRatioArgument(); // save to EEPROM
  }

  String pushFirmwareVersion = getValueFromJSON("pFv", responseData);
  if (pushFirmwareVersion != "")
  {
    detectUpdateFlag = true;
  }

  String requestForInfo = getValueFromJSON("i", responseData);
  if (requestForInfo != "")
  {
    detectInfoRequest = true;
  }
}

void toggleWiFi()
{
  // only toggle by using the button, not saving in EEPROM
  pSettings->beginAsAccessPoint(!  pSettings->beginAsAccessPoint());  // toggle
  if (pSettings->beginAsAccessPoint() == true)
  {
    //switchToAccessPoint();
    setupWiFiAsAccessPoint();          // local network as access point
  }
  else
  {
    //switchToNetwork();
    setupWiFiAsStation();   // part of local network as station
  }
}

void initHardware()
{
  Serial.begin(115200);
  pinMode(IR_SEND, OUTPUT);
  pinMode(IR_RECEIVE_1, INPUT);  // default LOW
  pinMode(IR_RECEIVE_2, INPUT);  // default LOW

  pinMode(PULSE_LED, OUTPUT);
  pinMode(ACCESSPOINT_LED, OUTPUT);
  pinMode(STATION_LED, OUTPUT);

  pinMode(BUTTON, INPUT_PULLUP); // default HIGH
}

void initServer()
{
  server.close();
  // start webserver

  //server.on("/help/", handleHelp);

  // handles notFound
  server.onNotFound(handleWiFi);

  // interactive pages
  server.on("/wifi/", handleWiFi);
  server.on("/info/", handleInfo);
  // handles input from interactive pages
  server.on("/networkssid/", handleNetworkSSID);
  server.on("/wifiConnect/", handleWifiConnect);
  server.on("/language/", handleLanguage);
  server.on("/update/", handleVersion);
  server.on("/restart/", handleRestart);

  // url-commands, not used in normal circumstances
  server.on("/ap/", switchToAccessPoint);
  server.on("/network/", switchToNetwork);
  server.on("/eraseSettings/", eraseSettings);
  server.on("/initSettings/", initSettings);
  server.on("/getSettings/", getSettings);
  server.on("/saveSettings/", saveSettings);
  server.on("/reset/", resetWiFiManagerToFactoryDefaults);

  // for selecting a target server when developing
  // arguments: server, port, path, default are the factorySettings
  // /connect/ without arguments connects to the production server, 
  //           port and path
  server.on("/connect/", handleConnect);

  server.begin();
  Serial.println("HTTP server started");
}

void setup()
{
  /* It seems to help preventing ESPerror messages with mode(3,6) when
  using a delay */
  initHardware();

  digitalWrite(IR_RECEIVE_1, LOW);
  digitalWrite(IR_RECEIVE_2, LOW);

  delay(pSettings->WAIT_PERIOD);

  // see https://forum.arduino.cc/index.php?topic=121654.0 voor circuit brownout
  delay(pSettings->WAIT_PERIOD);
  // use EITHER setupWiFiAsAccessPoint OR setupWiFiAsStation
  
  // get saved setting from EEPROM
  eepromStartModeAP = pSettings->beginAsAccessPoint();

  if (pSettings->beginAsAccessPoint())
  {
    setupWiFiAsAccessPoint();        // local network as access point
  }
  else
  {
    setupWiFiAsStation();   // part of local network as station
  }

  delay(pSettings->WAIT_PERIOD);
  startmDNS();

  delay(pSettings->WAIT_PERIOD);

  initServer();

  // for asyncrequest
  lastSendMillis = millis();
  lastResponseMillis = millis();

  delay(pSettings->WAIT_PERIOD);

  echoInterruptOn();
  
  buttonInterruptOn();
  digitalWrite(IR_SEND, HIGH);   // always on
}

void loop()
{
  // update should be run on every loop
  MDNS.update();

  if (detectUpdateFlag == true)
  {
    String result = updateFirmware("latest");
    detectUpdateFlag = false;
  }

  if (updateSucceeded == true)
  {
    ESP.restart();
  }

  if (detectButtonFlag == true)
  {
    toggleWiFi();   // only toggle between AP and STA by using the button, not saving in EEPROM
    detectButtonFlag = false;
  }

  // For ESP8266WebServer
  server.handleClient();

  // For handleHTTPClient
  if (WiFi.getMode() == WIFI_STA)
  {
    /* send data to target server using ESP8266HTTPClient */
    if (millis() - lastSendMillis > pSettings->getSendPeriod())
    {
      if ((aRequest.readyState() == 0) || (aRequest.readyState() == 4)) {
        sendContentToTarget(&aRequest, wifiClient, pSettings, pWifiSettings, String(WiFi.macAddress()), revolutions, bladesPerMinute, detectInfoRequest);
        detectInfoRequest = false;    // reset value so no info will be sent again
      }
      lastSendMillis = millis();
    }
    String response = getAsyncResponse(&aRequest);
    if (response != "") 
    {
      lastResponseMillis = millis();
      processServerData(response);
    }
    if (millis() - lastResponseMillis > NO_RESPONSE_MILLIS)
    {
      lastResponseMillis = millis();
      setupWiFiAsStation();
    }
  }

  // For automatic Reset after loosing WiFi connection in STA mode
  if ((WiFi.getMode() == WIFI_AP) && (eepromStartModeAP == false))
  {
    //echoInterruptOff();
    if (millis() > lastStartMillis + NO_STA_MAX_MILLIS)
    {
      lastStartMillis = millis();
      setupWiFiAsStation();   // try to start WiFi again
    }
    //echoInterruptOn();
  }
  
  checkGlobalPulseInLoop();

  if (millis() > pulseLedOnTime + 1)
  {
    digitalWrite(PULSE_LED, LOW);
  }
  digitalWrite(PULSE_LED, LOW);
}
