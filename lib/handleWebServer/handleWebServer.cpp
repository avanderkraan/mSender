#include "handleWebServer.h"

// menubuttons
const uint8_t WIFI = 0;
const uint8_t INFO = 1;
const char *buttons[] = {"_wifi", "_info"};

String getStyle()
{
  String result = "";
  result += "<style>\r\n";
  result += ".title {\r\n";
  result += "  font-size:1.5em;\r\n";
  result += "}\r\n";
  result += ".small {\r\n";
  result += "  font-size:0.5em;\r\n";
  result += "}\r\n";
  result += ".menu-header {\r\n";
  result += "  font-size:1.2em;\r\n";
  result += "  font-weight:700;\r\n";
  result += "}\r\n";
  result += ".nav {\r\n";
  result += "  list-style:none;\r\n";
  result += "}\r\n";
  result += ".nav-pills>li+li {\r\n";
  result += "  margin-left:2px;\r\n";
  result += "}\r\n";
  result += ".nav-pills>li {\r\n";
  result += "  float:left;\r\n";
  result += "}\r\n";
  result += ".nav-pills>li.active>a, .nav-pills>li.active>a:focus, .nav-pills>li.active>a:hover {\r\n";
  result += "  color: #fff;\r\n";
  result += "  background-color: #337ab7;\r\n";
  result += "}\r\n";
  result += ".nav-pills>li>a {\r\n";
  result += "  border-top-left-radius:4px;\r\n";
  result += "  border-top-right-radius:4px;\r\n";
  result += "  border-bottom-left-radius:4px;\r\n";
  result += "  border-bottom-right-radius:4px;\r\n";
  result += "}\r\n";
  result += ".nav>li {\r\n";
  result += "  position:relative;\r\n";
  result += "  display:block;\r\n";
  result += "}\r\n";
  result += ".nav>li>a {\r\n";
  result += "  position:relative;\r\n";
  result += "  display:block;\r\n";
  result += "  padding-top:10px;\r\n";
  result += "  padding-right:15px;\r\n";
  result += "  padding-bottom:10px;\r\n";
  result += "  padding-left:15px;\r\n";
  result += "}\r\n";
  result += "a {\r\n";
  result += "  text-decoration:none;\r\n";
  result += "  cursor:pointer\r\n";
  result += "}\r\n";
  result += "/* Smartphones ----------- */\r\n";
  result += "@media only screen and (max-width: 760px) {\r\n";
  result += ".nav>li>a {\r\n";
  result += "  position:relative;\r\n";
  result += "  display:block;\r\n";
  result += "  padding-top:8px;\r\n";
  result += "  padding-right:10px;\r\n";
  result += "  padding-bottom:8px;\r\n";
  result += "  padding-left:10px;\r\n";
  result += "}\r\n";
  result += "}\r\n";
  result += "</style>\r\n";

  return result;
}

String getHeaderPart(uint8_t button)
{
  String logo = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAABhGlDQ1BJQ0MgcHJvZmlsZQAAKJF9kT1Iw0AcxV9TpVUqClYQcchQnSyIijhqFYpQodQKrTqYXPoFTRqSFBdHwbXg4Mdi1cHFWVcHV0EQ/ABxdHJSdJES/5cUWsR4cNyPd/ced+8AoV5mqtkxDqiaZaTiMTGTXRUDrwhiAF3oQ0Bipj6XTCbgOb7u4ePrXZRneZ/7c/QoOZMBPpF4lumGRbxBPL1p6Zz3icOsKCnE58RjBl2Q+JHrsstvnAsOCzwzbKRT88RhYrHQxnIbs6KhEk8RRxRVo3wh47LCeYuzWq6y5j35C0M5bWWZ6zSHEccilpCECBlVlFCGhSitGikmUrQf8/APOf4kuWRylcDIsYAKVEiOH/wPfndr5icn3KRQDOh8se2PESCwCzRqtv19bNuNE8D/DFxpLX+lDsx8kl5raZEjoHcbuLhuafIecLkDDD7pkiE5kp+mkM8D72f0TVmg/xboXnN7a+7j9AFIU1eJG+DgEBgtUPa6x7uD7b39e6bZ3w8H+HJ89etzkgAAAAZiS0dEAP8A/wD/oL2nkwAAAAlwSFlzAAALEwAACxMBAJqcGAAAAAd0SU1FB+YIFQkQMU+zl7cAAAGwSURBVFjD7ZU9SwNBEIafRFS00vyBEFQEW0FEEARrOwUtk17RUrEwaewtbRQEf4GVjWhhaQI2ooEIdpr4UaSTnM0sLMveJZvsBosMbHH79T4zOzMHAxvYP7Ux4BB4BHIe7isAWy4HzoEq0AJee4QoAA3gGZjo9FAGqHiAyIt4HYiAdZfDGaAsEFEXEEq8IedL3YRvUiLx4ghhel7qJYFcIbyKmxDtcqLgI+xJEGUDImsRr4cQN6ujBuwAaU38O7S4WR1n2pt/AkvAG1DsR6dUOXErHu/JfI4+2r6Iq5Htl/ARMAvcGAAViUxQK4nYNfBlAPwIRCa0eF2SLjJGS0o0SCSUuGoyJxaAYBC65w2p+TWL+K80p0jathcIUzwv8wcxEZjzCWGGPa+tncraA3AsoY+ARekFPUMUYzxXpkpwG0gBH/K9qTUkBVGVzulUHXfGm5umSnAXGAHeLf8ABaEnZscQCwJgEx/V3rwJPGnfl8ZeHeLC9SmmYubnYxIwAu4t+3PAqs+esJEA0HS9LN0FwEzC2rjkRBCAIWAYmG6zb1n2pXyEW7/kKiH0trHSicAffrbMN6nraY8AAAAASUVORK5CYII=";
  String result = "<!DOCTYPE HTML>\r\n<html>\r\n";
  result += "<head>\r\n";
  result += "<meta charset=\"utf-8\">\r\n";
  result += "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\r\n";
  result += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n";
  result += "<!-- The above 3 meta tags *must* come first in the head; any other head content must come *after* these tags -->\r\n";
  result += "<link rel='icon' type='image/png' href='";
  result += logo;
  result += "'>\r\n";
  result += "<title>Molen Sensor</title>\r\n";

  result += getStyle();
  result += "</head>\r\n";
  result += "<body>\r\n";

  result += "<div class='header'>\r\n";
  result += "<span>\r\n";
  result += "<a href='/'><img src='";
  result += logo;
  result += "' width='30'></a>\r\n";
  result += "<span class=\"small\">© 2022 MAMI</span>\r\n";
  result += "</span>\r\n";
  result += "<span class='title'>Mill Sensor</span>\r\n";
  result += "</div>\r\n";

  result += "<div class='menu-button'>\r\n";
  result += "  <ul class='nav nav-pills' id='pills-tab'>\r\n";
//  result += "    <li class='nav-item _spin'>\r\n";
//  result += "      <a class='nav-link' id='pills-spin-tab' href='/spin/'>Mill</a>\r\n";
//  result += "    </li>\r\n";
  result += "    <li class='nav-item _wifi'>\r\n";
  result += "      <a class='nav-link' id='pills-wifi-tab' href='/wifi/'>Connect</a>\r\n";
  result += "    </li>\r\n";
  result += "    <li class='nav-item _info'>\r\n";
  result += "      <a class='nav-link' id='pills-info-tab' href='/info/'>Info</a>\r\n";
  result += "    </li>\r\n";
  result += "  </ul>\r\n";
  result += "</div>\r\n";

  result += "<br><br><hr>\r\n";
  String selectedButton = buttons[button];

  result.replace(selectedButton, "active");
  return result;
}

