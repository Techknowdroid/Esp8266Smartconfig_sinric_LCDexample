/*
 Tarun Goel VERSION 9.00 -  5th August,2018
*/ 

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h> //  get it from https://github.com/Links2004/arduinoWebSockets/releases 
#include <ArduinoJson.h> // get it from https://arduinojson.org/ or install via Arduino library manager
#include <StreamString.h>

#include <LiquidCrystal_I2C.h>  // get from https://www.arduinolibraries.info/libraries/liquid-crystal-i2-c
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;

#define MyApiKey ".........................." // In this line Enter your sinric API Key. Your API Key is displayed on sinric.com dashboard
#define MySSID "FIRSTFLOOR" // TODO: Change to your Wifi network SSID
#define MyWifiPassword "............" // In this line Enter your Wifi password
#define MYPIN1 D3  //GPIO 0
#define MYPIN2 D4  //GPIO 02
#define MYPIN3 D5  //GPIO 14
#define MYPIN4 D6  //GPIO 12
#define MYPIN5 D7  //GPIO 13

#define HEARTBEAT_INTERVAL 300000 // 5 Minutes 

uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

void setPowerStateOnServer(String deviceId, String value);
void setTargetTemperatureOnServer(String deviceId, String value, String scale);

// deviceId is the ID assgined to your smart-home-device in sinric.com dashboard. Copy it from dashboard and paste it here

void turnOn(String deviceId) {
  if (deviceId == "5b632822a98fea5737c2c941") //In this line Enter your sinric Device ID. DeviceId is the ID assgined to your smart-home-device in sinric.com dashboard. Copy it from dashboard and paste it here. Device ID of MYPIN1 is Light 13 for me.
  {  
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    Serial.print(" Switch D3");
    pinMode(MYPIN1, HIGH);
    lcd.noBacklight();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Light  13: On     ");
    delay(5000);
    lcd.noBacklight();
  
  } 
  else if (deviceId == "5b6328d1a98fea5737c2c942") // In this line Enter your sinric Device ID. DeviceId is the ID assgined to your smart-home-device in sinric.com dashboard. Copy it from dashboard and paste it here.Device ID of MYPIN2 is Light 14 for me.
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    Serial.print(" Switch D4");
    pinMode(MYPIN2, HIGH);
    lcd.noBacklight();
    lcd.backlight();
    lcd.setCursor(0, 1);
    lcd.print("Light  14: On     ");
    delay(5000);
    lcd.noBacklight();

  }
   else if (deviceId == "5b6328e7a98fea5737c2c943") // In this line Enter your sinric Device ID. DeviceId is the ID assgined to your smart-home-device in sinric.com dashboard. Copy it from dashboard and paste it here.Device ID of MYPIN3 is Light 15 for me.
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    Serial.print(" Switch D5");
    pinMode(MYPIN3, HIGH);
    lcd.noBacklight();
    lcd.backlight();
    lcd.setCursor(0, 2);
    lcd.print("Light  15: On     ");
    delay(5000);
    lcd.noBacklight();

  }
   else if (deviceId == "5b632910a98fea5737c2c944") // In this line Enter your sinric Device ID. DeviceId is the ID assgined to your smart-home-device in sinric.com dashboard. Copy it from dashboard and paste it here.Device ID of MYPIN4 is Light 16 for me.
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    Serial.print(" Switch D6");
    pinMode(MYPIN4, HIGH);
    lcd.noBacklight();
    lcd.backlight();
    lcd.setCursor(0, 3);
    lcd.print("Light  16: On     ");
    delay(5000);
    lcd.noBacklight();

  }
   else if (deviceId == "5b63291ea98fea5737c2c945") // In this line Enter your sinric Device ID. DeviceId is the ID assgined to your smart-home-device in sinric.com dashboard. Copy it from dashboard and paste it here.Device ID of MYPIN5 is Light 17 for me.
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    Serial.print(" Switch D7");
    pinMode(MYPIN5, HIGH);
    lcd.noBacklight();
    lcd.backlight();
    lcd.setCursor(0, 4);
    lcd.print("Light  17: On     ");
    delay(5000);
    lcd.noBacklight();

  }
   
  else {
    Serial.print("Turn on for unknown device id: ");
    Serial.println(deviceId);    
  }     
}

