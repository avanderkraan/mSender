#include "handleWebServer.h"

void info(ESP8266WebServer &server, Settings * pSettings, WiFiSettings * pWifiSettings) {
  String starthtml = "<!DOCTYPE HTML>\r\n<html>\r\n";
  starthtml += "<head>\r\n";
  starthtml += "<meta charset=\"utf-8\">\r\n";
  starthtml += "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\r\n";
  starthtml += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n";
  starthtml += "<!-- The above 3 meta tags *must* come first in the head; any other head content must come *after* these tags -->\r\n";
  starthtml += "<link rel='icon' type='image/png' href='data:image/png;base64,iVBORw0KGgo='>\r\n";
  starthtml += "<title>info</title>\r\n";
  starthtml += "</head>\r\n";
  starthtml += "<body>\r\n";
  String endhtml = "</body>\r\n";
  endhtml += "</html>\r\n";

  String result = starthtml;
  String myIP = "";
  if (WiFi.getMode() == WIFI_AP)
  {
    myIP = WiFi.softAPIP().toString();
  }
  if (WiFi.getMode() == WIFI_STA)
  {
    myIP = WiFi.localIP().toString();

    result += "Passwords and IP addresses are not sent to the server<br>\r\n";
    result += "Information with bold characters which you see has been sent to ";
    result += pSettings->getTargetServer();
  }

  result += "\r\n\r\n<br><br>IP address: ";
  result += myIP;

  result += "\r\n<br><strong>Firmware version: ";
  result += pSettings->getFirmwareVersion();

  result += "\r\n<br>Ratio: ";
  result += pSettings->getRatioArgument();

  result += "\r\n<br>Access Point SSID: ";
  result += pWifiSettings->readAccessPointSSID();

  result += "\r\n<br>Network SSID: ";
  result += pWifiSettings->readNetworkSSID();
  result += "</strong>";

  result += "\r\n<br>Server name: ";
  result += pSettings->getTargetServer();

  result += "\r\n<br>Server port: ";
  result += String(pSettings->getTargetPort());

  result += "\r\n<br>Server path: ";
  result += pSettings->getTargetPath();

  result += "<br>\r\n";
  result += "<br>\r\n";
  result += "<a href='/help/'>Go to the home/help page</a>\r\n";

  result += endhtml;

  server.sendHeader("Cache-Control", "no-cache");
  server.sendHeader("Connection", "keep-alive");
  server.sendHeader("Pragma", "no-cache");
  server.send(200, "text/html", result);   
}

void showWiFiMode(ESP8266WebServer &server, Settings * pSettings)
{
  String result = "<!DOCTYPE HTML>\r\n<html>\r\n";
  result += "<head>\r\n";
  result += "<meta charset=\"utf-8\">\r\n";
  result += "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\r\n";
  result += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n";
  result += "<!-- The above 3 meta tags *must* come first in the head; any other head content must come *after* these tags -->\r\n";
  result += "<link rel='icon' type='image/png' href='data:image/png;base64,iVBORw0KGgo='>\r\n";
  result += "<title>mill</title>\r\n";
  result += "</head>\r\n";
  result += "<body>\r\n";
  result += "WiFi mode is changing if both WiFi leds are on\r\n";
  result += "<br>\r\n";
  result += "<br>\r\n";
  result += "<a href='/help/'>Go to the home/help page</a>\r\n";
  result += "\r\n</body>\r\n</html>\r\n";
  server.sendHeader("Cache-Control", "no-cache");
  server.sendHeader("Connection", "keep-alive");
  server.sendHeader("Pragma", "no-cache");
  server.send(200, "text/html", result);
}