String getHeaderPart_nl(uint8_t button)
{
  String logo = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAABhGlDQ1BJQ0MgcHJvZmlsZQAAKJF9kT1Iw0AcxV9TpVUqClYQcchQnSyIijhqFYpQodQKrTqYXPoFTRqSFBdHwbXg4Mdi1cHFWVcHV0EQ/ABxdHJSdJES/5cUWsR4cNyPd/ced+8AoV5mqtkxDqiaZaTiMTGTXRUDrwhiAF3oQ0Bipj6XTCbgOb7u4ePrXZRneZ/7c/QoOZMBPpF4lumGRbxBPL1p6Zz3icOsKCnE58RjBl2Q+JHrsstvnAsOCzwzbKRT88RhYrHQxnIbs6KhEk8RRxRVo3wh47LCeYuzWq6y5j35C0M5bWWZ6zSHEccilpCECBlVlFCGhSitGikmUrQf8/APOf4kuWRylcDIsYAKVEiOH/wPfndr5icn3KRQDOh8se2PESCwCzRqtv19bNuNE8D/DFxpLX+lDsx8kl5raZEjoHcbuLhuafIecLkDDD7pkiE5kp+mkM8D72f0TVmg/xboXnN7a+7j9AFIU1eJG+DgEBgtUPa6x7uD7b39e6bZ3w8H+HJ89etzkgAAAAZiS0dEAP8A/wD/oL2nkwAAAAlwSFlzAAALEwAACxMBAJqcGAAAAAd0SU1FB+YIFQkQMU+zl7cAAAGwSURBVFjD7ZU9SwNBEIafRFS00vyBEFQEW0FEEARrOwUtk17RUrEwaewtbRQEf4GVjWhhaQI2ooEIdpr4UaSTnM0sLMveJZvsBosMbHH79T4zOzMHAxvYP7Ux4BB4BHIe7isAWy4HzoEq0AJee4QoAA3gGZjo9FAGqHiAyIt4HYiAdZfDGaAsEFEXEEq8IedL3YRvUiLx4ghhel7qJYFcIbyKmxDtcqLgI+xJEGUDImsRr4cQN6ujBuwAaU38O7S4WR1n2pt/AkvAG1DsR6dUOXErHu/JfI4+2r6Iq5Htl/ARMAvcGAAViUxQK4nYNfBlAPwIRCa0eF2SLjJGS0o0SCSUuGoyJxaAYBC65w2p+TWL+K80p0jathcIUzwv8wcxEZjzCWGGPa+tncraA3AsoY+ARekFPUMUYzxXpkpwG0gBH/K9qTUkBVGVzulUHXfGm5umSnAXGAHeLf8ABaEnZscQCwJgEx/V3rwJPGnfl8ZeHeLC9SmmYubnYxIwAu4t+3PAqs+esJEA0HS9LN0FwEzC2rjkRBCAIWAYmG6zb1n2pXyEW7/kKiH0trHSicAffrbMN6nraY8AAAAASUVORK5CYII=";
  String result = "<!DOCTYPE HTML>\r\n<html>\r\n";
  result += "<head>\r\n";
  result += "<meta charset=\"utf-8\">\r\n";
  result += "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\r\n";
  result += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n";
  result += "<!-- The above 3 meta tags *must* come first in the head; any other head content must come *after* these tags -->\r\n";
  result += "<link rel='icon' type='image/png' href='";
  result += logo;
  result += "'>\r\n";
  result += "<title>Molen sensor</title>\r\n";

  result += getStyle();
  result += "</head>\r\n";
  result += "<body>\r\n";

  result += "<div class='header'>\r\n";
  result += "<span>\r\n";
  result += "<a href='/'><img src='";
  result += logo;
  result += "' width='30'></a>\r\n";
  result += "<span class=\"small\">© 2022 MAMI</span>\r\n";
  result += "</span>\r\n";
  result += "<span class='title'>Molen Sensor</span>\r\n";
  result += "</div>\r\n";

  result += "<div class='menu-button'>\r\n";
  result += "  <ul class='nav nav-pills' id='pills-tab'>\r\n";
//  result += "    <li class='nav-item _spin'>\r\n";
//  result += "      <a class='nav-link' id='pills-spin-tab' href='/spin/'>Molen</a>\r\n";
//  result += "    </li>\r\n";
  result += "    <li class='nav-item _wifi'>\r\n";
  result += "      <a class='nav-link' id='pills-wifi-tab' href='/wifi/'>Connect</a>\r\n";
  result += "    </li>\r\n";
  result += "    <li class='nav-item _info'>\r\n";
  result += "      <a class='nav-link' id='pills-info-tab' href='/info/'>Info</a>\r\n";
  result += "    </li>\r\n";
  result += "  </ul>\r\n";
  result += "</div>\r\n";
  result += "<br><br><hr>\r\n";
  
  result.replace(buttons[button], "active");
  return result;
}

