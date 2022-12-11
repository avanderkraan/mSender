
#include "settings.h"

String Settings::getFirmwareVersion()
{
  return String(this->major) + "." + String(this->minor) + "." + String(this->patch);
}

String Settings::getDeviceKey()
{
  return this->deviceKey;
}

void Settings::setDeviceKey(String myDeviceKey)
{
  this->deviceKey = myDeviceKey;
}

uint8_t Settings::getMAX_RATIO_ARGUMENT()
{
  return this->MAX_RATIO_ARGUMENT;
}

uint8_t Settings::getMAX_AXES()
{
  return this->MAX_AXES;
}
  
uint8_t Settings::getMAX_WHEELS()
{
  return this->MAX_WHEELS;
}

char Settings::getAXES_DELIMITER()
{
  return this->AXES_DELIMITER;
}

char Settings::getWHEEL_DELIMITER()
{
  return this->WHEEL_DELIMITER;
}

uint16_t Settings::getSendPeriod()
{
  return this->sendPeriod;
}

void Settings::setRequestInterval(String requestInterval)
{
  uint16_t result = this->atoi16_t(requestInterval);
  if (result == 0)
  {
    result = 5;  // set to 5 seconds
  }
  this->sendPeriod = result * 1000;  // milliseconds
}

String Settings::getTargetServer()
{
  return this->targetServer;
}

uint16_t Settings::getTargetPort()
{
  return this->targetPort;
}

String Settings::getTargetPath()
{
  return this->targetPath;
}

String Settings::getRatioArgument()
{
  return this->ratioArgument;
}

void Settings::setRatioArgument(String ratio)
{
  this->ratioArgument = ratio;
}

String Settings::getFactoryRatioArgument()
{
  return this->factoryRatioArgument;
}

uint8_t Settings::atoi8_t(String s) 
{
    uint8_t i, n;
    n = 0;
    for (i = 0; s[i] >= '0' && s[i] <= '9'; i++)
        n = 10 * n +(s[i] - '0');
    return n;
}

uint16_t Settings::atoi16_t(String s) 
{
    uint8_t i;
    uint16_t n;
    n = 0;
    for (i = 0; s[i] >= '0' && s[i] <= '9'; i++)
        n = 10 * n +(s[i] - '0');
    return n;
}

uint32_t Settings::atoi32_t(String s) 
{
    uint8_t i;
    uint32_t n;
    n = 0;
    for (i = 0; s[i] >= '0' && s[i] <= '9'; i++)
        n = 10 * n +(s[i] - '0');
    return n;
}

String Settings::getFirstElement(String line, char delimiter)
{
  String result = line.substring(0, line.indexOf(delimiter));
  return result;
}

String Settings::getLastElement(String line, char delimiter)
{
  String result = "";
  if (line.lastIndexOf(delimiter) > -1) {
    result = line.substring(line.lastIndexOf(delimiter) + 1, line.length());
  }
  return result;
}

void Settings::calculateRatio(String ratioArgument)
{
  // only for calculating the ratio between axes per revolution of the first axis
  float myRatio = 1.0;
  uint8_t count = 0;
  String wheels[this->MAX_AXES];  // number of '-' MUST not exceed MAX_AXES 

  bool stop = false;
  count = 0;
  String wheel = "";
  for (uint8_t i = 0; i <= ratioArgument.length(); i++) {
    if ((ratioArgument[i] == this->AXES_DELIMITER) || (i == ratioArgument.length())) {
      stop = true;
    }
    else {
      wheel = wheel + ratioArgument[i];
    }
    if (stop) {
      wheels[count] = wheel;
      count +=1;
      wheel = "";
      stop = false;
    }
  }
  uint8_t bladePosition = 0;
  uint8_t wheelPosition = 1;
 
  this->blades = this->atoi8_t(wheels[bladePosition]);
  for (uint8_t i = wheelPosition; i < count; i++)
  {
    uint8_t firstWheel = this->atoi8_t(this->getFirstElement(wheels[i], this->WHEEL_DELIMITER));
    uint8_t lastWheel = this->atoi8_t(this->getLastElement(wheels[i], this->WHEEL_DELIMITER));
    if (lastWheel > 0)      // then there are axes involved
    {
      myRatio *= (firstWheel * 1.0 / lastWheel * 1.0) * 1.0;
    }
  }
  this->ratio = myRatio;                   // used for revolutions, not anymore, see pulseFactor
}

