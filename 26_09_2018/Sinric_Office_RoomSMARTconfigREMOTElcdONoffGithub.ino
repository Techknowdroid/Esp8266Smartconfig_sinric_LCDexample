/*
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

// Using Smartconfig app on mobile instead of hardcoded SSID and Password.
// Using LCD 20x4.
// The Pin SCL on the LCD Panel goes to D1(GPIO 5) on the NodeMcu The Pin SDA on the LCD Panel goes to D2(GPIO 4) on the NodeMcu

//////////////////// SINRIC and ESP8266 libraries////////////////////////////////////////////////////////////
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h> //  get it from https://github.com/Links2004/arduinoWebSockets/releases 
#include <ArduinoJson.h> // get it from https://arduinojson.org/ or install via Arduino library manager
#include <StreamString.h>

////////////SmartConfig Libraries/////////////////////////////////////////////////////////////////////////////
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <functional>
#include "switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"

///////// New SmartConfig Libraries from ESP8266 Page : http://bbhh.dlinkddns.com/index.php/2017/09/13/esp8266-smartconfig/
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiManager.h>
//////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////REMOTE CONTROL variables
#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRutils.h>
#include <IRrecv.h>
// an IR detector/demodulator is connected to D7
uint16_t RECV_PIN = D7;
IRrecv irrecv(RECV_PIN);
decode_results results;


// prototypes
boolean connectWifi();
boolean wifiConnected = false;


#include <LiquidCrystal_I2C.h>  // get from https://www.arduinolibraries.info/libraries/liquid-crystal-i2-c
// The Pin SCL on the LCD Panel goes to D1(GPIO 5) on the NodeMcu The Pin SDA on the LCD Panel goes to D2(GPIO 4) on the NodeMcu
#include <Wire.h>



LiquidCrystal_I2C lcd(0x27, 20, 4);


/***********************************************************************
   MILLIS timer for read sensor and display on LCD screen
 ***********************************************************************/

unsigned long interval15sec=15000; // the time we need to wait BEFORE the LCD Backlight is turmed Off.
unsigned long time_15sec = 0; // millis() returns an unsigned long.
int light=1;             // the current reading from the LCD backlight
char Str1[400];
  
ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;


//UpnpBroadcastResponder upnpBroadcastResponder;

#define MyApiKey "                " // TODO: Change to your sinric API Key. Your API Key is displayed on sinric.com dashboard
//#define MySSID "          " // TODO: Change to your Wifi network SSID
//#define MyWifiPassword "     " // TODO: Change to your Wifi network password
#define MYPIN1 D3    //GPIO 
#define MYPIN2 D4    //GPIO 
#define MYPIN3 D5    //GPIO 
#define MYPIN4 D6    //GPIO 
//#define MYPIN5 D8  //GPIO 

#define HEARTBEAT_INTERVAL 300000 // 5 Minutes 

uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

void setPowerStateOnServer(String deviceId, String value);
void setTargetTemperatureOnServer(String deviceId, String value, String scale);

// deviceId is the ID assgined to your smart-home-device in sinric.com dashboard. Copy it from dashboard and paste it here

