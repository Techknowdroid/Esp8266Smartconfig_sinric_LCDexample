# Esp8266Smartconfig_sinric_LCDexample. 
Use an Esp8266(NodeMcu) with the Sinric API and Echo Dot to control switches(Relays).

The SSID and Password of the router is entered in the SmartConfig App instead of hardcoding into the ino file.

To download the SmartConfig App use the Google PlayStore for Android devices.

After loading the INO code into the NodeMcu(Esp8266) (with the necessary Sinric API KEY and the various switch keys), open the SmartConfig app on your smart phone(android). Enter the SSID name to connect to and enter the password of that SSID (router). Then press CONFIRM . The app will connect your (NodeMcu)ESP8266 to this SSID (router). As simple as that.

Then ask Echo Dot to search for new devices and once the devices are found you are good to go.

A 20x4 LCD is connected to the NodeMcu(esp8266) to show the status of events.
The Pin SCL on the LCD Panel goel to D1(GPIO 5) on the NodeMcu
The Pin SDA on the LCD Panel goel to D2(GPIO 4) on the NodeMcu
