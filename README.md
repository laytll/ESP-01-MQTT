# ESP-01-MQTT
Aruduino sketch for controlling ESP-01 single relay board

![image](https://user-images.githubusercontent.com/59602093/123157418-69b0a580-d462-11eb-9ab9-c0ef86a08eda.png)

This is a sketch for the cheap single relay boards that have an ESP-01. 
Not tested with ESP-01S but should work.

 ESP has Tx only enabled to free pins 2 & 3 for later use.
 Tx is only really used for checking WiFi or MQTT status
 It connects to an MQTT server then:
  - announces connection with unqiue ESP8266 chip ID
  - switches relay low
  - publishes heartbeat relay state on relay01/status
  - waits for a message 0 or 1 in relay01/set/request
  - confirms success in relay01/set/response
 
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