void turnOn(String deviceId) {
  if (deviceId == "5b632822a98fea5737c2c941") // Device ID of MYPIN1 which is Light 13 on the sinric webpage https://sinric.com/
  {  
    Serial.println("");
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    Serial.println(" Switch D3");
    pinMode(MYPIN1, HIGH);
    //lcd.noBacklight();
    lcd.backlight();
    Serial.println("Backlight ON for 15 secs."); 
        light=0;
        time_15sec = millis(); 
    lcd.setCursor(0, 0);
    lcd.print("Light  13: On     ");
    //delay(5000);
    //lcd.noBacklight();
  
  } 
  else if (deviceId == "5b6328d1a98fea5737c2c942") // Device ID of MYPIN2 which is Light 14 on the sinric webpage https://sinric.com/
  { 
    Serial.println("");
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    Serial.println(" Switch D4");
    pinMode(MYPIN2, HIGH);
    lcd.backlight();
    Serial.println("Backlight ON for 15 secs."); 
        light=0;
        time_15sec = millis(); 
    lcd.setCursor(0, 1);
    lcd.print("Light  14: On      ");
    //delay(5000);
   // lcd.noBacklight();

  }
   else if (deviceId == "5b6328e7a98fea5737c2c943") // Device ID of MYPIN3 which is Light 15 on the sinric webpage https://sinric.com/
  { 
    Serial.println("");
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    Serial.println(" Switch D5");
    pinMode(MYPIN3, HIGH);
    lcd.backlight();
    Serial.println("Backlight ON for 15 secs."); 
        light=0;
        time_15sec = millis(); 
    lcd.setCursor(0, 2);
    lcd.print("Light  15: On      ");
   // delay(5000);
    //lcd.noBacklight();

  }
   else if (deviceId == "5b632910a98fea5737c2c944") // Device ID of MYPIN4 which is Light 16 on the sinric webpage https://sinric.com/
  { 
    Serial.println("");
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    Serial.println(" Switch D6");
    pinMode(MYPIN4, HIGH);
    lcd.backlight();
    Serial.println("Backlight ON for 15 secs."); 
        light=0;
        time_15sec = millis(); 
    lcd.setCursor(0, 3);
    lcd.print("Light  16: On      ");
    //delay(5000);
    //lcd.noBacklight();

  }
   /*else if (deviceId == "5b63291ea98fea5737c2c945") // Device ID of MYPIN5 which is Light 17 on the sinric webpage https://sinric.com/
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    Serial.print(" Switch D7");
    pinMode(MYPIN5, HIGH);
   // lcd.noBacklight();
    lcd.backlight();
    lcd.setCursor(0, 4);
    lcd.print("Light  17: On      ");
   // delay(5000);
    //lcd.noBacklight();

  }
  */
   
  else {
    Serial.print("Turn on for unknown device id: ");
    Serial.println(deviceId);    
  }     
}