String getUpdatePart(Settings * pSettings)
{
  String result = "";

  result += "WiFi mode: ";
  if (pSettings->beginAsAccessPoint() == true)
  {
    result += "Access Point\r\n";
    result += "<br>\r\n";
    result += "(url: <a href='http://molen.local/' target='_blank'>molen.local</a> or <a href='http://192.168.4.1/' target='_blank'>http://192.168.4.1</a>)\r\n";
    result += "<br>\r\n";
    if (pSettings->getLastNetworkIP() == "Unknown")
    {
      result += "(last known Network address is unknown";
      result += ")\r\n";
    }
    else
    {
      result += "(last known Network address is: ";
      result += pSettings->getLastNetworkIP();
      result += ")\r\n";
    }
  }
  else
  {
    result += "Network Station\r\n";
    result += "<br>\r\n";
    if (pSettings->getLastNetworkIP() != "Unknown")
    {
      result += "(url: <a href='http://molen.local/' target='_blank'>molen.local</a> or via a local IP address, last known is: <a href='http://";
      result += pSettings->getLastNetworkIP();
      result += "/' target='_blank'>";
      result += pSettings->getLastNetworkIP();
      result += "</a>\r\n";
      result += ")\r\n";
    }
  }
  result += "<br><br><input id='restartButton' type='button' onclick='restart();' value='Restart'<br>\r\n";
  result += "<br><br>\r\n";

  result += "Version: <span id='version'></span>\r\n";
  result += " <input id='updateFirmwareButton' type='button' onclick='updateFirmware();' value='Update Firmware'<br>\r\n";
  result += " <div id=\"updateFirmwareMessage\"><div>\r\n";
  result += "<br><br>\r\n";
  result += "<script>\r\n";
  result += "  function restart() {\r\n";
  result += "    document.getElementById(\"restartButton\").disabled = true;\r\n";
  result += "    document.getElementById(\"updateFirmwareButton\").disabled = true;\r\n";
  result += "    document.getElementById(\"updateFirmwareMessage\").innerHTML = \"Please refresh this page after about 1 minute\";\r\n";
  result += "    var params = \"name=restart\";\r\n";
  result += "    sendUpdateFirmware(params, \"/restart/\");\r\n";
  result += "  };\r\n";
  result += "</script>\r\n";

  result += "<script>\r\n";
  result += "  document.getElementById(\"version\").innerHTML = \"";
  result += pSettings->getFirmwareVersion();
  result += "\";\r\n";
  result += "  function updateFirmware() {\r\n";
  result += "    document.getElementById(\"restartButton\").disabled = true;\r\n";
  result += "    document.getElementById(\"updateFirmwareButton\").disabled = true;\r\n";
  result += "    document.getElementById(\"updateFirmwareMessage\").innerHTML = \"Please refresh this page after about 1 minute\";\r\n";
  result += "    var params = \"name=update\";\r\n";
  result += "    sendUpdateFirmware(params, \"/update/\");\r\n";
  result += "  };\r\n";
  result += "</script>\r\n";

  result += "<script>\r\n";
  result += "  function sendUpdateFirmware(data, path) {\r\n";
  result += "    var xhr = new XMLHttpRequest();   // new HttpRequest instance\r\n";
  result += "    xhr.open(\"POST\", path);\r\n";
  result += "    xhr.setRequestHeader(\"Content-Type\", \"application/x-www-form-urlencoded; charset=UTF-8\");\r\n";
  result += "    document.getElementById(\"sendMessage\").innerHTML = \"Please wait\";\r\n";
  result += "    xhr.onreadystatechange = function() { // Call a function when the state changes\r\n";
  result += "     var myResponseText = \"\";\r\n";
  result += "      if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {\r\n";
  result += "        window.location.reload();\r\n";
  result += "     }\r\n";
  result += "      if (this.readyState === XMLHttpRequest.DONE && this.status !== 200) {\r\n";
  result += "       myResponseText = this.statusText || \"\";\r\n";
  result += "      }\r\n";
  result += "      document.getElementById(\"sendMessage\").innerHTML = myResponseText;\r\n";
  result += "    }\r\n";
  result += "    xhr.send(data);\r\n";
  result += "  }\r\n";

  result += "</script>\r\n";

  return result;
}