void help(ESP8266WebServer &server, Settings * pSettings)
{
  String result = "<!DOCTYPE HTML>\r\n<html>\r\n";
  result += "<head>\r\n";
  result += "<meta charset=\"utf-8\">\r\n";
  result += "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\r\n";
  result += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n";
  result += "<!-- The above 3 meta tags *must* come first in the head; any other head content must come *after* these tags -->\r\n";
  result += "<link rel='icon' type='image/png' href='data:image/png;base64,iVBORw0KGgo='>\r\n";
  result += "<title>mill</title>\r\n";
  result += "</head>\r\n";
  result += "<body>\r\n";
  result += "<input id=\"EN\" type=\"button\" onclick=\"selectLanguage(this)\" value=\"English\">\r\n";
  result += "<input id=\"NL\" type=\"button\" onclick=\"selectLanguage(this)\" value=\"Nederlands\">\r\n";
  result += "<br><br>\r\n";
  result += "WiFi mode can be changed using the small button on the Counter<br\r\n";
  result += "Both leds will go on and after a while one will stay on: green is Station, yellow is Access Point\r\n";
  result += "<br>\r\n";

  result += "WiFi mode: ";
  if (pSettings->beginAsAccessPoint() == true)
  {
    result += "Access Point\r\n";
    result += "<br>\r\n";
    result += "(url: <a href='http://molen.local/' target='_blank'>molen.local</a> or <a href='http://192.168.4.1/' target='_blank'>http://192.168.4.1</a>)\r\n";
  }
  else
  {
    result += "Network Station\r\n";
    result += "<br>\r\n";
    result += "(url: <a href='http://molen.local/' target='_blank'>molen.local</a> or via a local IP address, last known is: <a href='http://";
    result += pSettings->getLastNetworkIP();
    result += "/' target='_blank'>";
    result += pSettings->getLastNetworkIP();
    result += "</a>\r\n";
    result += ")\r\n";
  }  
  result += "<br>\r\n";
  result += "<div id=\"sendMessage\"></div>\r\n";
  result += "<br><br>\r\n";

  result += "<input id='restartButton' type='button' onclick='restart()' value='Restart'<br>\r\n";
  result += "<br><br>\r\n";

  result += "Version: <span id='version'></span>\r\n";
  result += " <input id='updateFirmwareButton' type='button' onclick='updateFirmware()' value='Update Firmware'<br>\r\n";
  result += " <div id=\"updateFirmwareMessage\"><div>\r\n";
  result += "<br><br>\r\n";

  result += "Menu\r\n";
  result += "<br><br><br>\r\n";
  result += "<a href='/help/'>Help</a> help/home screen\r\n";
  result += "<br><br>\r\n";
  result += "<a href='/wifi/'>WiFi</a> settings to connect the Counter to WiFi\r\n";
  result += "<br><br>\r\n";
  result += "<a href='/info/'>Information</a> shown on screen and partially sent to the server. Passwords and IP addresses are not sent<br>\r\n";
  result += "<br><br>\r\n";
  result += "<script>\r\n";
  result += "  function restart() {\r\n";
  result += "    document.getElementById(\"restartButton\").disabled = true\r\n";
  result += "    document.getElementById(\"updateFirmwareButton\").disabled = true\r\n";
  result += "    document.getElementById(\"updateFirmwareMessage\").innerHTML = \"Please refresh this page after about 1 minute\"\r\n";
  result += "    var params = \"name=restart\";\r\n";
  result += "    sendUpdateFirmware(params, \"/restart/\");\r\n";
  result += "  };\r\n";
  result += "</script>\r\n";

  result += "<script>\r\n";
  result += "  document.getElementById(\"version\").innerHTML = \"";
  result += pSettings->getFirmwareVersion();
  result += "\";\r\n";
  result += "  function updateFirmware() {\r\n";
  result += "    document.getElementById(\"restartButton\").disabled = true\r\n";
  result += "    document.getElementById(\"updateFirmwareButton\").disabled = true\r\n";
  result += "    document.getElementById(\"updateFirmwareMessage\").innerHTML = \"Please refresh this page after about 1 minute\"\r\n";
  result += "    var params = \"name=update\";\r\n";
  result += "    sendUpdateFirmware(params, \"/update/\");\r\n";
  result += "  };\r\n";
  result += "</script>\r\n";

  result += "<script>\r\n";
  result += "  function selectLanguage(component) {\r\n";
  result += "    var params = \"name=help\" + \"&language=\" + component.id;\r\n";
  result += "    document.getElementById(\"NL\").disabled = true;\r\n";
  result += "    document.getElementById(\"EN\").disabled = true;\r\n";
  result += "    sendData(params, \"/language/\");\r\n";
  result += "  }\r\n";
  result += "  function sendData(data, path) {\r\n";
  result += "    var xhr = new XMLHttpRequest();   // new HttpRequest instance\r\n";
  result += "    xhr.open(\"POST\", path);\r\n";
  result += "    xhr.setRequestHeader(\"Content-Type\", \"application/x-www-form-urlencoded\");\r\n";
  result += "    //xhr.setRequestHeader(\"Content-Type\", \"application/json\");\r\n";
  result += "   document.getElementById(\"sendMessage\").innerHTML = \"Please wait\";\r\n";
  result += "    xhr.onreadystatechange = function() { // Call a function when the state changes.\r\n";
  result += "     var myResponseText = \"\";\r\n";
  result += "      if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {\r\n";
  result += "        window.location.reload();\r\n";
  //result += "       myResponseText = this.responseText || \"\";\r\n";
  result += "     }\r\n";
  result += "      if (this.readyState === XMLHttpRequest.DONE && this.status !== 200) {\r\n";
  result += "       myResponseText = this.statusText || \"\";\r\n";
  result += "      }\r\n";
  result += "      document.getElementById(\"sendMessage\").innerHTML = myResponseText;\r\n";
  result += "    }\r\n";
  result += "    xhr.send(data);\r\n";
  result += "  }\r\n";

  result += "  function sendUpdateFirmware(data, path) {\r\n";
  result += "    var xhr = new XMLHttpRequest();   // new HttpRequest instance\r\n";
  result += "    xhr.open(\"POST\", path);\r\n";
  result += "    xhr.setRequestHeader(\"Content-Type\", \"application/x-www-form-urlencoded\");\r\n";
  result += "   document.getElementById(\"sendMessage\").innerHTML = \"Please wait\";\r\n";
  result += "    xhr.onreadystatechange = function() { // Call a function when the state changes.\r\n";
  result += "     var myResponseText = \"\";\r\n";
  result += "      if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {\r\n";
  result += "        window.location.reload();\r\n";
  //result += "       myResponseText = this.responseText || \"\";\r\n";
  result += "     }\r\n";
  result += "      if (this.readyState === XMLHttpRequest.DONE && this.status !== 200) {\r\n";
  result += "       myResponseText = this.statusText || \"\";\r\n";
  result += "      }\r\n";
  result += "      document.getElementById(\"sendMessage\").innerHTML = myResponseText;\r\n";
  result += "    }\r\n";
  result += "    xhr.send(data);\r\n";
  result += "  }\r\n";

  result += "</script>\r\n";

  result += "\r\n</body>\r\n</html>\r\n";
  server.sendHeader("Cache-Control", "no-cache");
  server.sendHeader("Connection", "keep-alive");
  server.sendHeader("Pragma", "no-cache");
  server.send(200, "text/html", result);
}