void turnOff(String deviceId) {
   if (deviceId == "5b632822a98fea5737c2c941") // Device ID of MYPIN1 Device ID of MYPIN1 which is Light 13 on the sinric webpage https://sinric.com/
   {  
     Serial.println("");
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     Serial.println(" Switch D3");
     pinMode(MYPIN1, LOW);
     lcd.backlight();
     Serial.println("Backlight ON for 15 secs."); 
        light=0;
         time_15sec = millis(); 
     lcd.setCursor(0, 0);
     lcd.print("Light  13: Off     ");
    // delay(5000);
    // lcd.noBacklight();
   }
   else if (deviceId == "5b6328d1a98fea5737c2c942") // Device ID of MYPIN2 which is Light 14 on the sinric webpage https://sinric.com/
   { 
     Serial.println("");
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     Serial.println(" Switch D4");
     pinMode(MYPIN2, LOW);
     lcd.backlight();
     Serial.println("Backlight ON for 15 secs."); 
        light=0;
        time_15sec = millis(); 
     lcd.setCursor(0, 1);
     lcd.print("Light  14: Off     ");
     //delay(5000);
     //lcd.noBacklight();

  }
   else if (deviceId == "5b6328e7a98fea5737c2c943") // Device ID of MYPIN3 which is Light 15 on the sinric webpage https://sinric.com/
   { 
     Serial.println("");
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     Serial.println(" Switch D5");
     pinMode(MYPIN3, LOW);
     lcd.backlight();
     Serial.println("Backlight ON for 15 secs."); 
        light=0;
         time_15sec = millis(); 
     lcd.setCursor(0, 2);
     lcd.print("Light  15: Off     ");
     //delay(5000);
     //lcd.noBacklight();

  }
   else if (deviceId == "5b632910a98fea5737c2c944") // Device ID of MYPIN4 which is Light 16 on the sinric webpage https://sinric.com/
   { 
     Serial.println("");
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     Serial.println(" Switch D6");
     pinMode(MYPIN4, LOW);
     lcd.backlight();
     Serial.println("Backlight ON for 15 secs."); 
        light=0;
         time_15sec = millis(); 
     lcd.setCursor(0, 3);
     lcd.print("Light  16: Off     ");
     //delay(5000);
     //lcd.noBacklight();

  }
  /* else if (deviceId == "5b63291ea98fea5737c2c945") // Device ID of MYPIN5 which is Light 17 on the sinric webpage https://sinric.com/
   { 
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     Serial.print(" Switch D7");
     pinMode(MYPIN5, LOW);
     //lcd.noBacklight();
     lcd.backlight();
     lcd.setCursor(0, 4);
     lcd.print("Light  17: Off     ");
    // delay(5000);
    // lcd.noBacklight();

  }
  */
  
  else {
     Serial.print("Turn off for unknown device id: ");
     Serial.println(deviceId);    
  }
}
/////////////////////// SINRIC CODE for WEBSOCKET///////////////////////////////////////////////////////
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      isConnected = false;    
      Serial.printf("[WSc] Webservice disconnected from sinric.com!\n");
      break;
    case WStype_CONNECTED: {
      isConnected = true;
      Serial.printf("[WSc] Service connected to sinric.com at url: %s\n", payload);
      Serial.printf("Waiting for commands from sinric.com ...\n");
    //Create system initialized message
    lcd.clear();
    lcd.setCursor(0, 0);
    delay(1000);
    lcd.print("Sinric ONLINE:");
    delay(1000);
    lcd.setCursor(0, 2);
    lcd.print("Give Alexa Commands");
    delay(3000);
    //lcd.clear();   
    lcd.noBacklight();
    }
      break;
    case WStype_TEXT: {
        Serial.printf("[WSc] get text: %s\n", payload);
        // Example payloads

        // For Switch or Light device types
        // {"deviceId": xxxx, "action": "setPowerState", value: "ON"} // https://developer.amazon.com/docs/device-apis/alexa-powercontroller.html

        // For Light device type
        // Look at the light example in github
          
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject((char*)payload); 
        String deviceId = json ["deviceId"];     
        String action = json ["action"];
        
        if(action == "setPowerState") { // Switch or Light
            String value = json ["value"];
            if(value == "ON") {
                turnOn(deviceId);
            } else {
                turnOff(deviceId);
            }
        }
        else if (action == "SetTargetTemperature") {
            String deviceId = json ["deviceId"];     
            String action = json ["action"];
            String value = json ["value"];
        }
        else if (action == "test") {
            Serial.println("[WSc] received test command from sinric.com");
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
  }
}
/////////////Sinric Code ENDS for WEBSOCKET////////////////////////////////////////////////////////////////////////////////


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

 // Start the inra red receiver
 //Serial.begin(9600);
  irrecv.enableIRIn();  // Start the infra red receiver 

  
 //Initalize LCD
  lcd.init();
  //lcd.noBacklight();
  lcd.backlight();
  lcd.begin(4, 20);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Open SMARTconfig");
  lcd.setCursor(0, 2);
  lcd.print("on your phone..");
  //delay(5000);

 ////////////////CODE for ESPTouch Smartconfig begins here : http://bbhh.dlinkddns.com/index.php/2017/09/13/esp8266-smartconfig/