void Settings::calculatePulseFactor(String ratioArgument)
{
  // for calculating the pulse factor
  // how many pulses are neccesary to get 1 revolution of the first axis
  uint8_t count = 0;
  String axes[this->MAX_WHEELS];   // split on '-' 
  bool stop = false;
  count = 0;
  String axis = "";
  float myPulseFactor = 1.0;        // factor that goes from pulses to 1st axis
  for (uint8_t i = 0; i <= ratioArgument.length(); i++) {
    if ((ratioArgument[i] == this->WHEEL_DELIMITER) || (i == ratioArgument.length())) {
      stop = true;
    }
    else {
      axis = axis + ratioArgument[i];
    }
    if (stop) {
      axes[count] = axis;
      count +=1;
      axis = "";
      stop = false;
    }
  }
    for (uint8_t i = 0; i < count; i++)
  {
    uint8_t firstWheel = this->atoi8_t(this->getFirstElement(axes[i], this->AXES_DELIMITER));
    uint8_t lastWheel = this->atoi8_t(this->getLastElement(axes[i], this->AXES_DELIMITER));
    if (lastWheel > 0)
    {
      // if no second value then no new calculation. the previous is then ok
      myPulseFactor *= (lastWheel * 1.0 / firstWheel * 1.0) * 1.0;
    }
  }
  this->pulseFactor = myPulseFactor;      // used for revolutions in molen.cpp
}

uint16_t Settings::setupEEPROM()
{
  // It seems to help preventing ESPerror messages with mode(3,6) when using a delay 
  delay(this->WAIT_PERIOD);
  if (!this->isInitialized())
  {
    this->eraseEEPROM();  // clear complete EEPROM area
    this->initNumber = this->INITCHECK;
    return this->saveSettings();
  }
  delay(this->WAIT_PERIOD);
  return this->getSettings();
}

uint16_t Settings::setupUpdatedFirmware()
{
  uint16_t address = this->address;
  uint16_t firstAddress = this->address;
  if (this->isUpdated())
  { 
    delay(this->WAIT_PERIOD);

    EEPROM.begin(this->MAX_EEPROM_SIZE);
    //EEPROM.put(address, this->initNumber);
    address += sizeof(this->initNumber);
    EEPROM.put(address, this->major);
    address += sizeof(this->major);
    EEPROM.put(address, this->minor);
    address += sizeof(this->minor);
    EEPROM.put(address, this->patch);
    address += sizeof(this->patch);

    EEPROM.commit();    // with success it will return true
    EEPROM.end();  // release RAM copy of EEPROM content
    
    delay(this->WAIT_PERIOD);
  }
  return firstAddress - address;
}

uint16_t Settings::saveSettings()
{
  // It seems to help preventing ESPerror messages with mode(3,6) when using a delay 
  delay(this->WAIT_PERIOD);

  uint16_t firstAddress = this->address;
  uint16_t address = this->address;

  EEPROM.begin(this->MAX_EEPROM_SIZE);

  EEPROM.put(address, this->initNumber);
  address += sizeof(this->initNumber);
  EEPROM.put(address, this->major);
  address += sizeof(this->major);
  EEPROM.put(address, this->minor);
  address += sizeof(this->minor);
  EEPROM.put(address, this->patch);
  address += sizeof(this->patch);

  char myLanguage[3];  // one more for the null character
  strcpy(myLanguage, this->language.c_str());
  EEPROM.put(address, myLanguage);
  address += 3;

  bool _startAsAccessPoint = false;  // always try to start as Network Station is default
  //EEPROM.put(address, this->startAsAccessPoint);
  EEPROM.put(address, _startAsAccessPoint);
  address += sizeof(this->startAsAccessPoint);

  char myTargetServer[33];  // one more for the null character
  strcpy(myTargetServer, this->targetServer.c_str());
  EEPROM.put(address, myTargetServer);
  address += 33;

  EEPROM.put(address, this->targetPort);
  address += sizeof(this->targetPort);
  
  char myTargetPath[17];  // one more for the null character
  strcpy(myTargetPath, this->targetPath.c_str());
  EEPROM.put(address, myTargetPath);
  address += 17;


  char myRatioArgument[65];  // one more for the null character
  strcpy(myRatioArgument, this->ratioArgument.c_str());
  EEPROM.put(address, myRatioArgument);
  address += 65;

  char myDeviceKey[37];  // one more for the null character
  strcpy(myDeviceKey, this->deviceKey.c_str());
  EEPROM.put(address, myDeviceKey);
  address += 37;

  EEPROM.commit();    // with success it will return true
  EEPROM.end();       // release RAM copy of EEPROM content

  delay(this->WAIT_PERIOD);

  this->calculateRatio(this->getRatioArgument());
  this->calculatePulseFactor(this->getRatioArgument());

  return address - firstAddress;
}