void wifi(ESP8266WebServer &server, Settings * pSettings, WiFiSettings * pWiFiSettings)
{
  String result = "<!DOCTYPE HTML>\r\n<html>\r\n";
  result += "<head>\r\n";
  result += "<meta charset=\"utf-8\">\r\n";
  result += "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\r\n";
  result += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n";
  result += "<!-- The above 3 meta tags *must* come first in the head; any other head content must come *after* these tags -->\r\n";
  result += "<link rel='icon' type='image/png' href='data:image/png;base64,iVBORw0KGgo='>\r\n";
  result += "<title>mill</title>\r\n";
  result += "</head>\r\n";
  result += "<body>\r\n";
  result += "Choose AccessPoint to change the default AccessPoint SSID and password\r\n";
  result += "<br>\r\n";
  result += "Choose Network to connect the sender to your WiFi\r\n";
  result += "<br><br>\r\n";
  result += "<input type=\"radio\" name=\"wifi\" onclick=\"displayWiFiMode()\" value=\"ap\">Access Point\r\n";
  result += "<br>\r\n";
  result += "<input type=\"radio\" name=\"wifi\" onclick=\"displayWiFiMode()\" value=\"network\">Network Station\r\n";
  result += "<br>\r\n";
  result += "<input type=\"radio\" name=\"wifi\" onclick=\"displayWiFiMode()\" value=\"erase\">Go to Erase-WiFi-Data menu\r\n";
  result += "<br>\r\n";
  result += "<br>\r\n";
  result += "<div id=\"ap\">\r\n";
  result += "  Clients can get access to this Access Point using the SSID and password entered below\r\n";
  result += "  <br>\r\n";
  result += "  An empty SSID will result in a default SSID for the Counter\r\n";
  result += "  <br>\r\n";
  result += "  An empty password will result in an unencrypted, open Access Point\r\n";
  result += "  <br>\r\n";
  result += "  SSID: <input type=\"text\" name=\"ssid\" maxlength=\"32\" size=\"33\" placeholder=\"";
  if (pWiFiSettings->getAccessPointSSID() == "")
  {
    result += "SSID";
  }
  else
  {
    result += pWiFiSettings->getAccessPointSSID();
  }
  result += "\" value=\"";
  result += pWiFiSettings->getAccessPointSSID();
  result += "\">\r\n";
  result += "  <br>\r\n";
  result += "  Password: <input type=\"password\" name=\"password\" maxlength=\"32\" size=\"33\" placeholder=\"";
  if (pWiFiSettings->getAccessPointPassword() == "")
  {
    result += "password";
  }
  else
  {
    result += pWiFiSettings->getAccessPointPassword();
  }
  result += "\" value=\"";
  result += pWiFiSettings->getAccessPointPassword();
  result += "\" onkeyup=\"checkURIComponent(this, 'apPasswordMessage', 'Invalid password character');\">\r\n";
  result += " <span id=\"apPasswordMessage\"></span>\r\n";
  result += "  <br><br>\r\n";
  result += "  After 'Save' wait for confirmation.\r\n";
  result += "  <br>\r\n";
  result += "  <input id=\"apButton\" type=\"button\" name=\"apButton\" value=\"Save\" onclick=\"saveAP(this)\">\r\n";
  result += "  <input type=\"button\" name=\"apCancelButton\" value=\"Cancel\" onclick=\"cancelWiFi()\">\r\n";
  result += "</div>\r\n";
  result += "\r\n";
  result += "<div id=\"network\">\r\n";
  result += "  Connect to one of the WiFi networks below\r\n";
  result += "  <br><br>\r\n";
  result += "  <span id=\"ssidList\">Please wait for a list or a message to show here</span>\r\n";
  result += "  <br><br>\r\n";
  result += "  <span id=\"selectedWiFiPassword\">";
  result += "Give the password for the selected WiFi network <span id=\"selectedWiFi\"></span>\r\n";
  result += "     <br>\r\n";
  result += "     Password: <input type=\"password\" name=\"password\" maxlength=\"32\" size=\"33\" placeholder=\"password\" value=\"";
  result += "\" onkeyup=\"checkURIComponent(this, 'networkPasswordMessage', 'Invalid password character');\">\r\n";
  result += "     <br><br>\r\n";
  result += "  </span>\r\n";
  result += "  After 'Save' wait for confirmation.\r\n";
  result += "  <br>\r\n";
  result += "   <input id=\"networkButton\" type=\"button\" name=\"networkButton\" value=\"Save\" onclick=\"saveNetwork(this)\">\r\n";
  result += "   <input type=\"button\" name=\"networkCancelButton\" value=\"Cancel\" onclick=\"cancelWiFi()\">\r\n";
  result += "  <span id=\"networkPasswordMessage\"></span>\r\n";
  result += "</div>\r\n";
  result += "<div id=\"erase\">\r\n";
  result += "  !!!Warning!!!\r\n";
  result += "  <br><br>\r\n";
  result += "  A click on an Erase button will immediately erase saved data! Your current connection will stay alive.\r\n";
  result += "  <br><br>\r\n";
  result += "Do NOT Erase and <input type=\"button\" name=\"networkButton\" value=\"Cancel\" onclick=\"cancelWiFi()\">\r\n";
  result += "  <br><br>\r\n";
  result += "  <input id=\"eraseAPData\" type=\"button\" name=\"eraseAPDataButton\" value=\"Erase\" onclick=\"eraseWiFiData(this)\"> Erase Access Point data, will result in an unencrypted, open Access Point\r\n";
  result += "  <br>\r\n";
  result += "  <input id=\"eraseNetworkData\" type=\"button\" name=\"eraseNetworkDataButton\" value=\"Erase\" onclick=\"eraseWiFiData(this)\"> Erase Network data\r\n";
  result += "  <br>\r\n";
  result += "  <input id=\"eraseWiFiData\" type=\"button\" name=\"eraseWiFiDataButton\" value=\"Erase\" onclick=\"eraseWiFiData(this)\"> Erase Access Point AND Network data\r\n";
  result += "</div>\r\n";
  result += "\r\n";
  result += "  <br>\r\n";
  result += "<div id=\"sendMessage\"></div>\r\n";
  result += "\r\n";
  result += "<br>\r\n";
  result += "<br>\r\n";
  result += "<a href='/help/'>Go to the home/help page</a>\r\n";
  result += "<script>\r\n";
  result += "function checkURIComponent(component, messageId, message) {\r\n";
  result += "  var invalidCharacterArray = [\"&\"]\r\n";
  result += "  var buttonNodeId = component.parentNode.id;\r\n";
  result += "  if (buttonNodeId == \"selectedWiFiPassword\") {\r\n";
  result += "    buttonNodeId = component.parentNode.parentNode.id;\r\n";
  result += "  };\r\n";
  result += "  var buttonId = buttonNodeId + \"Button\"\r\n";
  result += "   var valid = true;\r\n";
  result += "   for (var c = 0; c < invalidCharacterArray.length; c++) {\r\n";
  result += "    if (component.value.indexOf(invalidCharacterArray[c]) > -1) {\r\n";
  result += "      valid = false;\r\n";
  result += "      break;\r\n";
  result += "    };\r\n";
  result += "  };\r\n";
  result += "  if (valid) {\r\n";
  result += "    document.getElementById(messageId).innerHTML = \"\";\r\n";
  result += "  }\r\n";
  result += "  else {\r\n";
  result += "    document.getElementById(messageId).innerHTML = message;\r\n";
  result += "  }\r\n";
  result += "  document.getElementById(buttonId).disabled = !valid;\r\n";
  result += "   return valid;\r\n";
  result += "}\r\n";
  result += "  function cancelWiFi() {\r\n";
  result += "    window.location.reload();\r\n";
  result += "  }\r\n";
  result += "  function sendData(data) {\r\n";
  result += "    var xhr = new XMLHttpRequest();   // new HttpRequest instance\r\n";
  result += "    xhr.open(\"POST\", \"/wifiConnect/\");\r\n";
  result += "    xhr.setRequestHeader(\"Content-Type\", \"application/x-www-form-urlencoded\");\r\n";
  result += "    //xhr.setRequestHeader(\"Content-Type\", \"application/json\");\r\n";
  result += "   document.getElementById(\"sendMessage\").innerHTML = \"Please wait\";\r\n";
  result += "    xhr.onreadystatechange = function() { // Call a function when the state changes.\r\n";
  result += "     var myResponseText = \"\";\r\n";
  result += "      if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {\r\n";
  result += "       myResponseText = this.responseText || \"\";\r\n";
  result += "     }\r\n";
  result += "      if (this.readyState === XMLHttpRequest.DONE && this.status !== 200) {\r\n";
  result += "       myResponseText = this.statusText || \"\";\r\n";
  result += "      }\r\n";
  result += "      document.getElementById(\"sendMessage\").innerHTML = myResponseText;\r\n";
  result += "    }\r\n";
  result += "    xhr.send(data);\r\n";
  result += "  }\r\n";
  result += "\r\n";
  result += "  function saveAP(content) {\r\n";
  result += "    var children = content.parentNode.childNodes;\r\n";
  result += "    var ssid = \"\";\r\n";
  result += "    var password = \"\";\r\n";
  result += "    for (var i = 0; i < children.length; i++) {\r\n";
  result += "      if (children[i].name == \"ssid\") {\r\n";
  result += "        ssid = children[i].value || \"\";\r\n";
  result += "      }\r\n";
  result += "      if (children[i].name == \"password\") {\r\n";
  result += "        password = children[i].value || \"\";\r\n";
  result += "      }\r\n";
  result += "    }\r\n";
  result += "    var params = \"name=ap\" + \"&ssid=\" + encodeURIComponent(ssid) + \"&password=\" + encodeURIComponent(password);\r\n";
  result += "    sendData(params);\r\n";
  result += "    //var json_upload = JSON.stringify({name:\"ap\", ssid:ssid, password:password});\r\n";
  result += "    //sendData(json_upload);\r\n";
  result += "  }\r\n";
  result += "\r\n";
  result += "  function saveNetwork(content) {\r\n";
  result += "    var children = document.getElementById(\"selectedWiFiPassword\").childNodes;\r\n";
  result += "    var ssid = document.getElementById(\"selectedWiFi\").innerHTML;\r\n";
  result += "    var password = \"\";\r\n";
  result += "    for (var i = 0; i < children.length; i++) {\r\n";
  result += "      if (children[i].name == \"password\") {\r\n";
  result += "        password = children[i].value || \"\";\r\n";
  result += "      }\r\n";
  result += "    }\r\n";
  result += "    var params = \"name=network\" + \"&ssid=\" + encodeURIComponent(ssid) + \"&password=\" + encodeURIComponent(password);\r\n";
  result += "    sendData(params);\r\n";
  result += "    //var json_upload = JSON.stringify({name:\"network\", ssid:ssid, password:password});\r\n";
  result += "    //sendData(json_upload);\r\n";
  result += "  }\r\n";
  result += "  function eraseWiFiData(content) {\r\n";
  result += "  var params = \"name=erase\" + \"&target=\" + content.id;\r\n";
  result += "      sendData(params);\r\n";
  result += "  }\r\n";
  result += "</script>\r\n";
  result += "\r\n";
  result += "<script>\r\n";

  result += "function showNetworkList(networks) {\r\n";
  result += "  var showNetworkList = [];\r\n";
  result += "  var showNetwork = \"\";\r\n";
  result += "  var networkList = (new Function(\"return [\" + networks + \"];\")());\r\n";
  result += "  networkList.sort((a,b) => (a.dBm > b.dBm) ? 1: (a.dBm === b.dBm) ? ((a.ssid > b.ssid) ? 1: -1) : -1);\r\n";
  result += "  networkList = networkList.filter(function(e) { return e.dBm > -80});\r\n";
  result += "  sortedList = [...networkList].reverse();\r\n";
  result += "  for (var key in sortedList) {\r\n";
  result += "    var value = sortedList[key]\r\n";
  result += "    showNetworkList.push(Object.values(value)[0]);\r\n";
  result += "  }\r\n";
  result += "  var uniqueShowNetworkList = [...new Set(showNetworkList)];\r\n";
  result += "  for (var key in uniqueShowNetworkList) {\r\n";
  result += "    showNetwork += \"<div>\" + uniqueShowNetworkList[key] + \"</div>\";\r\n";
  result += "  }\r\n";
  result += "  if (showNetwork == \"\") {\r\n";
  result += "    showNetwork = \"No WiFi signal available\"\r\n";
  result += "  }\r\n";
  result += "  else {\r\n";
  result += "    document.getElementById(\"selectedWiFiPassword\").style.display=\"block\"\r\n";
  result += "  }\r\n";
  result += "  document.getElementById(\"ssidList\").innerHTML = showNetwork;\r\n";
  result += "}\r\n"; 
  result += "function selectNetworkSSID(content) {\r\n";
  result += "  ssid = \"\"\r\n";
  result += "  if (content.name == \"networkSSID\") {\r\n";
  result += "    ssid = content.value;\r\n";
  result += "  }\r\n";
  result += "  document.getElementById(\"selectedWiFi\").innerHTML = ssid;\r\n";
  result += "}\r\n";
  result += "function loadWiFiNetworkList() {\r\n";
  result += "  var xhttp = new XMLHttpRequest();\r\n";
  result += "  xhttp.onreadystatechange = function() {\r\n";
  result += "    if (this.readyState == 4 && this.status == 200) {\r\n";
  result += "      showNetworkList(this.responseText);\r\n";
  result += "    }\r\n";
  result += "  };\r\n";
  result += "  xhttp.open(\"GET\", \"/networkssid/\", true);\r\n";
  result += "  xhttp.send();\r\n";
  result += "}\r\n";
  result += "</script>\r\n";
  result += "\r\n";
  result += "<script>\r\n";
  result += "function displayWiFiMode() {\r\n";
  result += "var ele = document.getElementsByName('wifi');\r\n";
  result += "\r\n";
  result += "  for(i = 0; i < ele.length; i++) {\r\n";
  result += "    if(ele[i].checked) {\r\n";
  result += "       document.getElementById(ele[i].value).style.display=\"block\";\r\n";
  result += "       if (ele[i].value == 'network') {\r\n";
  result += "        document.getElementById(\"selectedWiFi\").innerHTML = \"\"\r\n";
  result += "        document.getElementById(\"selectedWiFiPassword\").style.display=\"none\"\r\n";
  result += "         loadWiFiNetworkList();\r\n";
  result += "       }\r\n";
  result += "     }\r\n";
  result += "     else {\r\n";
  result += "       document.getElementById(ele[i].value).style.display=\"none\";\r\n";
  result += "     }\r\n";
  result += "  }\r\n";
  result += "}\r\n";
  result += "displayWiFiMode();\r\n";
  result += "</script>\r\n";
  result += "</body>\r\n";
  result += "</html>\r\n";
  
  server.sendHeader("Cache-Control", "no-cache");
  server.sendHeader("Connection", "keep-alive");
  server.sendHeader("Pragma", "no-cache");
  server.send(200, "text/html", result);
}