String getUpdatePart_nl(Settings * pSettings)
{
  String result = "";
  result += "WiFi modus: ";
  if (pSettings->beginAsAccessPoint() == true)
  {
    result += "Access Point\r\n";
    result += "<br>\r\n";
    result += "(url: <a href='http://molen.local/' target='_blank'>molen.local</a> of <a href='http://192.168.4.1/' target='_blank'>http://192.168.4.1</a>)\r\n";
    result += "<br>\r\n";
    if (pSettings->getLastNetworkIP() == "Unknown")
    {
      result += "(laatst bekende Netwerk adres is onbekend";
      result += ")\r\n";
    }
    else
    {
      result += "(laatst bekende Netwerk adres is: ";
      result += pSettings->getLastNetworkIP();
      result += ")\r\n";
    }
  }
  else
  {
    result += "Netwerk Station\r\n";
    result += "<br>\r\n";
    if (pSettings->getLastNetworkIP() != "Unknown")
    {
      result += "(url: <a href='http://molen.local/' target='_blank'>molen.local</a> of via een lokaal IP adres, laatst bekende adres is: <a href='http://";
      result += pSettings->getLastNetworkIP();
      result += "/' target='_blank'>";
      result += pSettings->getLastNetworkIP();
      result += "</a>\r\n";
      result += ")\r\n";
    }
  }
  result += "<br>\r\n";
  result += "<div id=\"sendMessage\"></div>\r\n";
  result += "<br><br>\r\n";

  result += "<input id='restartButton' type='button' onclick='restart();' value='Restart'<br>\r\n";
  result += "<br><br>\r\n";

  result += "Versie: <span id='version'></span>\r\n";
  result += " <input id='updateFirmwareButton' type='button' onclick='updateFirmware();' value='Update Firmware'<br>\r\n";
  result += " <div id=\"updateFirmwareMessage\"><div>\r\n";
  result += "<br><br>\r\n";

  result += "<script>\r\n";
  result += "  function restart() {\r\n";
  result += "    document.getElementById(\"restartButton\").disabled = true;\r\n";
  result += "    document.getElementById(\"updateFirmwareButton\").disabled = true;\r\n";
  result += "    document.getElementById(\"updateFirmwareMessage\").innerHTML = \"Please refresh this page after about 1 minute\";\r\n";
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
  result += "  function sendUpdateFirmware(data, path) {\r\n";
  result += "    var xhr = new XMLHttpRequest();   // new HttpRequest instance\r\n";
  result += "    xhr.open(\"POST\", path);\r\n";
  result += "    xhr.setRequestHeader(\"Content-Type\", \"application/x-www-form-urlencoded; charset=UTF-8\");\r\n";
  result += "    document.getElementById(\"sendMessage\").innerHTML = \"Even geduld\";\r\n";
  result += "    xhr.onreadystatechange = function() { // Call a function when the state changes\r\n";
  result += "     var myResponseText = \"\";\r\n";
  result += "      if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {\r\n";
  result += "        window.location.reload();\r\n";
  result += "     }\r\n";
  result += "      if (this.readyState === XMLHttpRequest.DONE && this.status !== 200) {\r\n";
  result += "       myResponseText = this.statusText || \"\";\r\n";
  result += "      }\r\n";
  result += "      document.getElementById(\"sendMessage\").innerHTML = myResponseText;\r\n";
  result += "    }\r\n";
  result += "    xhr.send(data);\r\n";
  result += "  }\r\n";

  result += "</script>\r\n";
  return result;
}

String getFooterPart(Settings * pSettings)
{
  String result = "<br>\r\n";
  result += "<div id=\"sendMessage\"></div>\r\n";
  
  result += "<br><hr>\r\n";
  result += "<select id='languagechoice' onchange='selectLanguage(this);'>\r\n";
  result += "  <option value='EN' id='EN'>English</option>\r\n";
  result += "  <option value='NL' id='NL'>Nederlands</option>\r\n";
  result += "</select>\r\n";

  result += "<script>\r\n";
  result += "  function selectLanguage(component) {\r\n";
  result += "    var params = \"name=help\" + \"&language=\" + component.value;\r\n";
  result += "    document.getElementById(\"NL\").removeAttribute('selected');\r\n";
  result += "    document.getElementById(\"EN\").removeAttribute('selected');\r\n";
  result += "    sendDataHome(params, \"/language/\");\r\n";
  result += "  }\r\n";
  result += "  function sendDataHome(data, path) {\r\n";
  result += "    var xhr = new XMLHttpRequest();   // new HttpRequest instance\r\n";
  result += "    xhr.open(\"POST\", path);\r\n";
  result += "    xhr.setRequestHeader(\"Content-Type\", \"application/x-www-form-urlencoded; charset=UTF-8\");\r\n";
  result += "    document.getElementById(\"sendMessage\").innerHTML = \"Please wait\";\r\n";
  result += "    xhr.onreadystatechange = function() { // Call a function when the state changes\r\n";
  result += "     var myResponseText = \"\";\r\n";
  result += "      if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {\r\n";
  result += "        window.location.reload();\r\n";
  result += "     }\r\n";
  result += "      if (this.readyState === XMLHttpRequest.DONE && this.status !== 200) {\r\n";
  result += "       myResponseText = this.statusText || \"\";\r\n";
  result += "      }\r\n";
  result += "      document.getElementById(\"sendMessage\").innerHTML = myResponseText;\r\n";
  result += "    }\r\n";
  result += "    xhr.send(data);\r\n";
  result += "  }\r\n";
  result += "document.getElementById('EN').setAttribute('selected', 'selected');\r\n";
  result += "</script>\r\n";
  result += "</body>\r\n";

  return result;
}

