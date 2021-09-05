#include "handleHTTPClient.h"
#include "base64.h"

String getSendData(Settings * pSettings, String macAddress, uint32_t revolutions, uint32_t bladesPerMinute) {
  String result = "";
  result += "\"data\": {";
  result += "\"r\":";        // revolutions of the main axis
  result += "\"";
  result += revolutions;
  result += "\",";

  result += "\"bpm\":";      // blades per minute (enden in dutch)
  result += "\"";
  result += bladesPerMinute;
  result += "\",";

  result += "\"b\":";        // number of blades
  result += "\"";
  result += pSettings->blades;
  result += "\",";

  result += "\"mac\":";      // macAddress
  result += "\"";
  result += macAddress;
  result += "\"";
  result += "}";
  return result;
}

String getSendInfo(Settings * pSettings, WiFiSettings* pWifiSettings, String macAddress, uint32_t revolutions, uint32_t bladesPerMinute) {
  String result = "";
  result += "\"info\": {";

  result += "\"v\":";        // firmwareVersion
  result += "\"";
  result += pSettings->getFirmwareVersion();
  result += "\",";

  result += "\"key\":";      // deviceKey
  result += "\"";
  result += pSettings->getDeviceKey();
  result += "\",";

  result += "\"ra\":";
  result += "\"";
  result += pSettings->getRatioArgument();
  result += "\",";

  result += "\"apssid\":";
  result += "\"";
  result += pWifiSettings->readAccessPointSSID();
  result += "\",";

  result += "\"stssid\":";
  result += "\"";
  result += pWifiSettings->readNetworkSSID();
  result += "\",";

  result += "\"cid\":";
  result += "\"";
  result += String(ESP.getFlashChipId());
  result += "\",";
 
  result += "\"crs\":";
  result += "\"";
  result += String(ESP.getFlashChipRealSize());
  result += "\",";
 
  result += "\"csi\":";
  result += "\"";
  result += String(ESP.getFlashChipSize());
  result += "\",";
 
  result += "\"csp\":";
  result += "\"";
  result += String(ESP.getFlashChipSpeed());
  result += "\",";
 
  result += "\"cm\":";
  result += "\"";
  result += String(ESP.getFlashChipMode());
  result += "\"";

  result += "}";
  return result;
}

void sendContentToTarget(asyncHTTPrequest* pRequest, WiFiClient wifiClient, Settings * pSettings, WiFiSettings* pWifiSettings, String macAddress, uint32_t revolutions, uint32_t bladesPerMinute, bool withInfo)
{
  String targetServer = pSettings->getTargetServer();
  uint16_t port =  pSettings->getTargetPort();
  String path =  pSettings->getTargetPath();
  String url = targetServer + ":" + String(port) + path;
 
  // Note: BasicAuthentication does not allow any colon characters
  //       replace them with an underscore
  String key = macAddress;
  key.replace(":", "_");
  // Note: String end with \n character that has to be removed in the header
  String auth = "Basic " + base64::encode(key + ":" + pSettings->getDeviceKey());
  auth.replace("\n","");

  String postData = "{";
  postData += getSendData(pSettings, macAddress, revolutions, bladesPerMinute);
  if (withInfo == true) {
    postData += ",";
    postData += getSendInfo(pSettings, pWifiSettings, macAddress, revolutions, bladesPerMinute);
  }
  postData += "}";

  if (pRequest->readyState() == 0 || pRequest->readyState() == 4)
  {
    pRequest->open("POST", url.c_str());
    pRequest->setReqHeader("Content-Type", "application/json");
    pRequest->setReqHeader("Cache-Control", "no-cache");
    pRequest->setReqHeader("Connection", "keep-alive");
    pRequest->setReqHeader("Pragma", "no-cache");
    pRequest->setReqHeader("WWW-Authenticate", "Basic realm=\"role_model\", charset=\"UTF-8\"");
    pRequest->setReqHeader("Authorization", auth.c_str());
    pRequest->send(postData);
  }
}

String getAsyncResponse(asyncHTTPrequest* pRequest) {
  if (pRequest->readyState() == 4)
  {
    if (pRequest->responseHTTPcode() == 200)
    {
      return pRequest->responseText();;
    }
  }
  return "";
}
