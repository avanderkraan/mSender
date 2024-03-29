#ifndef WIFISETTINGS_H    // To make sure you don't declare the function more than once by including the header multiple times.
#define WIFISETTINGS_H
#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <EEPROM.h>               // used to store and read settings
#include "settings.h"
#include "dencrypt.h"

class WiFiSettings
{
private:
  /* Maximum size of EEPROM, SPI_FLASH_SEC_SIZE comes from spi_flash.h */
  const uint16_t MAX_EEPROM_SIZE = SPI_FLASH_SEC_SIZE;

  /* wait period in milliseconds, value comes from the Settings class */
  uint32_t WAIT_PERIOD = 0;

  /* maximum length of SSID Name string, excluding NULL character */
  const uint8_t MAX_SSID = 32;
  
  /* maximum length of password string, excluding NULL character */
  const uint8_t MAX_PASSWORD = 32;
  
  /* SSID */
  String ssidAccessPoint = "";

  /* Password */
  String passwordAccessPoint = "administrator";

  /* SSID */
  String ssidNetwork = "";

  /* Password */
  String passwordNetwork = "";

  /* first address for WiFiSettings storage, value comes from the Settings class */
  uint16_t address = 0;

  /* sizeof of serialized variable, marked as 'to store' */
  uint16_t storageSize;

  /* Storage size check, default false */
  bool storageSizeIsAvailable = false;

  Dencrypt dencrypt = Dencrypt();
  
public:
  WiFiSettings(Settings * pSettings)
  {
    this->WAIT_PERIOD = pSettings->WAIT_PERIOD;
    /* Storage for AP and Network SSID, plus AP and Network Password */
    this->storageSize = 516;   // including 4 NULL characters in total (1 for each part) 

    /* first thing to do is set the start-address for this module to the offsetaddress as defined in Settings */ 
    this->address = pSettings->getWiFiDataAddress();

    /* if there is not enough space on EEPROM, writing will fail and reading will return an empty String */
    if (pSettings->getWiFiDataAddress() + this->storageSize < this->MAX_EEPROM_SIZE)  // is there enough space on EEPROM?
    {
      this->storageSizeIsAvailable = true;

      if (! this->isInitialized()) {
        this->setAccessPointSSID(String("ESP-" + WiFi.macAddress()));
        this->setAccessPointPassword(this->passwordAccessPoint);
        this->saveAuthorizationAccessPoint();
      }
      else
      {
        this->ssidNetwork = this->readNetworkSSID();
        this->passwordNetwork = this->readNetworkPassword();
        this->ssidAccessPoint = this->readAccessPointSSID();
        this->passwordAccessPoint = this->readAccessPointPassword();
      }
    }
  };

  ~WiFiSettings()
  {
  };

  /* saves settings in EEPROM starting on EEPROM-address (default = 0), returns length of saved bytes */
  uint16_t saveAuthorizationAccessPoint();

  /* saves settings in EEPROM starting on EEPROM-address (default = 0), returns length of saved bytes */
  uint16_t saveAuthorizationNetwork();

  /* set the Access Point SSID in the class variable */
  void setAccessPointSSID(String ssid);

  /* get the Access Point SSID from the class variable */
  String getAccessPointSSID();

  /* set the Access Point Password in the class variable */
  void setAccessPointPassword(String password);

  /* get the Access Point Password in the class variable */
  String getAccessPointPassword();

  /* set the WiFi Network SSID in the class variable */
  void setNetworkSSID(String ssid);

  /* get the WiFi Network SSID from the class variable */
  String getNetworkSSID();

  /* set the WiFi Network Password in the class variable */
  void setNetworkPassword(String password);

  /* read AccessPoint SSID from EEPROM, does NOT save it in the class variable */
  String readAccessPointSSID();

  /* read AccessPoint Password from EEPROM, does NOT save it in the class variable */
  String readAccessPointPassword();

  /* read Network SSID from EEPROM, does NOT save it in the class variable */
  String readNetworkSSID();

 /* get the WiFi Network Password in the class variable */
  String getNetworkPassword();

  /* read Network Password from EEPROM, does NOT save it in the class variable */
  String readNetworkPassword();

  /* erase settings, set value ff on every EEPROM Access Point Settings address, set class AccessPoint vaiables SSID and Password to factory value, returns true if it succeeds */
  bool eraseAccessPointSettings();

  /* erase settings, set value ff on every EEPROM Network Settings address, set class Network vaiables SSID and Password to factory value, returns true if it succeeds */
  bool eraseNetworkSettings();

  /* erase settings, set value ff on every EEPROM WiFiSettings address, set class AccessPoint and Network vaiables SSID and Password to factory value, returns true if it succeeds */
  bool eraseWiFiSettings();

private:
  /* checks to see if the WiFiSettings are already set, assuming that 0xff means unset */
  bool isInitialized();

  /* does the erase of EEPROM addresses */
  bool eraseSettings(uint16_t startAddress, uint16_t lastAddress);

public:
  /* Used in this class program to check the availablity of storage space */
  bool getStorageSizeIsAvailable();

};

#endif