bool Settings::isUpdated() {
  uint16_t address = this->address;
  delay(this->WAIT_PERIOD);
  
  EEPROM.begin(this->MAX_EEPROM_SIZE);
  uint8_t currentMajor = this->major;
  uint8_t currentMinor = this->minor;
  uint16_t currentPatch = this->patch;
  //EEPROM.get(address, this->initNumber);
  address += sizeof(this->initNumber);
  EEPROM.get(address, currentMajor);
  address += sizeof(this->major);
  EEPROM.get(address, currentMinor);
  address += sizeof(this->minor);
  EEPROM.get(address, currentPatch);
  address += sizeof(this->patch);
  EEPROM.end();  // release RAM copy of EEPROM content

  delay(this->WAIT_PERIOD);
  bool result = this->major != currentMajor &&
                this->minor != currentMinor &&
                this->patch != currentPatch;

  return result;
}

bool Settings::isInitialized() {
  delay(this->WAIT_PERIOD);
  
  EEPROM.begin(sizeof(this->initNumber));
  this->initNumber = EEPROM.read(this->address);
  EEPROM.end();  // release RAM copy of EEPROM content

  delay(this->WAIT_PERIOD);
 
  return (this->initNumber == this->INITCHECK);
}

bool Settings::eraseEEPROM() {
  delay(this->WAIT_PERIOD);

  EEPROM.begin(this->MAX_EEPROM_SIZE);
  // replace values in EEPROM with 0xff
  for (uint16_t i = 0; i < this->MAX_EEPROM_SIZE; i++) {
    EEPROM.write(this->address + i,0xff);
  }
  bool result = EEPROM.commit();    // with success it will return true
  EEPROM.end();  // release RAM copy of EEPROM content

  delay(this->WAIT_PERIOD);

  this->setRatioArgument(this->getFactoryRatioArgument());
  this->calculateRatio(this->getFactoryRatioArgument());
  this->calculatePulseFactor(this->getFactoryRatioArgument());

  return result;
}

bool Settings::eraseSettings() {
  delay(this->WAIT_PERIOD);

  EEPROM.begin(this->storageSize);
  // replace values in EEPROM with 0xff
  for (uint16_t i = 0; i < this->storageSize; i++) {
    EEPROM.write(this->address + i,0xff);
  }
  bool result = EEPROM.commit();    // with success it will return true
  EEPROM.end();  // release RAM copy of EEPROM content

  delay(this->WAIT_PERIOD);

  this->setRatioArgument(this->getFactoryRatioArgument());
  this->calculateRatio(this->getFactoryRatioArgument());
  this->calculatePulseFactor(this->getFactoryRatioArgument());

  return result;
}


uint16_t Settings::initSettings()
{
  // It seems to help preventing ESPerror messages with mode(3,6) when using a delay 
  delay(this->WAIT_PERIOD);

  uint16_t firstAddress = this->address;
  uint16_t address = this->address;

  EEPROM.begin(this->MAX_EEPROM_SIZE);

  EEPROM.put(address, this->factoryInitNumber);
  address += sizeof(this->factoryInitNumber);
  EEPROM.put(address, this->major);
  address += sizeof(this->major);
  EEPROM.put(address, this->minor);
  address += sizeof(this->minor);
  EEPROM.put(address, this->patch);
  address += sizeof(this->patch);
  char myFactoryLanguage[3];  // one more for the null character
  strcpy(myFactoryLanguage, this->factoryLanguage.c_str());
  EEPROM.put(address, myFactoryLanguage);
  address += 3;

  EEPROM.put(address, this->factoryStartAsAccessPoint);
  address += sizeof(this->factoryStartAsAccessPoint);

  char myFactoryTargetServer[33];  // one more for the null character
  strcpy(myFactoryTargetServer, this->factoryTargetServer.c_str());
  EEPROM.put(address, myFactoryTargetServer);
  address += 33;

  EEPROM.put(address, this->factoryTargetPort);
  address += sizeof(this->factoryTargetPort);
  
  char myFactoryTargetPath[17];  // one more for the null character
  strcpy(myFactoryTargetPath, this->factoryTargetPath.c_str());
  EEPROM.put(address, myFactoryTargetPath);
  address += 17;

  //uint8_t myMaxRatioArgument = this->MAX_RATIO_ARGUMENT;
  char myRatioArgument[65];  // one more for the null character
  strcpy(myRatioArgument, this->factoryRatioArgument.c_str());
  EEPROM.put(address, myRatioArgument);
  address += 65;

  //uint8_t myMaxDeviceKey = sizeof(this->deviceKey);
  char myDeviceKey[37];  // one more for the null character
  strcpy(myDeviceKey, this->factoryDeviceKey.c_str());
  EEPROM.put(address, myDeviceKey);
  address += 37;

  EEPROM.commit();    // with success it will return true
  EEPROM.end();       // release RAM copy of EEPROM content
  delay(this->WAIT_PERIOD);

  this->setRatioArgument(this->getFactoryRatioArgument());
  this->calculateRatio(this->getFactoryRatioArgument());
  this->calculatePulseFactor(this->getFactoryRatioArgument());

  return address - firstAddress;
}