// setup WiFi
    // no need to config SSID and password
    // turn on Auto Connect
    WiFi.mode(WIFI_STA);
        if(WiFi.getAutoConnect() == false){
        WiFi.setAutoConnect(true);
    }
    WiFi.setAutoReconnect(true);
 
    // start WiFi connecting
    int cnt = 0;
    
    ////////////////////// Uncomment and run below code once, if you want to erase all the stored SSID & Passwords///
    //reset saved settings
     //WiFiManager wifiManager;
     //wifiManager.resetSettings();
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    
    Serial.println("ESP8266 is trying to connect to a known WiFi.");
    while (WiFi.status() != WL_CONNECTED) {
        // onboard LED flash twice quickly
        // UART output "..." 
        // ESP8266 is trying to connect to a known WiFi
        delay(1000);
        Serial.println(".");
        // if no connection available after timeout
        if (cnt++ >= 10) {
            Serial.println("");
            WiFi.beginSmartConfig();
            while (1) {
                // ESP8266 is in smartconfig loop
                // use app to finish the smartconfig
                //status_blink();
                
                /* Wait for SmartConfig packet from mobile */
                Serial.println("Waiting for SmartConfig.");
                while (!WiFi.smartConfigDone()) {
                delay(500);
                Serial.print(".");
                }
                Serial.println("");
                Serial.println("SmartConfig done.");
                 /* Wait for WiFi to connect to AP */
                Serial.println("Waiting for WiFi");
                while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                Serial.print(".");
                }
                Serial.println("WiFi Connected.");
                Serial.print("IP Address: ");
                Serial.println(WiFi.localIP());
                delay(500);
                
                if (WiFi.smartConfigDone()) {
                    Serial.println();
                    Serial.println(F("[WIFI] SmartConfig: Success"));
                    break;
                }
                Serial.print(F("|"));
            }
        }
        //BUILTIN_LED_OFF();
    }
 
    Serial.println("");
    Serial.println(F("WiFi connected"));  
    Serial.print(F("IP address: "));
    Serial.println(WiFi.localIP());
 
    WiFi.printDiag(Serial);
    //WiFi.printDiag(lcd);
    //delay(10000);
 
    Serial.print(F("RSSI: "));
    Serial.println(WiFi.RSSI());
    Serial.print(F("BSSID: "));
    Serial.println(WiFi.BSSIDstr());

    //Serial.println(Str1);

    WiFiManager wifiManager;
    Serial.print("Stored SSID: ");
    Serial.println(wifiManager.getSSID());
    Serial.print("Stored passphrase: ");
    Serial.println(wifiManager.getPassword()); 
    delay(10000);

//////////Code for ESPTouch Smartconfig ends here////////////////////////////////////////////////////////////

 
    
    //if you get here you have connected to the WiFi
   Serial.println("connected...YAY!!)");  


   
  
  
  // SINRIC CODE starts from here.
  //Initalize LCD
 
  Serial.println("Waiting for WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  
  }
    Serial.println("WiFi Connected.");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi SSID : ");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.SSID());
    lcd.setCursor(0, 2);
    lcd.print("Password is : ");
    lcd.setCursor(0, 3);
    lcd.print(wifiManager.getPassword());
    delay(9000);
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.println("WiFi Connected to SSID...  ");
    Serial.println(WiFi.SSID());
    Serial.println("WiFi Password...is *NOT* DISPLAYED.");
    Serial.println(wifiManager.getPassword());

    //Create Polling Message
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  Getting Status   ");
    lcd.setCursor(0, 2);
    lcd.print("  Of Smart Devices  ");
    delay(4000);

  // server address, port and URL
  webSocket.begin("iot.sinric.com", 80, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("apikey", MyApiKey);
  
  // try again every 5000ms if connection has failed
  webSocket.setReconnectInterval(5000);   // If you see 'class WebSocketsClient' has no member named 'setReconnectInterval' error update arduinoWebSockets
}



//////////// VOID LOOP Starts here

void loop() {
  webSocket.loop();
  
  if(isConnected) {
      uint64_t now = millis();
      
      // Send heartbeat in order to avoid disconnections during ISP resetting IPs over night. Thanks @MacSass
      if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
          heartbeatTimestamp = now;
          webSocket.sendTXT("H");          
      }
  }   

   unsigned long currentMillis = millis(); // grab current time
 
 // check if "interval" time has passed (20000 milliseconds)
 if ((unsigned long)(currentMillis - time_15sec) >= interval15sec){
  
   if (light==0)
   {
    Serial.println("");
    Serial.print("light = 0 "); 
    Serial.println("So Backlight off."); 
    lcd.noBacklight(); // switch on LCD backlight
   }
     // save the "current" time
     time_15sec = millis(); 
   } // Don't execute this loop till 15 seconds have elapsed.
  
  
  
  

      