String getFooterPart_nl(Settings * pSettings)
{
  String result = "<br>\r\n";
  result += "<div id=\"sendMessage\"></div>\r\n";
  
  result += "<br><hr>\r\n";
  result += "<select id='languagechoice' onchange='selectLanguage(this);'>\r\n";
  result += "  <option value='EN' id='EN'>English</option>\r\n";
  result += "  <option value='NL' id='NL'>Nederlands</option>\r\n";
  result += "</select>\r\n";

  result += "<script>\r\n";
  result += "  function selectLanguage(component) {\r\n";
  result += "    var params = \"name=help\" + \"&language=\" + component.value;\r\n";
  result += "    document.getElementById(\"NL\").removeAttribute('selected');\r\n";
  result += "    document.getElementById(\"EN\").removeAttribute('selected');\r\n";
  result += "    sendDataHome(params, \"/language/\");\r\n";
  result += "  }\r\n";
  result += "  function sendDataHome(data, path) {\r\n";
  result += "    var xhr = new XMLHttpRequest();   // new HttpRequest instance\r\n";
  result += "    xhr.open(\"POST\", path);\r\n";
  result += "    xhr.setRequestHeader(\"Content-Type\", \"application/x-www-form-urlencoded; charset=UTF-8\");\r\n";
  result += "    document.getElementById(\"sendMessage\").innerHTML = \"Even geduld\";\r\n";
  result += "    xhr.onreadystatechange = function() { // Call a function when the state changes\r\n";
  result += "     var myResponseText = \"\";\r\n";
  result += "      if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {\r\n";
  result += "        window.location.reload();\r\n";
  result += "     }\r\n";
  result += "      if (this.readyState === XMLHttpRequest.DONE && this.status !== 200) {\r\n";
  result += "       myResponseText = this.statusText || \"\";\r\n";
  result += "      }\r\n";
  result += "      document.getElementById(\"sendMessage\").innerHTML = myResponseText;\r\n";
  result += "    }\r\n";
  result += "    xhr.send(data);\r\n";
  result += "  }\r\n";
  result += "document.getElementById('NL').setAttribute('selected', 'selected');\r\n";
  result += "</script>\r\n";
  result += "</body>\r\n";

  return result;
}

void info(ESP8266WebServer &server, Settings * pSettings, WiFiSettings * pWifiSettings) {
  String result = getHeaderPart(INFO);
  result += "<div class='menu-header'>\r\n";
  result += "Info about the mill sensor\r\n";
  result += "</div>\r\n";
  result += "<br><br><br>\r\n";

  result += getUpdatePart(pSettings);

  String myIP = "";
  String startStrong = "<strong>";
  String endStrong = "</strong>";
  if (WiFi.getMode() == WIFI_AP)
  {
    myIP = WiFi.softAPIP().toString();
    startStrong = "";
    endStrong = "";
  }
  if (WiFi.getMode() == WIFI_STA)
  {
    myIP = WiFi.localIP().toString();

    result += "With an update request the information below in bold will be sent to ";
    result += pSettings->getTargetServer();
    result += "<br>Passwords and IP addresses are not sent to the server<br>\r\n";
  }

  result += "\r\n<br>IP address: ";
  result += myIP;
  result += "\r\n<br>MAC address: ";
  result += WiFi.macAddress();

  result += startStrong;
  result += "\r\n<br>Firmware version: ";
  result += pSettings->getFirmwareVersion();
  result += endStrong;

  result += startStrong;
  result += "\r\n<br>Ratio: ";
  result += pSettings->getRatioArgument();
  result += endStrong;
 
  result += "\r\n<br>Access Point SSID: ";
  result += pWifiSettings->readAccessPointSSID();

  result += "\r\n<br>Network SSID: ";
  result += pWifiSettings->readNetworkSSID();

  result += "\r\n<br>Server name: ";
  result += pSettings->getTargetServer();

  result += "\r\n<br>Server port: ";
  result += String(pSettings->getTargetPort());

  result += "\r\n<br>Server path: ";
  result += pSettings->getTargetPath();

  result += getFooterPart(pSettings);

  result += "</html>\r\n";

  server.sendHeader("Cache-Control", "no-cache");
  server.sendHeader("Connection", "keep-alive");
  server.sendHeader("Pragma", "no-cache");
  server.send(200, "text/html", result); 
}