uint16_t Settings::getSettings()
{
  // It seems to help preventing ESPerror messages with mode(3,6) when using a delay 
  delay(this->WAIT_PERIOD);

  uint16_t firstAddress = this->address;
  uint16_t address = this->address;

  EEPROM.begin(this->MAX_EEPROM_SIZE);
  
  EEPROM.get(address, this->initNumber);
  address += sizeof(this->initNumber);

  // major, minor, batch is done at setupFirmware
  address += sizeof(this->major);
  address += sizeof(this->minor);
  address += sizeof(this->patch);

  char myLanguage[3];  // one more for the null character
  EEPROM.get(address, myLanguage);
  this->language = String(myLanguage);
  address += 3;

  EEPROM.get(address, this->startAsAccessPoint);
  address += sizeof(this->startAsAccessPoint);

  char myTargetServer[33];  // one more for the null character
  EEPROM.get(address, myTargetServer);
  this->targetServer = String(myTargetServer);
  address += 33;

  EEPROM.get(address, this->targetPort);
  address += sizeof(this->targetPort);
  
  char myTargetPath[17];  // one more for the null character
  EEPROM.get(address, myTargetPath);
  this->targetPath = String(myTargetPath);
  address += 17;

  //uint8_t myMaxRatioArgument = this->MAX_RATIO_ARGUMENT;
  char myRatioArgument[65];
  EEPROM.get(address, myRatioArgument);
  this->ratioArgument = String(myRatioArgument);
  address += 65;

  //uint8_t myMaxDeviceKey = sizeof(this->deviceKey);
  char mydeviceKey[37];
  EEPROM.get(address, mydeviceKey);
  this->deviceKey = String(mydeviceKey);
  address += 37;

  EEPROM.end();  // release RAM copy of EEPROM content
  delay(this->WAIT_PERIOD);

  this->calculateRatio(this->getRatioArgument());
  this->calculatePulseFactor(this->getRatioArgument());

  return address - firstAddress;
}

uint16_t Settings::saveTargetServerStuff()
{
  // The function EEPROM.put() uses EEPROM.update() to perform the write, so does not rewrites the value if it didn't change.
  // It seems to help preventing ESPerror messages with mode(3,6) when using a delay 
  delay(this->WAIT_PERIOD);

  uint16_t firstAddress = this->address;
  uint16_t address = this->address;

  EEPROM.begin(this->MAX_EEPROM_SIZE);

  address += sizeof(this->initNumber);
  address += sizeof(this->major);
  address += sizeof(this->minor);
  address += sizeof(this->patch);
  address += 3;  // language
  address += sizeof(this->startAsAccessPoint);

  char myTargetServer[33];  // one more for the null character
  strcpy(myTargetServer, this->targetServer.c_str());
  EEPROM.put(address, myTargetServer);
  address += 33;

  EEPROM.put(address, this->targetPort);
  address += sizeof(this->targetPort);

  char myTargetPath[17];  // one more for the null character
  strcpy(myTargetPath, this->targetPath.c_str());
  EEPROM.put(address, myTargetPath);
  address += 17;

  EEPROM.commit();    // with success it will return true
  EEPROM.end();       // release RAM copy of EEPROM content
  
  delay(this->WAIT_PERIOD);

  return address - firstAddress;
}