////////////////// REMOTE CONTROL VALUES decoded here below /////////////////////////////////
///////////////////REMOTE   REMOTE   REMOTE//////////////////////////////////////////////////

  if (irrecv.decode(&results)) 
  {
       // Serial.println(results.value, HEX);    //showing decoded data on serial terminal
       if (irrecv.decode(&results))
       {
         // print() & println() can't handle printing long longs. (uint64_t)
         Serial.println("");
         Serial.print("Remote sends the code = ");
         serialPrintUint64(results.value, HEX);
         Serial.println("");
       }
                  
      switch(results.value)
      {
        case 0xAF5B04F:   //  is Hex value. Use this converter https://www.binaryhexconverter.com/hex-to-decimal-converter
        Serial.println(F(""));
        Serial.println(F("This is 1 on the REMOTE "));
        Serial.println(F("So Light 13 is switched ON ")); //
        turnOn("5b632822a98fea5737c2c941");
        pinMode(MYPIN1, HIGH);
        lcd.backlight();
        Serial.println("Backlight ON for 15 secs."); 
        light=0;
        time_15sec = millis(); 
        //lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Light  13: On      ");
        //delay(1000);
        // lcd.noBacklight();
        break; // Button 1 on remote ends
        
        case 0xAF5708F:     //  is Hex value. Use this converter https://www.binaryhexconverter.com/hex-to-decimal-converter
        Serial.println(F(""));
        Serial.println(F("This is 2 On the REMOTE "));
        Serial.println(F("So Light 13 is switched OFF "));
        pinMode(MYPIN1, LOW);
        turnOff("5b632822a98fea5737c2c941");
        lcd.backlight();
        Serial.println("Backlight ON for 15 secs."); 
        light=0;
        time_15sec = millis(); 
        //lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Light  13: Off     ");
        // delay(1000);
        //lcd.noBacklight();
        break; // Button 2 on remote ends

        case 0xAF538C7:   //  is Hex value. Use this converter https://www.binaryhexconverter.com/hex-to-decimal-converter
        Serial.println(F(""));
        Serial.println(F("This is 4 on the REMOTE "));
        Serial.println(F("So Light 14 is switched ON ")); // 
        pinMode(MYPIN2, HIGH);
        turnOn("5b6328d1a98fea5737c2c942");
        lcd.backlight();
        Serial.println("Backlight ON for 15 secs."); 
        light=0;
        time_15sec = millis(); 
        //lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("Light  14: On      ");
        //delay(1000);
        // lcd.noBacklight();
        break; // Button 1 on remote ends
        
        case 0xAF5B847:     //  is Hex value. Use this converter https://www.binaryhexconverter.com/hex-to-decimal-converter
        Serial.println(F(""));
        Serial.println(F("This is 5 On the REMOTE "));
        Serial.println(F("So Light 14 is switched OFF "));
        pinMode(MYPIN2, LOW);
         turnOff("5b6328d1a98fea5737c2c942");
        lcd.backlight();
        Serial.println("Backlight ON for 15 secs."); 
        light=0;
        time_15sec = millis(); 
        //lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("Light  14: Off     ");
        // delay(1000);
        //lcd.noBacklight();
        break; // Button 2 on remote ends

        case 0xAF5F807:   //  is Hex value. Use this converter https://www.binaryhexconverter.com/hex-to-decimal-converter
        Serial.println(F(""));
        Serial.println(F("This is 7 on the REMOTE "));
        Serial.println(F("So Light 15 is switched ON ")); // 
        pinMode(MYPIN3, HIGH);
        turnOn("5b6328e7a98fea5737c2c943");
        lcd.backlight();
        Serial.println("Backlight ON for 15 secs."); 
        light=0;
        time_15sec = millis(); 
        //lcd.clear();
        lcd.setCursor(0, 2);
        lcd.print("Light  15: On      ");
        //delay(1000);
        // lcd.noBacklight();
        break; // Button 1 on remote ends
        
        case 0xAF520DF:     //  is Hex value. Use this converter https://www.binaryhexconverter.com/hex-to-decimal-converter
        Serial.println(F(""));
        Serial.println(F("This is 8 On the REMOTE "));
        Serial.println(F("So Light 15 is switched OFF "));
        pinMode(MYPIN3, LOW);
        turnOff("5b6328e7a98fea5737c2c943");
        lcd.backlight();
        Serial.println("Backlight ON for 15 secs."); 
        light=0;
        time_15sec = millis();  
        //lcd.clear();
        lcd.setCursor(0, 2);
        lcd.print("Light  15: Off     ");
        // delay(1000);
        //lcd.noBacklight();
        break; // Button 2 on remote ends

         case 0xAF548B7:   //  is Hex value. Use this converter https://www.binaryhexconverter.com/hex-to-decimal-converter
        Serial.println(F(""));
        Serial.println(F("This is VOL + on the REMOTE "));
        Serial.println(F("So Light 16 is switched ON ")); // 
        pinMode(MYPIN4, HIGH);
        turnOn("5b632910a98fea5737c2c944");
        lcd.backlight();
        Serial.println("Backlight ON for 15 secs."); 
        light=0;
        time_15sec = millis(); 
        //lcd.clear();
        lcd.setCursor(0, 3);
        lcd.print("Light  16: On      ");
        //delay(1000);
        // lcd.noBacklight();
        break; // Button 1 on remote ends
        
        case 0xAF5A857:     //  is Hex value. Use this converter https://www.binaryhexconverter.com/hex-to-decimal-converter
        Serial.println(F(""));
        Serial.println(F("This is VOL - On the REMOTE "));
        Serial.println(F("So Light 16 is switched OFF "));
        pinMode(MYPIN4, LOW);
        turnOff("5b632910a98fea5737c2c944");
        lcd.backlight();
        Serial.println("Backlight ON for 15 secs."); 
        light=0;
        time_15sec = millis(); 
        //lcd.clear();
        lcd.setCursor(0, 3);
        lcd.print("Light  16: Off     ");
        // delay(1000);
        //lcd.noBacklight();
        break; // Button 2 on remote ends

      } 
     
      irrecv.resume(); // Receive & check the next value after waiting for 1 second
  }
