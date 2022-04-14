//======================================================================
//  Program:Mailbox
//
//  Description: 
//                
//               
//
//
//  
//                
//                
//                
//
//======================================================================
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>



// Include config
#include "Connections_config.h"
#include "Io_config.h"

// Define the client
WiFiClient espClient;
PubSubClient client(espClient);



int startupmessege;
float analogValue;
float input_voltage;
float last_input_voltage;
int Input1;
int Input2;



int sleeptime = 4000e6; //standard 60 min sovtid


//Function to setup wifi and string printout

void handle_debug(bool sendmqtt, String debugmsg) {
  // Handle the debug output
  DEBUG_PORT.println(debugmsg);

  
  // send to mqtt_command_topic
  if (sendmqtt && client.connected())
  {
    char debugChar[50];
    debugmsg.toCharArray(debugChar,50);
    client.publish(mqtt_debug_topic, debugChar);
  }
}




void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  handle_debug(false, "");
  handle_debug(false, (String)"Connecting to: " + (String)ssid);
  
  WiFi.config(local_IP, gateway, subnet, primaryDNS);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DEBUG_PORT.print(".");
  }
  
  handle_debug(false, "");
  //handle_debug(false, (String)"WiFi connected. IP Address: " + (String)WiFi.localIP()); 
}

void handle_status(String statusTopic, String statusMsg) {
  // Send to debug
  //handle_debug(true, statusMsg);
  
  // send to mqtt_status_topic
  if (client.connected())
  {


    char statusChar[50];
    statusMsg.toCharArray(statusChar,50);

    char statusChar1[50];
    statusTopic = mqtt_status_topic + statusTopic;
    statusTopic.toCharArray(statusChar1,50);
    

    client.publish(statusChar1, statusChar);
  }
}

void handle_battery_status() {
  
  analogValue = analogRead (A0);
  delay (1000); 
  input_voltage = (analogValue * 5.0) / 1024.0;
  handle_status("/battery", String(input_voltage));

  
}


void handle_input_status() {
  
  Input1 = digitalRead(Input_1);
  Input2 = digitalRead(Input_2);

  if (Input1 = 0)
  {
    handle_status("/input1", String(Input1));
  }
  if (Input2 = 0)
  {
    handle_status("/input2", String(Input2));
  }
    
}


void handle_command(String command) {

  if(command.startsWith("sleep:")) 
    { 
      sleeptime = abs(command.substring(7).toInt() * 1e6);
      Serial.println("Going into deep sleep!");
      handle_debug(true, "Sleeping for: " + String(sleeptime));
      delay (1000); 
      ESP.deepSleep(sleeptime);
    }

}




void callback(char* topic, byte* message, unsigned int length) {
  String messageTemp;

  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }

  handle_debug(false, (String)"Message arrived on topic: " + (String)topic + (String)". Message: " + (String)messageTemp);

  //Commands
  if ((String)topic == (String)mqtt_command_topic) {
    // We got a command, lets handle it!
    handle_command(messageTemp);

    handle_debug(false, "Got command via MQTT");
  }
}

void setup()
{
  DEBUG_PORT.begin(9600);
  while (!Serial)
    ;
  DEBUG_PORT.print( F("esp running\n") );

  // Start wifi 
  setup_wifi();
 

  pinMode(A0,OUTPUT);

  analogValue = analogRead (A0);
  delay (1000); 
  input_voltage = (analogValue * 5.0) / 1024.0;



  // Connect MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  startupmessege = 1;

  // start ArduinoOTA
  ArduinoOTA.setHostname(devicename);

  ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

    ArduinoOTA.begin();
    

}

//--------------------------

// Functions
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    handle_debug(false, "Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Mailbox", mqtt_username, mqtt_password, mqtt_lwt_topic, 1, true, "Offline")) {
      handle_debug(false, "MQTT connected");
      // Subscribe to commands
      client.subscribe(mqtt_command_topic);
      // Subscribe to preferences
      client.subscribe(mqtt_preferences_topic);
      // Set LWT to Online
      client.publish(mqtt_lwt_topic, "Online", true);
      // Send Boot message
      
      if (startupmessege == 1) {
        client.publish(mqtt_boot_topic, "Started", true);
        startupmessege = 0;
      }
    } else {
      handle_debug(false, (String)"MQTT failed, rc=" + (String)client.state() + (String)". Try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}










// Main loop
void loop()
{


  // Check if MQTT is connected 
  if (!client.connected()) {
    reconnect();
  }
  // Do the MQTT Client Loop
  client.loop();
  

  handle_battery_status();
  handle_input_status();


  

  // Handle ota
  ArduinoOTA.handle();

  
}




