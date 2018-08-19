# Esp8266Smartconfig_sinric_example. 
Use an Esp8266 with the Sinric API and Echo Dot to control switches(Relays).

The SSID and Password of the router is entered in the SmartConfig App instead of hardcoding into the ino file.

To download the SmartConfig App use the Google PlayStore for Android devices.

After loading the INO code into the Esp8266 (with the necessary Sinric API KEY and the various switch keys), open the SmartConfig app on your smart phone(android). Enter the SSID name to connect to and enter the password of that SSID (router). Then press CONFIRM . The app will connect your ESP8266 to this SSID (router). As simple as that.

Then ask Echo Dot to search for new devices and once the devices are found you are good to go.

A 20x4 LCD is connected to the esp8266 to show the status of events.
