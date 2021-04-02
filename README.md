# molen_sender

## Purpose
Collect data and send it to a server<br>

## How does it work
A sensor is connected to a microcontroller that collects pulses.<br>
When the microcontroller is connected to Internet, the pulses are sent to a server.<br>

## Guide
At start the microcontroller is configured as Access Point and will reveal itself with a SSID name starting with ESP-, followed by its MAC address.<br>
Use this SSID to connect using the default password administrator (see WiFi settings).<br>
With browsers that support mDNS you can open http://molen.local to connect. Otherwise you have to find the IP address (see No MDNS)<br>
In the browser a menu is shown where you should connect the microcontroller to your own WiFi network and thus upload data to the server.

## Menu

### Help

### Counter settings

### WiFi

## Hardware
The hardware consist of a board with a microcontroller, buttons LEDs and sensors<br>
LEDs will show the status.<br>
Green: microcontroller acts as an Access Point<br>
Yellow: microcontroller acts as a Station within your WiFi network<br>
Blue: the sensor has detected a pulse<br>
<br>
A small button is used to toggle between Access Point and Station.<br>
<br><br>
A very small button on the microcontroller itself is used to reset the microcontroller<br>
<br><br>
The sensors consist of a set of IR LEDs, mounted on a frame<br>

## Advanced
### Update firmware
In normal cases the microcontroller will look every hour for new firmware, but only when the sensors don't sense pulses.<br>
Beside that is it possible to press the 'Update Firmware' button to get the latest firmware.<br>
If you so so, you have to wait for about a minute and then reset the microcontroller.<br>

### No mDNS
With the button on board you can toggle between Access Point mode and Station mode.<br>
Step 1.<br>
In Access Point mode connect to your WiFi network using menu item WiFi->Network Station<br>
After saving the data, use the button on board to swith to Station mode.<br>
Step 2.<br>
After connecting to your WiFi network (Yellow LED) push the button on board again.<br>
The microcontroller will go to Access Point mode (Green LED).<br>
Now look in the browser, using http://192.68.4.1 and there will show up the last known IP address, given by your DHCP server.<br>
Step 3.
Push the botton on board again to go to Station mode.<br>
Using the last known IP address in your browser will give you the home page, but now within your own WiFi network