///////////////// Vanaf hier Nederlands ///////////////////////////
void info_nl(ESP8266WebServer &server, Settings * pSettings, WiFiSettings * pWifiSettings) {
  String starthtml = "<!DOCTYPE HTML>\r\n<html>\r\n";
  starthtml += "<head>\r\n";
  starthtml += "<meta charset=\"utf-8\">\r\n";
  starthtml += "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\r\n";
  starthtml += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n";
  starthtml += "<!-- The above 3 meta tags *must* come first in the head; any other head content must come *after* these tags -->\r\n";
  starthtml += "<link rel='icon' type='image/png' href='data:image/png;base64,iVBORw0KGgo='>\r\n";
  starthtml += "<title>info</title>\r\n";
  starthtml += "</head>\r\n";
  starthtml += "<body>\r\n";
  String endhtml = "</body>\r\n";
  endhtml += "</html>\r\n";

  String result = starthtml;
  String myIP = "";
  if (WiFi.getMode() == WIFI_AP)
  {
    myIP = WiFi.softAPIP().toString();
  }
  if (WiFi.getMode() == WIFI_STA)
  {
    myIP = WiFi.localIP().toString();

    result += "Wachtwoorden en IP adressen worden niet naar de server gestuurd<br>\r\n";
    result += "Informatie met dikgedrukte letters die je hieronder ziet is opgestuurd naar ";
    result += pSettings->getTargetServer();     
  }

  result += "\r\n\r\n<br><br>IP adres: ";
  result += myIP;

  result += "\r\n<br><strong>Firmware versie: ";
  result += pSettings->getFirmwareVersion();

  result += "\r\n<br>Ratio: ";
  result += pSettings->getRatioArgument();

  result += "\r\n<br>Access Point SSID: ";
  result += pWifiSettings->readAccessPointSSID();

  result += "\r\n<br>Netwerk SSID: ";
  result += pWifiSettings->readNetworkSSID();
  result += "</strong>";

  result += "\r\n<br>Server naam: ";
  result += pSettings->getTargetServer();

  result += "\r\n<br>Server poort: ";
  result += String(pSettings->getTargetPort());

  result += "\r\n<br>Server pad: ";
  result += pSettings->getTargetPath();

  result += "<br>\r\n";
  result += "<br>\r\n";
  result += "<a href='/help/'>Ga naar de begin/help pagina</a>\r\n";

  result += endhtml;

  server.sendHeader("Cache-Control", "no-cache");
  server.sendHeader("Connection", "keep-alive");
  server.sendHeader("Pragma", "no-cache");
  server.send(200, "text/html", result); 
}