void wifi(ESP8266WebServer &server, Settings * pSettings, WiFiSettings * pWiFiSettings)
{
  String result = "";
  result += "<div class='menu-header'>\r\n";
  result += "WiFi settings for the mill sensor\r\n";
  result += "</div>\r\n";
  result += "<br><br><br>\r\n";
  result += "Choose AccessPoint to change the default password\r\n";
  result += "<br>\r\n";
  result += "Choose Network to connect the mill-sensor to your own WiFi\r\n";
  result += "<br><br>\r\n";
  result += "<input type=\"radio\" name=\"wifi\" onclick=\"displayWiFiMode();\" value=\"ap\">Access Point\r\n";
  result += "<br>\r\n";
  result += "<input type=\"radio\" name=\"wifi\" onclick=\"displayWiFiMode();\" value=\"network\">Network Station\r\n";
  result += "<br>\r\n";
  result += "<input type=\"radio\" name=\"wifi\" onclick=\"displayWiFiMode();\" value=\"erase\">Go to Erase-WiFi-Data menu\r\n";
  result += "<br>\r\n";
  result += "<br>\r\n";
  result += "<div id=\"ap\">\r\n";
  result += "  Clients can get access to this Access Point using the SSID and password entered below\r\n";
  result += "  <br>\r\n";
  result += "  An empty password will result in an unencrypted, open Access Point\r\n";
  result += "  <br>\r\n";
  result += "  SSID: ";
  if (pWiFiSettings->getAccessPointSSID() == "")
  {
    result += "ESP-" + WiFi.softAPmacAddress();
  }
  else
  {
    result += pWiFiSettings->getAccessPointSSID();
  }
  result += "  <br>\r\n";
  result += "  Password: <input type=\"password\" name=\"password\" maxlength=\"32\" size=\"33\" placeholder=\"";
  if (pWiFiSettings->getAccessPointPassword() == "")
  {
    result += "administrator";
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
  result += "  After 'Save' wait for confirmation\r\n";
  result += "  <br>\r\n";
  result += "  <input id=\"apButton\" type=\"button\" name=\"apButton\" value=\"Save\" onclick=\"saveAP(this);\">\r\n";
  result += "  <input type=\"button\" name=\"apCancelButton\" value=\"Cancel\" onclick=\"cancelWiFi();\">\r\n";
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
  result += "  After 'Save' wait for confirmation\r\n";
  result += "  <br>\r\n";
  result += "   <input id=\"networkButton\" type=\"button\" name=\"networkButton\" value=\"Save\" onclick=\"saveNetwork(this);\">\r\n";
  result += "   <input type=\"button\" name=\"networkCancelButton\" value=\"Cancel\" onclick=\"cancelWiFi();\">\r\n";
  result += "  <span id=\"networkPasswordMessage\"></span>\r\n";
  result += "</div>\r\n";
  result += "<div id=\"erase\">\r\n";
  result += "  !!!Warning!!!\r\n";
  result += "  <br><br>\r\n";
  result += "  A click on an Erase button will immediately erase saved data! Your current connection will stay alive\r\n";
  result += "  <br><br>\r\n";
  result += "Do NOT Erase and <input type=\"button\" name=\"networkButton\" value=\"Cancel\" onclick=\"cancelWiFi();\">\r\n";
  result += "  <br><br>\r\n";
  result += "  <input id=\"eraseAPData\" type=\"button\" name=\"eraseAPDataButton\" value=\"Erase\" onclick=\"eraseWiFiData(this);\"> Erase Access Point data; will result in an unsave, open Access Point\r\n";
  result += "  <br><br>\r\n";
  result += "  <input id=\"eraseNetworkData\" type=\"button\" name=\"eraseNetworkDataButton\" value=\"Erase\" onclick=\"eraseWiFiData(this);\"> Erase Network Station data\r\n";
  result += "  <br><br>\r\n";
  result += "  <input id=\"eraseWiFiData\" type=\"button\" name=\"eraseWiFiDataButton\" value=\"Erase\" onclick=\"eraseWiFiData(this);\"> Erase Access Point AND Network Station data; will result in an unsave, open Access Point\r\n";
  result += "</div>\r\n";
  result += "<script>\r\n";
  result += "function checkURIComponent(component, messageId, message) {\r\n";
  result += "  var invalidCharacterArray = [\" \"];\r\n";
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
//  result += "  document.getElementById(buttonId).disabled = !valid;\r\n";
  result += "   return valid;\r\n";
  result += "}\r\n";
  result += "  function cancelWiFi() {\r\n";
  result += "    window.location.reload();\r\n";
  result += "  }\r\n";
  result += "  function sendDataConnect(data) {\r\n";
  result += "    var xhr = new XMLHttpRequest();   // new HttpRequest instance\r\n";
  result += "    xhr.open(\"POST\", \"/wifiConnect/\");\r\n";
  result += "    xhr.setRequestHeader(\"Content-Type\", \"application/x-www-form-urlencoded; charset=UTF-8\");\r\n";
  result += "    document.getElementById(\"sendMessage\").innerHTML = \"Please wait\";\r\n";
  result += "    xhr.onreadystatechange = function() { // Call a function when the state changes\r\n";
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
  result += "    sendDataConnect(params);\r\n";
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
  result += "    sendDataConnect(params);\r\n";
  result += "  }\r\n";
  result += "  function eraseWiFiData(content) {\r\n";
  result += "  var params = \"name=erase\" + \"&target=\" + content.id;\r\n";
  result += "      sendDataConnect(params);\r\n";
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
  result += "    document.getElementById(\"selectedWiFiPassword\").style.display=\"block\";\r\n";
  result += "  }\r\n";
  result += "  document.getElementById(\"ssidList\").innerHTML = showNetwork;\r\n";
  result += "}\r\n"; 
  result += "function selectNetworkSSID(content) {\r\n";
  result += "  var ssid = \"\";\r\n";
  result += "  if (content.name == \"networkSSID\") {\r\n";
  result += "    ssid = content.value;\r\n";
  result += "  }\r\n";
  result += "  if (ssid != \"\"){\r\n";
  result += "    document.getElementById(\"selectedWiFi\").innerHTML = ssid;\r\n";
  result += "    document.getElementById(\"networkButton\").disabled = false;\r\n";
  result += "  }\r\n";
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
  result += "        document.getElementById(\"networkButton\").disabled = true;\r\n";
  result += "        document.getElementById(\"selectedWiFi\").innerHTML = \"\";\r\n";
  result += "        document.getElementById(\"selectedWiFiPassword\").style.display=\"none\";\r\n";
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
  
  server.sendHeader("Cache-Control", "no-cache");
  server.sendHeader("Connection", "keep-alive");
  server.sendHeader("Pragma", "no-cache");
  server.chunkedResponseModeStart(200, "text/html");
  server.sendContent(getHeaderPart(WIFI));
  server.sendContent(result);
  server.sendContent(getFooterPart(pSettings));
  server.sendContent("</html>\r\n");
  server.chunkedResponseFinalize();
  server.client().flush();
  server.client().stop();
}

///////////////// Vanaf hier Nederlands ///////////////////////////
void info_nl(ESP8266WebServer &server, Settings * pSettings, WiFiSettings * pWifiSettings) {
  String result = getHeaderPart_nl(INFO);
  result += "<div class='menu-header'>\r\n";
  result += "Informatie over de molen sensor\r\n";
  result += "</div>\r\n";
  result += "<br><br><br>\r\n";

  result += getUpdatePart_nl(pSettings);

  String myIP = "";
  String startStrong = "<strong>";
  String endStrong = "</strong>";
  if (WiFi.getMode() == WIFI_AP)
  {
    myIP = WiFi.softAPIP().toString();
  }
  if (WiFi.getMode() == WIFI_STA)
  {
    myIP = WiFi.localIP().toString();

    result += "Bij een update aanvraag wordt de dikgedrukte informatie die je hieronder ziet opgestuurd naar \r\n";
    result += pSettings->getTargetServer();     
    result += "<br>Wachtwoorden en IP adressen worden niet naar de server gestuurd<br>\r\n";

  }

  result += "\r\n<br>IP adres: ";
  result += myIP;
  result += "\r\n<br>MAC adres: ";
  result += WiFi.macAddress();

  result += startStrong;
  result += "\r\n<br>Firmware versie: ";
  result += pSettings->getFirmwareVersion();
  result += endStrong;

  result += startStrong;
  result += "\r\n<br>Ratio: ";
  result += pSettings->getRatioArgument();
  result += endStrong;

  result += "\r\n<br>Access Point SSID: ";
  result += pWifiSettings->readAccessPointSSID();

  result += "\r\n<br>Netwerk SSID: ";
  result += pWifiSettings->readNetworkSSID();

  result += "\r\n<br>Server naam: ";
  result += pSettings->getTargetServer();

  result += "\r\n<br>Server poort: ";
  result += String(pSettings->getTargetPort());

  result += "\r\n<br>Server pad: ";
  result += pSettings->getTargetPath();

  result += getFooterPart_nl(pSettings);

  result += "</html>\r\n";


  server.sendHeader("Cache-Control", "no-cache");
  server.sendHeader("Connection", "keep-alive");
  server.sendHeader("Pragma", "no-cache");
  server.send(200, "text/html", result); 
}

void wifi_nl(ESP8266WebServer &server, Settings * pSettings, WiFiSettings * pWiFiSettings)
{
//  String result = getHeaderPart_nl(WIFI);
  String result = "";
  result += "<div class='menu-header'>\r\n";
  result += "WiFi instellingen voor de molen sensor\r\n";
  result += "</div>\r\n";
  result += "<br><br><br>\r\n";
  result += "Kies AccessPoint om het standaard wachtwoord te wijzigen\r\n";
  result += "<br>\r\n";
  result += "Kies Netwerk Station om de molen sensor te koppelen aan je eigen WiFi\r\n";
  result += "<br><br>\r\n";
  result += "<input type=\"radio\" name=\"wifi\" onclick=\"displayWiFiMode();\" value=\"ap\">Access Point\r\n";
  result += "<br>\r\n";
  result += "<input type=\"radio\" name=\"wifi\" onclick=\"displayWiFiMode();\" value=\"network\">Netwerk Station\r\n";
  result += "<br>\r\n";
  result += "<input type=\"radio\" name=\"wifi\" onclick=\"displayWiFiMode();\" value=\"erase\">Ga naar Verwijder WiFi-gegevens menu\r\n";
  result += "<br>\r\n";
  result += "<br>\r\n";
  result += "<div id=\"ap\">\r\n";
  result += "  Apparaten kunnen toegang krijgen tot dit Access Point met het hieronder ingevulde SSID en wachtwoord\r\n";
  result += "  <br>\r\n";
  result += "  Een niet-ingevuld wachtwoord geeft een onveilige, open Access Point\r\n";
  result += "  <br>\r\n";
  result += "  SSID: ";
  if (pWiFiSettings->getAccessPointSSID() == "")
  {
    result += "ESP-" + WiFi.softAPmacAddress();
  }
  else
  {
    result += pWiFiSettings->getAccessPointSSID();
  }
  result += "  <br>\r\n";
  result += "  Wachtwoord: <input type=\"password\" name=\"password\" maxlength=\"32\" size=\"33\" placeholder=\"";
  if (pWiFiSettings->getAccessPointPassword() == "")
  {
    result += "administrator";
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
  result += "  Na 'Save' even wachten op bevestiging\r\n";
  result += "  <br>\r\n";
  result += "  <input id=\"apButton\" type=\"button\" name=\"apButton\" value=\"Save\" onclick=\"saveAP(this);\">\r\n";
  result += "  <input type=\"button\" name=\"apCancelButton\" value=\"Cancel\" onclick=\"cancelWiFi();\">\r\n";
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
  result += "  Na 'Save' even wachten op bevestiging\r\n";
  result += "  <br>\r\n";
  result += "   <input id=\"networkButton\" type=\"button\" name=\"networkButton\" value=\"Save\" onclick=\"saveNetwork(this);\">\r\n";
  result += "   <input type=\"button\" name=\"networkCancelButton\" value=\"Cancel\" onclick=\"cancelWiFi();\">\r\n";
  result += "  <span id=\"networkPasswordMessage\"></span>\r\n";
  result += "</div>\r\n";
  result += "<div id=\"erase\">\r\n";
  result += "  !!!Opgelet!!!\r\n";
  result += "  <br><br>\r\n";
  result += "  Een klik op de 'Erase' knop zal onmiddellijk de opgeslagen gegevens verwijderen! De huidige verbinding blijft bestaan\r\n";
  result += "  <br><br>\r\n";
  result += "Verwijder NIETS en <input type=\"button\" name=\"networkButton\" value=\"Cancel\" onclick=\"cancelWiFi();\">\r\n";
  result += "  <br><br>\r\n";
  result += "  <input id=\"eraseAPData\" type=\"button\" name=\"eraseAPDataButton\" value=\"Erase\" onclick=\"eraseWiFiData(this);\"> Verwijder Access Point gegevens; resulteert in een onveilig, open Access Point\r\n";
  result += "  <br><br>\r\n";
  result += "  <input id=\"eraseNetworkData\" type=\"button\" name=\"eraseNetworkDataButton\" value=\"Erase\" onclick=\"eraseWiFiData(this);\"> Verwijder Netwerk Station gegevens\r\n";
  result += "  <br><br>\r\n";
  result += "  <input id=\"eraseWiFiData\" type=\"button\" name=\"eraseWiFiDataButton\" value=\"Erase\" onclick=\"eraseWiFiData(this);\"> Verwijder Access Point EN Netwerk Station gegevens; resulteert in een onveilig, open Access Point\r\n";
  result += "</div>\r\n";
  result += "<script>\r\n";
  result += "function checkURIComponent(component, messageId, message) {\r\n";
  result += "  var invalidCharacterArray = [\" \"];\r\n";
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
//  result += "  document.getElementById(buttonId).disabled = !valid;\r\n";
  result += "   return valid;\r\n";
  result += "}\r\n";
  result += "  function cancelWiFi() {\r\n";
  result += "    window.location.reload();\r\n";
  result += "  }\r\n";
  result += "  function sendDataConnect(data) {\r\n";
  result += "    var xhr = new XMLHttpRequest();   // new HttpRequest instance\r\n";
  result += "    xhr.open(\"POST\", \"/wifiConnect/\");\r\n";
  result += "    xhr.setRequestHeader(\"Content-Type\", \"application/x-www-form-urlencoded; charset=UTF-8\");\r\n";
  result += "    document.getElementById(\"sendMessage\").innerHTML = \"Even geduld\";\r\n";
  result += "    xhr.onreadystatechange = function() { // Call a function when the state changes\r\n";
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
  result += "    sendDataConnect(params);\r\n";
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
  result += "    sendDataConnect(params);\r\n";
  result += "  }\r\n";
  result += "  function eraseWiFiData(content) {\r\n";
  result += "  var params = \"name=erase\" + \"&target=\" + content.id;\r\n";
  result += "      sendDataConnect(params);\r\n";
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
  result += "    document.getElementById(\"selectedWiFiPassword\").style.display=\"block\";\r\n";
  result += "  }\r\n";
  result += "  document.getElementById(\"ssidList\").innerHTML = showNetwork;\r\n";
  result += "}\r\n"; 
  result += "function selectNetworkSSID(content) {\r\n";
  result += "  var ssid = \"\";\r\n";
  result += "  if (content.name == \"networkSSID\") {\r\n";
  result += "    ssid = content.value;\r\n";
  result += "  }\r\n";
  result += "  if (ssid != \"\"){\r\n";
  result += "    document.getElementById(\"selectedWiFi\").innerHTML = ssid;\r\n";
  result += "    document.getElementById(\"networkButton\").disabled = false;\r\n";
  result += "  }\r\n";
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
  result += "        document.getElementById(\"networkButton\").disabled = true;\r\n";
  result += "        document.getElementById(\"selectedWiFi\").innerHTML = \"\";\r\n";
  result += "        document.getElementById(\"selectedWiFiPassword\").style.display=\"none\";\r\n";
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
  
  server.sendHeader("Cache-Control", "no-cache");
  server.sendHeader("Connection", "keep-alive");
  server.sendHeader("Pragma", "no-cache");
  server.chunkedResponseModeStart(200, "text/html");
  server.sendContent(getHeaderPart_nl(WIFI));
  server.sendContent(result);
  server.sendContent(getFooterPart_nl(pSettings));
  server.sendContent("</html>\r\n");
  server.chunkedResponseFinalize();
  server.client().flush();
  server.client().stop();
}