void turnOff(String deviceId) {
   if (deviceId == "5b632822a98fea5737c2c941") //In this line Enter your sinric Device ID. DeviceId is the ID assgined to your smart-home-device in sinric.com dashboard. Copy it from dashboard and paste it here. Device ID of MYPIN1 is Light 13 for me.
   {  
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     Serial.print(" Switch D3");
     pinMode(MYPIN1, LOW);
     lcd.noBacklight();
     lcd.backlight();
     lcd.setCursor(0, 0);
     lcd.print("Light  13: Off     ");
     delay(5000);
     lcd.noBacklight();
   }
   else if (deviceId == "5b6328d1a98fea5737c2c942") //In this line Enter your sinric Device ID. DeviceId is the ID assgined to your smart-home-device in sinric.com dashboard. Copy it from dashboard and paste it here. Device ID of MYPIN2 is Light 14 for me.
   { 
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     Serial.println(" Switch D4");
     pinMode(MYPIN2, LOW);
     lcd.noBacklight();
     lcd.backlight();
     lcd.setCursor(0, 1);
     lcd.print("Light  14: Off     ");
     delay(5000);
     lcd.noBacklight();

  }
   else if (deviceId == "5b6328e7a98fea5737c2c943") //In this line Enter your sinric Device ID. DeviceId is the ID assgined to your smart-home-device in sinric.com dashboard. Copy it from dashboard and paste it here. Device ID of MYPIN3 is Light 15 for me.
   { 
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     Serial.print(" Switch D5");
     pinMode(MYPIN3, LOW);
     lcd.noBacklight();
     lcd.backlight();
     lcd.setCursor(0, 2);
     lcd.print("Light  15: Off     ");
     delay(5000);
     lcd.noBacklight();

  }
   else if (deviceId == "5b632910a98fea5737c2c944") //In this line Enter your sinric Device ID. DeviceId is the ID assgined to your smart-home-device in sinric.com dashboard. Copy it from dashboard and paste it here. Device ID of MYPIN4 is Light 16 for me.
   { 
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     Serial.print(" Switch D6");
     pinMode(MYPIN4, LOW);
     lcd.noBacklight();
     lcd.backlight();
     lcd.setCursor(0, 3);
     lcd.print("Light  16: Off     ");
     delay(5000);
     lcd.noBacklight();

  }
   else if (deviceId == "5b63291ea98fea5737c2c945") //In this line Enter your sinric Device ID. DeviceId is the ID assgined to your smart-home-device in sinric.com dashboard. Copy it from dashboard and paste it here. Device ID of MYPIN5 is Light 17 for me.
   { 
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     Serial.print(" Switch D7");
     pinMode(MYPIN5, LOW);
     lcd.noBacklight();
     lcd.backlight();
     lcd.setCursor(0, 4);
     lcd.print("Light  17: Off     ");
     delay(5000);
     lcd.noBacklight();

  }
  
  else {
     Serial.print("Turn off for unknown device id: ");
     Serial.println(deviceId);    
  }
}

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
    delay(7000);
    lcd.clear();   
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

void setup() {
  Serial.begin(115200);
  /* Set ESP8266 to WiFi Station mode */

  //Initalize LCD
  lcd.init();
  lcd.noBacklight();
  lcd.backlight();
  lcd.begin(4, 20);


  
  WiFi.mode(WIFI_STA);
  /* start SmartConfig */
  WiFi.beginSmartConfig();
  
   /* Wait for SmartConfig packet from mobile */
  Serial.println("Waiting for SmartConfig.");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Start Smartconfig");
  lcd.setCursor(0, 2);
  lcd.print("on your phone..");
  while (!WiFi.smartConfigDone()) {
    delay(500);
    //lcd.print("."); 
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("SmartConfig done.");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.noBacklight();
  //delay(500);
  lcd.backlight();
  lcd.print("Smartconfig Done!!");
  delay(3000);
  /* Wait for WiFi to connect to AP */
  Serial.println("Waiting for WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Connected.");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected to:");
    lcd.setCursor(0, 2);
    lcd.print(WiFi.SSID());
    delay(5000);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("WiFi Connected to SSID...  ");
  Serial.println(WiFi.SSID());
  Serial.println("WiFi Password...NOT DISPLAYED.");
//  Serial.print(WiFi.PASS());

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
}

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
