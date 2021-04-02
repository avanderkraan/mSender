# molen_sender

## Purpose
Collect data and sent it to a server<br>

## How does it work
A sensor is connected to a microcontroller that collects pulses.<br>
When the microcontroller is connected to Internet, the pulses are sent to a server.<br>

## Guide
At start the microcontroller is configured as Access Point and will reveal itself with a SSID name starting with ESP-, followed by its MAC address.<br>
Use this SSID to connect and sign in with the username and password.<br>
After that a menu is shown where you sghould connect the microcontroller to your own WiFi network. That makes the microcontroller a Station within your local WiFi network.<br>
LEDs will show the status.<br>
Green: microcontroller acts as an Access Point<br>
Yellow: microcontroller acts as a Station within your WiFi network<br>
Blue: the sensor has detected a pulse<br>

A small button is used to toggle between Access Point and Station.<br>