uint16_t Settings::saveRatioArgument()
{
  // The function EEPROM.put() uses EEPROM.update() to perform the write, so does not rewrites the value if it didn't change.
  // It seems to help preventing ESPerror messages with mode(3,6) when using a delay 
  delay(this->WAIT_PERIOD);

  uint16_t firstAddress = this->address;
  uint16_t address = this->address;
 
  EEPROM.begin(this->MAX_EEPROM_SIZE);

  address += sizeof(this->initNumber);
  address += sizeof(this->major);
  address += sizeof(this->minor);
  address += sizeof(this->patch);
  address += 3;   // language
  address += sizeof(this->startAsAccessPoint);

  address += 33;  // targetServer
  address += sizeof(this->targetPort);
  address += 17;  // targetPath

  char myRatioArgument[65];  // one more for the null character
  strcpy(myRatioArgument, this->ratioArgument.c_str());
  EEPROM.put(address, myRatioArgument);

  EEPROM.commit();    // with success it will return true
  EEPROM.end();       // release RAM copy of EEPROM content
  
  delay(this->WAIT_PERIOD);

  this->calculateRatio(this->getRatioArgument());
  this->calculatePulseFactor(this->getRatioArgument());

  return address - firstAddress;
}

uint16_t Settings::saveDeviceKey()
{
  // The function EEPROM.put() uses EEPROM.update() to perform the write, so does not rewrites the value if it didn't change.
  // It seems to help preventing ESPerror messages with mode(3,6) when using a delay 
  delay(this->WAIT_PERIOD);

  uint16_t firstAddress = this->address;
  uint16_t address = this->address;
 
  EEPROM.begin(this->MAX_EEPROM_SIZE);

  address += sizeof(this->initNumber);
  address += sizeof(this->major);
  address += sizeof(this->minor);
  address += sizeof(this->patch);
  address += 3;   // language
  address += sizeof(this->startAsAccessPoint);

  address += 33;  // targetServer
  address += sizeof(this->targetPort);
  address += 17;  // targetPath

  address += 65;  // ratioArgument

  char myDeviceKey[37];  // one more for the null character
  strcpy(myDeviceKey, this->deviceKey.c_str());
  EEPROM.put(address, myDeviceKey);

  EEPROM.commit();    // with success it will return true
  EEPROM.end();       // release RAM copy of EEPROM content
  
  delay(this->WAIT_PERIOD);

  return address - firstAddress;
}

/*
uint16_t Settings::getOffsetAddress()
{
  return this->addressOffset;
}
*/

uint16_t Settings::getWiFiDataAddress()
{
  return this->wifiDataAddress;
}

/*
bool Settings::setOffsetAddress(uint16_t deltaAddress)
{
  if (this->getOffsetAddress() + deltaAddress > this->MAX_EEPROM_SIZE)
  {
    return false;
  }
  this->addressOffset += deltaAddress;
  return true;
}
*/

void Settings::setCounter(uint32_t counter)
{
  this->rawCounter = counter;
}

void Settings::setCounter(String counter)
{
  this->rawCounter = this->atoi32_t(counter);
}

uint32_t Settings::getCounter()
{
  return this->rawCounter;
}

bool Settings::beginAsAccessPoint()
{
  return this->startAsAccessPoint;
}

void Settings::beginAsAccessPoint(bool myBeginAsAccessPointValue)
{
  this->startAsAccessPoint = myBeginAsAccessPointValue;
}

String Settings::getFactoryStartModeWiFi()
{
  return this->factoryStartAsAccessPoint ? "ap" : "network";
}

String Settings::getFactoryTargetServer()
{
  return this->factoryTargetServer;
}

void Settings::setTargetServer(String targetServer)
{
  this->targetServer = targetServer;
}

uint16_t Settings::getFactoryTargetPort()
{
  return this->factoryTargetPort;
}

void Settings::setTargetPort(String targetPort)
{
  this->targetPort = this->atoi16_t(targetPort);
}

String Settings::getFactoryTargetPath()
{
  return this->factoryTargetPath;
}

void Settings::setTargetPath(String targetPath)
{
  this->targetPath = targetPath;
}

void Settings::setLanguage(String language)
{
  this->language = language;

   // It seems to help preventing ESPerror messages with mode(3,6) when using a delay 
  delay(this->WAIT_PERIOD);

  uint16_t address = this->address;

  EEPROM.begin(this->MAX_EEPROM_SIZE);
  address += sizeof(this->initNumber);
  address += sizeof(this->major);
  address += sizeof(this->minor);
  address += sizeof(this->patch);

  char myLanguage[3];  // one more for the null character
  strcpy(myLanguage, this->language.c_str());
  EEPROM.put(address, myLanguage);
  address += 3;

  EEPROM.commit();    // with success it will return true
  EEPROM.end();       // release RAM copy of EEPROM content

  delay(this->WAIT_PERIOD);
}

String Settings::getLanguage()
{
  return this->language;
}

void Settings::setLastNetworkIP(String lastNetworkIP)
{
  this->lastNetworkIP = lastNetworkIP;
}

String Settings::getLastNetworkIP()
{
  return this->lastNetworkIP;
}