void help_nl(ESP8266WebServer &server, Settings * pSettings)
{
  String result = "<!DOCTYPE HTML>\r\n<html>\r\n";
  result += "<head>\r\n";
  result += "<meta charset=\"utf-8\">\r\n";
  result += "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\r\n";
  result += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n";
  result += "<!-- The above 3 meta tags *must* come first in the head; any other head content must come *after* these tags -->\r\n";
  result += "<link rel='icon' type='image/png' href='data:image/png;base64,iVBORw0KGgo='>\r\n";
  result += "<title>molen</title>\r\n";
  result += "</head>\r\n";
  result += "<body>\r\n";
  result += "<input id=\"EN\" type=\"button\" onclick=\"selectLanguage(this)\" value=\"English\">\r\n";
  result += "<input id=\"NL\" type=\"button\" onclick=\"selectLanguage(this)\" value=\"Nederlands\">\r\n";
  result += "<br><br>\r\n";
  result += "WiFi modus kan gewijzigd worden met de kleine knop op de Teller<br\r\n";
  result += "Beide leds gaan aan en na een tijdje blijft er een aan: groen is Station, geel is Access Point\r\n";
  result += "<br>\r\n";
  result += "WiFi modus: ";
  if (pSettings->beginAsAccessPoint() == true)
  {
    result += "Access Point\r\n";
    result += "<br>\r\n";
    result += "(url: <a href='http://molen.local/' target='_blank'>molen.local</a> of <a href='http://192.168.4.1/' target='_blank'>http://192.168.4.1</a>)\r\n";
  }
  else
  {
    result += "Netwerk Station\r\n";
    result += "<br>\r\n";
    result += "(url: <a href='http://molen.local/' target='_blank'>molen.local</a> of via een lokaal IP adres, laatst bekende adres is: <a href='http://";
    result += pSettings->getLastNetworkIP();
    result += "/' target='_blank'>";
    result += pSettings->getLastNetworkIP();
    result += "</a>\r\n";
    result += ")\r\n";
  }  
  result += "<br>\r\n";
  result += "<div id=\"sendMessage\"></div>\r\n";
  result += "<br><br>\r\n";

  result += "<input id='restartButton' type='button' onclick='restart()' value='Restart'<br>\r\n";
  result += "<br><br>\r\n";

  result += "Versie: <span id='version'></span>\r\n";
  result += " <input id='updateFirmwareButton' type='button' onclick='updateFirmware()' value='Update Firmware'<br>\r\n";
  result += " <div id=\"updateFirmwareMessage\"><div>\r\n";
  result += "<br><br>\r\n";
  result += "Menu\n";
  result += "<br><br><br>\r\n";
  result += "<a href='/help/'>Help</a> begin/help scherm\r\n";
  result += "<br><br>\r\n";
  result += "<a href='/wifi/'>WiFi</a> instellingen om de Teller te koppelen aan WiFi\r\n";
  result += "<br><br>\r\n";
  result += "<a href='/info/'>Informatie</a> op het scherm, wordt deels verzonden naar de server. Wachtwoorden en IP adressen worden niet verstuurd<br>\r\n";
  result += "<br><br>\r\n";
  result += "<script>\r\n";
  result += "  function restart() {\r\n";
  result += "    document.getElementById(\"restartButton\").disabled = true\r\n";
  result += "    document.getElementById(\"updateFirmwareButton\").disabled = true\r\n";
  result += "    document.getElementById(\"updateFirmwareMessage\").innerHTML = \"Please refresh this page after about 1 minute\"\r\n";
  result += "    var params = \"name=restart\";\r\n";
  result += "    sendUpdateFirmware(params, \"/restart/\");\r\n";
  result += "  };\r\n";
  result += "</script>\r\n";

  result += "<script>\r\n";
  result += "  document.getElementById(\"version\").innerHTML = \"";
  result += pSettings->getFirmwareVersion();
  result += "\";\r\n";
  result += "  function updateFirmware() {\r\n";
  result += "    document.getElementById(\"restartButton\").disabled = true\r\n";
  result += "    document.getElementById(\"updateFirmwareButton\").disabled = true\r\n";
  result += "    document.getElementById(\"updateFirmwareMessage\").innerHTML = \"Na ongeveer 1 minuut kun je de pagina verversen\"\r\n";
  result += "    var params = \"name=update\";\r\n";
  result += "    sendUpdateFirmware(params, \"/update/\");\r\n";
  result += "  };\r\n";
  result += "</script>\r\n";

  result += "<script>\r\n";
  result += "  function selectLanguage(component) {\r\n";
  result += "    var params = \"name=help\" + \"&language=\" + component.id;\r\n";
  result += "    document.getElementById(\"NL\").disabled = true;\r\n";
  result += "    document.getElementById(\"EN\").disabled = true;\r\n";
  result += "    sendData(params, \"/language/\");\r\n";
  result += "  }\r\n";
  result += "  function sendData(data, path) {\r\n";
  result += "    var xhr = new XMLHttpRequest();   // new HttpRequest instance\r\n";
  result += "    xhr.open(\"POST\", path);\r\n";
  result += "    xhr.setRequestHeader(\"Content-Type\", \"application/x-www-form-urlencoded\");\r\n";
  result += "    //xhr.setRequestHeader(\"Content-Type\", \"application/json\");\r\n";
  result += "    document.getElementById(\"sendMessage\").innerHTML = \"Please wait\";\r\n";
  result += "    xhr.onreadystatechange = function() { // Call a function when the state changes.\r\n";
  result += "      var myResponseText = \"\";\r\n";
  result += "        if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {\r\n";
  result += "          window.location.reload();\r\n";
  result += "        }\r\n";
  result += "      if (this.readyState === XMLHttpRequest.DONE && this.status !== 200) {\r\n";
  result += "       myResponseText = this.statusText || \"\";\r\n";
  result += "      }\r\n";
  result += "      document.getElementById(\"sendMessage\").innerHTML = myResponseText;\r\n";
  result += "    }\r\n";
  result += "    xhr.send(data);\r\n";
  result += "  }\r\n";

  result += "  function sendUpdateFirmware(data, path) {\r\n";
  result += "    var xhr = new XMLHttpRequest();   // new HttpRequest instance\r\n";
  result += "    xhr.open(\"POST\", path);\r\n";
  result += "    xhr.setRequestHeader(\"Content-Type\", \"application/x-www-form-urlencoded\");\r\n";
  result += "   document.getElementById(\"sendMessage\").innerHTML = \"Please wait\";\r\n";
  result += "    xhr.onreadystatechange = function() { // Call a function when the state changes.\r\n";
  result += "     var myResponseText = \"\";\r\n";
  result += "      if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {\r\n";
  result += "        window.location.reload();\r\n";
  //result += "       myResponseText = this.responseText || \"\";\r\n";
  result += "     }\r\n";
  result += "      if (this.readyState === XMLHttpRequest.DONE && this.status !== 200) {\r\n";
  result += "       myResponseText = this.statusText || \"\";\r\n";
  result += "      }\r\n";
  result += "      document.getElementById(\"sendMessage\").innerHTML = myResponseText;\r\n";
  result += "    }\r\n";
  result += "    xhr.send(data);\r\n";
  result += "  }\r\n";

  result += "</script>\r\n";
  result += "\r\n</body>\r\n</html>\r\n";
  server.sendHeader("Cache-Control", "no-cache");
  server.sendHeader("Connection", "keep-alive");
  server.sendHeader("Pragma", "no-cache");
  server.send(200, "text/html", result);
}

