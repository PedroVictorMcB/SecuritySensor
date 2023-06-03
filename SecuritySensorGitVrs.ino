/*
===================================================================================================
    Final Project ESP8266 - Pedro Victor
===================================================================================================


    Based in a tutorial of:

    Rui Santos
    Complete project details at https://RandomNerdTutorials.com/telegram-request-esp32-esp8266-nodemcu-sensor-readings/

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

    Project created using Brian Lough's Universal Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
*/

// Include session: Here we import all libraries that we'll need in this venture
// ------------------------------------------------------------------------------------------------------------------

// Notice that our target here is only ESP8266
#include <ESP8266WiFi.h>
#include <string>
#include <ESP8266HTTPClient.h>
// #include <esp8266httpclient.h> (caso não dê certo, retorne está lib)
#include <ESP8266WebServer.h>

// ------------------------------------------------------------------------------------------------------------------

// Constants setup: Add here environment variables and configurations that will run in the entire application

// ------------------------------------------------------------------------------------------------------------------
// Replace with your network credentials
const char* ssid = "";
const char* password = "";

// set the esp8266 as client
HTTPClient client;
WiFiClient wifiClient;

//Checks for new messages every 1 second.
int sensorReadingDelay = 3000;
unsigned long lastTimeSensorRead;

const int LIn = D0;                // the pin that the LED is atteched to
const int sensor = D2;              // the pin that the sensor is atteched to
int ledState = LOW;
bool isSensorTrg = false;

// ------------------------------------------------------------------------------------------------------------------

// fuction to url encode msg

String urlencode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
    
}

// Get Readings of our sensor
String getSensorReadings(){
  int read = digitalRead(sensor);   // read sensor value
  
  if (!isSensorTrg && read == HIGH){  // If the sensor is unactive, it will turn in HIGH and go to this condition
    isSensorTrg = true;   
    ledState = ledState == HIGH ? LOW : HIGH; 

    digitalWrite(LIn, ledState);
    return urlencode("Olá, Temos Um Invasor!");
  }
  else if (isSensorTrg && read == LOW){  // If the sensor is activated, it will turn in LOW and go to this condition
    isSensorTrg = false;  // smoothnees of the sensor readings
    delay(100);  
  }
    return "";
}
// function to bot send me messages
void sendWppMsg(String msg){
  client.begin(wifiClient, "http://api.callmebot.com/whatsapp.php?phone=************&text="+msg+"&apikey=*******");  //Specify request destination, use the apikey you receved on whatsapp with callmebot
    Serial.println("http://api.callmebot.com/whatsapp.php?phone=************&text="+msg+"&apikey=*******");
    int statusCode = client.GET();                                  //Send the request
    Serial.println(statusCode);
    if (statusCode > 0) { //Check the returning code
 
      String payload = client.getString();   //Get the request response payload
      Serial.println(payload);             //Print the response payload
 
    }
 
    client.end();   //Close connection  
}

//conect to Wifi
IPAddress conectWifi(char *ssid, char *password){
    // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  return WiFi.localIP();
}

void setup() {
    Serial.begin(9600);
  IPAddress ip = conectWifi((char*)ssid, (char*)password);
  // Print assigned local IP
  Serial.println(ip);

  pinMode(LIn, OUTPUT);      // initalize LED as an output
  pinMode(sensor, INPUT);    // initialize sensor as an input
  digitalWrite(LIn, LOW); // ledState, control values for Lin
}

// This loop will receive bot messages. It's a default implementation and we don't need to change anything here.
void loop() {
  String sensorResult = getSensorReadings();
  if (sensorResult != ""){
    sendWppMsg(sensorResult);
  }
}