/*//////////////////////////////////////////////////////////////////////////
 if (irrecv.decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    serialPrintUint64(results.value, HEX);
    Serial.println("");
    irrecv.resume();  // Receive the next value
  }
  delay(100);
*//////////////////////////////////////////////////////////////////////////
 
  
}     // VOID LOOP ENDS HERE



// If you are going to use a push button to on/off the switch manually, use this function to update the status on the server
// so it will reflect on Alexa app.
// eg: setPowerStateOnServer("deviceid", "ON")
void setPowerStateOnServer(String deviceId, String value) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["deviceId"] = deviceId;
  root["action"] = "setPowerState";
  root["value"] = value;
  StreamString databuf;
  root.printTo(databuf);
  
  webSocket.sendTXT(databuf);
}

//eg: setPowerStateOnServer("deviceid", "CELSIUS", "25.0")
void setTargetTemperatureOnServer(String deviceId, String value, String scale) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["action"] = "SetTargetTemperature";
  root["deviceId"] = deviceId;
  
  JsonObject& valueObj = root.createNestedObject("value");
  JsonObject& targetSetpoint = valueObj.createNestedObject("targetSetpoint");
  targetSetpoint["value"] = value;
  targetSetpoint["scale"] = scale;
   
  StreamString databuf;
  root.printTo(databuf);
  
  webSocket.sendTXT(databuf);
}