void wifi_nl(ESP8266WebServer &server, Settings * pSettings, WiFiSettings * pWiFiSettings)
{
  String result = "<!DOCTYPE HTML>\r\n<html>\r\n";
  result += "<head>\r\n";
  result += "<meta charset=\"utf-8\">\r\n";
  result += "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\r\n";
  result += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n";
  result += "<!-- The above 3 meta tags *must* come first in the head; any other head content must come *after* these tags -->\r\n";
  result += "<link rel='icon' type='image/png' href='data:image/png;base64,iVBORw0KGgo='>\r\n";
  result += "<title>molen</title>\r\n";
  result += "</head>\r\n";
  result += "<body>\r\n";
  result += "Kies AccessPoint om de standaard waarden voor AccessPoint SSID en wachtwoord te wijzigen\r\n";
  result += "<br>\r\n";
  result += "Kies Netwerk Station om de zender te koppelen aan je eigen WiFi\r\n";
  result += "<br><br>\r\n";
  result += "<input type=\"radio\" name=\"wifi\" onclick=\"displayWiFiMode()\" value=\"ap\">Access Point\r\n";
  result += "<br>\r\n";
  result += "<input type=\"radio\" name=\"wifi\" onclick=\"displayWiFiMode()\" value=\"network\">Netwerk Station\r\n";
  result += "<br>\r\n";
  result += "<input type=\"radio\" name=\"wifi\" onclick=\"displayWiFiMode()\" value=\"erase\">Ga naar Verwijder WiFi-gegevens menu\r\n";
  result += "<br>\r\n";
  result += "<br>\r\n";
  result += "<div id=\"ap\">\r\n";
  result += "  Apparaten kunnen toegang krijgen tot dit Access Point met het hieronder ingevulde SSID en wachtwoord\r\n";
  result += "  <br>\r\n";
  result += "  Een niet-ingevuld SSID geeft het standaard SSID van het Access Point\r\n";
  result += "  <br>\r\n";
  result += "  Een niet-ingevuld wachtwoord geeft een onveilige, open Access Point\r\n";
  result += "  <br>\r\n";
  result += "  SSID: <input type=\"text\" name=\"ssid\" maxlength=\"32\" size=\"33\" placeholder=\"";
  if (pWiFiSettings->getAccessPointSSID() == "")
  {
    result += "SSID";
  }
  else
  {
    result += pWiFiSettings->getAccessPointSSID();
  }
  result += "\" value=\"";
  result += pWiFiSettings->getAccessPointSSID();
  result += "\">\r\n";
  result += "  <br>\r\n";
  result += "  Wachtwoord: <input type=\"password\" name=\"password\" maxlength=\"32\" size=\"33\" placeholder=\"";
  if (pWiFiSettings->getAccessPointPassword() == "")
  {
    result += "wachtwoord";
  }
  else
  {
    result += pWiFiSettings->getAccessPointPassword();
  }
  result += "\" value=\"";
  result += pWiFiSettings->getAccessPointPassword();
  result += "\" onkeyup=\"checkURIComponent(this, 'apPasswordMessage', 'Ongeldig wachtwoord karakter');\">\r\n";
  result += " <span id=\"apPasswordMessage\"></span>\r\n";
  result += "  <br><br>\r\n";
  result += "  Na 'Save' even geduld tot er een bevestiging is.\r\n";
  result += "  <br>\r\n";
  result += "  <input id=\"apButton\" type=\"button\" name=\"apButton\" value=\"Save\" onclick=\"saveAP(this)\">\r\n";
  result += "  <input type=\"button\" name=\"apCancelButton\" value=\"Cancel\" onclick=\"cancelWiFi()\">\r\n";
  result += "</div>\r\n";
  result += "\r\n";
  result += "<div id=\"network\">\r\n";
  result += "  Maak verbinding met een van de WiFi netwerken hieronder\r\n";
  result += "  <br><br>\r\n";
  result += "  <span id=\"ssidList\">Wacht even tot er een lijst of een melding verschijnt</span>\r\n";
  result += "  <br><br>\r\n";
  result += "  <span id=\"selectedWiFiPassword\">";
  result += "Geef het wachtwoord voor het geselecteerde WiFi netwerk <span id=\"selectedWiFi\"></span>\r\n";
  result += "     <br>\r\n";
  result += "     Wachtwoord: <input type=\"password\" name=\"password\" maxlength=\"32\" size=\"33\" placeholder=\"password\" value=\"";
  result += "\" onkeyup=\"checkURIComponent(this, 'networkPasswordMessage', 'Ongeldig wachtwoord karakter');\">\r\n";
  result += "     <br><br>\r\n";
  result += "  </span>\r\n";
  result += "  Na 'Save' even geduld tot er een bevestiging is.\r\n";
  result += "  <br>\r\n";
  result += "   <input id=\"networkButton\" type=\"button\" name=\"networkButton\" value=\"Save\" onclick=\"saveNetwork(this)\">\r\n";
  result += "   <input type=\"button\" name=\"networkCancelButton\" value=\"Cancel\" onclick=\"cancelWiFi()\">\r\n";
  result += "  <span id=\"networkPasswordMessage\"></span>\r\n";
  result += "</div>\r\n";
  result += "<div id=\"erase\">\r\n";
  result += "  !!!Opgelet!!!\r\n";
  result += "  <br><br>\r\n";
  result += "  Een klik op de 'Erase' knop zal onmiddellijk de opgeslagen gegevens verwijderen! De huidige verbinding blijft bestaan.\r\n";
  result += "  <br><br>\r\n";
  result += "Verwijder NIETS en <input type=\"button\" name=\"networkButton\" value=\"Cancel\" onclick=\"cancelWiFi()\">\r\n";
  result += "  <br><br>\r\n";
  result += "  <input id=\"eraseAPData\" type=\"button\" name=\"eraseAPDataButton\" value=\"Erase\" onclick=\"eraseWiFiData(this)\"> Verwijderen van Access Point resulteert in in een onveilig, open Access Point\r\n";
  result += "  <br>\r\n";
  result += "  <input id=\"eraseNetworkData\" type=\"button\" name=\"eraseNetworkDataButton\" value=\"Erase\" onclick=\"eraseWiFiData(this)\"> Verwijder Netwerk gegevens\r\n";
  result += "  <br>\r\n";
  result += "  <input id=\"eraseWiFiData\" type=\"button\" name=\"eraseWiFiDataButton\" value=\"Erase\" onclick=\"eraseWiFiData(this)\"> Verwijder Access Point EN Netwerk gegevens\r\n";
  result += "</div>\r\n";
  result += "\r\n";
  result += "  <br>\r\n";
  result += "<div id=\"sendMessage\"></div>\r\n";
  result += "\r\n";
  result += "<br>\r\n";
  result += "<br>\r\n";
  result += "<a href='/help/'>Ga naar de begin/help pagina</a>\r\n";
  result += "<script>\r\n";
  result += "function checkURIComponent(component, messageId, message) {\r\n";
  result += "  var invalidCharacterArray = [\"&\"]\r\n";
  result += "  var buttonNodeId = component.parentNode.id;\r\n";
  result += "  if (buttonNodeId == \"selectedWiFiPassword\") {\r\n";
  result += "    buttonNodeId = component.parentNode.parentNode.id;\r\n";
  result += "  };\r\n";
  result += "  var buttonId = buttonNodeId + \"Button\"\r\n";
  result += "   var valid = true;\r\n";
  result += "   for (var c = 0; c < invalidCharacterArray.length; c++) {\r\n";
  result += "    if (component.value.indexOf(invalidCharacterArray[c]) > -1) {\r\n";
  result += "      valid = false;\r\n";
  result += "      break;\r\n";
  result += "    };\r\n";
  result += "  };\r\n";
  result += "  if (valid) {\r\n";
  result += "    document.getElementById(messageId).innerHTML = \"\";\r\n";
  result += "  }\r\n";
  result += "  else {\r\n";
  result += "    document.getElementById(messageId).innerHTML = message;\r\n";
  result += "  }\r\n";
  result += "  document.getElementById(buttonId).disabled = !valid;\r\n";
  result += "   return valid;\r\n";
  result += "}\r\n";
  result += "  function cancelWiFi() {\r\n";
  result += "    window.location.reload();\r\n";
  result += "  }\r\n";
  result += "  function sendData(data) {\r\n";
  result += "    var xhr = new XMLHttpRequest();   // new HttpRequest instance\r\n";
  result += "    xhr.open(\"POST\", \"/wifiConnect/\");\r\n";
  result += "    xhr.setRequestHeader(\"Content-Type\", \"application/x-www-form-urlencoded\");\r\n";
  result += "    //xhr.setRequestHeader(\"Content-Type\", \"application/json\");\r\n";
  result += "   document.getElementById(\"sendMessage\").innerHTML = \"Even geduld\";\r\n";
  result += "    xhr.onreadystatechange = function() { // Call a function when the state changes.\r\n";
  result += "     var myResponseText = \"\";\r\n";
  result += "      if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {\r\n";
  result += "       myResponseText = this.responseText || \"\";\r\n";
  result += "     }\r\n";
  result += "      if (this.readyState === XMLHttpRequest.DONE && this.status !== 200) {\r\n";
  result += "       myResponseText = this.statusText || \"\";\r\n";
  result += "      }\r\n";
  result += "      document.getElementById(\"sendMessage\").innerHTML = myResponseText;\r\n";
  result += "    }\r\n";
  result += "    xhr.send(data);\r\n";
  result += "  }\r\n";
  result += "\r\n";
  result += "  function saveAP(content) {\r\n";
  result += "    var children = content.parentNode.childNodes;\r\n";
  result += "    var ssid = \"\";\r\n";
  result += "    var password = \"\";\r\n";
  result += "    for (var i = 0; i < children.length; i++) {\r\n";
  result += "      if (children[i].name == \"ssid\") {\r\n";
  result += "        ssid = children[i].value || \"\";\r\n";
  result += "      }\r\n";
  result += "      if (children[i].name == \"password\") {\r\n";
  result += "        password = children[i].value || \"\";\r\n";
  result += "      }\r\n";
  result += "    }\r\n";
  result += "    var params = \"name=ap\" + \"&ssid=\" + encodeURIComponent(ssid) + \"&password=\" + encodeURIComponent(password);\r\n";
  result += "    sendData(params);\r\n";
  result += "    //var json_upload = JSON.stringify({name:\"ap\", ssid:ssid, password:password});\r\n";
  result += "    //sendData(json_upload);\r\n";
  result += "  }\r\n";
  result += "\r\n";
  result += "  function saveNetwork(content) {\r\n";
  result += "    var children = document.getElementById(\"selectedWiFiPassword\").childNodes;\r\n";
  result += "    var ssid = document.getElementById(\"selectedWiFi\").innerHTML;\r\n";
  result += "    var password = \"\";\r\n";
  result += "    for (var i = 0; i < children.length; i++) {\r\n";
  result += "      if (children[i].name == \"password\") {\r\n";
  result += "        password = children[i].value || \"\";\r\n";
  result += "      }\r\n";
  result += "    }\r\n";
  result += "    var params = \"name=network\" + \"&ssid=\" + encodeURIComponent(ssid) + \"&password=\" + encodeURIComponent(password);\r\n";
  result += "    sendData(params);\r\n";
  result += "    //var json_upload = JSON.stringify({name:\"network\", ssid:ssid, password:password});\r\n";
  result += "    //sendData(json_upload);\r\n";
  result += "  }\r\n";
  result += "  function eraseWiFiData(content) {\r\n";
  result += "  var params = \"name=erase\" + \"&target=\" + content.id;\r\n";
  result += "      sendData(params);\r\n";
  result += "  }\r\n";
  result += "</script>\r\n";
  result += "\r\n";
  result += "<script>\r\n";

  result += "function showNetworkList(networks) {\r\n";
  result += "  var showNetworkList = [];\r\n";
  result += "  var showNetwork = \"\";\r\n";
  result += "  var networkList = (new Function(\"return [\" + networks + \"];\")());\r\n";
  result += "  networkList.sort((a,b) => (a.dBm > b.dBm) ? 1: (a.dBm === b.dBm) ? ((a.ssid > b.ssid) ? 1: -1) : -1);\r\n";
  result += "  networkList = networkList.filter(function(e) { return e.dBm > -80});\r\n";
  result += "  sortedList = [...networkList].reverse();\r\n";
  result += "  for (var key in sortedList) {\r\n";
  result += "    var value = sortedList[key]\r\n";
  result += "    showNetworkList.push(Object.values(value)[0]);\r\n";
  result += "  }\r\n";
  result += "  var uniqueShowNetworkList = [...new Set(showNetworkList)];\r\n";
  result += "  for (var key in uniqueShowNetworkList) {\r\n";
  result += "    showNetwork += \"<div>\" + uniqueShowNetworkList[key] + \"</div>\";\r\n";
  result += "  }\r\n";
  result += "  if (showNetwork == \"\") {\r\n";
  result += "    showNetwork = \"Geen WiFi signaal beschikbaar\"\r\n";
  result += "  }\r\n";
  result += "  else {\r\n";
  result += "    document.getElementById(\"selectedWiFiPassword\").style.display=\"block\"\r\n";
  result += "  }\r\n";
  result += "  document.getElementById(\"ssidList\").innerHTML = showNetwork;\r\n";
  result += "}\r\n"; 
  result += "function selectNetworkSSID(content) {\r\n";
  result += "  ssid = \"\"\r\n";
  result += "  if (content.name == \"networkSSID\") {\r\n";
  result += "    ssid = content.value;\r\n";
  result += "  }\r\n";
  result += "  document.getElementById(\"selectedWiFi\").innerHTML = ssid;\r\n";
  result += "}\r\n";
  result += "function loadWiFiNetworkList() {\r\n";
  result += "  var xhttp = new XMLHttpRequest();\r\n";
  result += "  xhttp.onreadystatechange = function() {\r\n";
  result += "    if (this.readyState == 4 && this.status == 200) {\r\n";
  result += "      showNetworkList(this.responseText);\r\n";
  result += "    }\r\n";
  result += "  };\r\n";
  result += "  xhttp.open(\"GET\", \"/networkssid/\", true);\r\n";
  result += "  xhttp.send();\r\n";
  result += "}\r\n";
  result += "</script>\r\n";
  result += "\r\n";
  result += "<script>\r\n";
  result += "function displayWiFiMode() {\r\n";
  result += "var ele = document.getElementsByName('wifi');\r\n";
  result += "\r\n";
  result += "  for(i = 0; i < ele.length; i++) {\r\n";
  result += "    if(ele[i].checked) {\r\n";
  result += "       document.getElementById(ele[i].value).style.display=\"block\";\r\n";
  result += "       if (ele[i].value == 'network') {\r\n";
  result += "        document.getElementById(\"selectedWiFi\").innerHTML = \"\"\r\n";
  result += "        document.getElementById(\"selectedWiFiPassword\").style.display=\"none\"\r\n";
  result += "         loadWiFiNetworkList();\r\n";
  result += "       }\r\n";
  result += "     }\r\n";
  result += "     else {\r\n";
  result += "       document.getElementById(ele[i].value).style.display=\"none\";\r\n";
  result += "     }\r\n";
  result += "  }\r\n";
  result += "}\r\n";
  result += "displayWiFiMode();\r\n";
  result += "</script>\r\n";
  result += "</body>\r\n";
  result += "</html>\r\n";
  
  server.sendHeader("Cache-Control", "no-cache");
  server.sendHeader("Connection", "keep-alive");
  server.sendHeader("Pragma", "no-cache");
  server.send(200, "text/html", result);
}
