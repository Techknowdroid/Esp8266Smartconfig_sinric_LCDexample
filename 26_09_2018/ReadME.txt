 Techknowdroid VERSION 26th September,2018 
 Using Smartconfig app (download from playstore ) on your mobile to set the SSID and password for the nodemcu to connect to.
 Also added infra red sensor and Remote control functionality. If wifi is down for whatever reason Alexa will not be able to control the lights.
 But you can still control the lights with a remote control, for such scenarios.
 The remote I have used is a Denon RC-981. When you press any button on the remote, the code below, displays the remote's Control code, on the Serial Monitor, for the button pressed. 
 This functionality can be used to get the button codes for any infra red remote which you have lying around & which you want to use for controlling the lights.
 Once you know the button codes you can change the CASE switch in the "REMOTE CONTROL VALUES decoded here below" section to use those codes to perform the task via remote.

 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////To be able to get the password from the nodemcu I Have modified the WiFiManager.h library as explained below/////////////////////////////////////////////////////////
///////// This page provided the help https://forum.arduino.cc/index.php?topic=469663.0  for the modifications, to help in extracting the password from the nodemcu///////////

 I've added these 2 lines in WiFiManager.h in the public section
 void          getSSID();
 void          getPassword();

 Have also modified the WiFiManager.cpp file thus (just uncommented the below lines which were already in the WiFiManager.cpp file).
  String WiFiManager::getSSID() {
  if (_ssid == "") {
    DEBUG_WM(F("Reading SSID"));
    _ssid = WiFi.SSID();
    DEBUG_WM(F("SSID: "));
    DEBUG_WM(_ssid);
  }
  return _ssid;
  }

  String WiFiManager::getPassword() {
  if (_pass == "") {
    DEBUG_WM(F("Reading Password"));
    _pass = WiFi.psk();
    DEBUG_WM("Password: " + _pass);
    //DEBUG_WM(_pass);
  }
  return _pass;
  }

To recover the password, the code used is as below :-
WiFiManager wifiManager;
Serial.print("Stored SSID: ");
Serial.println(wifiManager.getSSID());
Serial.print("Stored passphrase: ");
Serial.println(wifiManager.getPassword()); 

///////// This page provided the help https://forum.arduino.cc/index.php?topic=469663.0  for the above modifications ////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/
