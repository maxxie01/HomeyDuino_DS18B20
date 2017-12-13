/*
   Example of Homey Arduino 
   with Wemos D1 (ESP8266) 
   and Dallas DS18B20 temperature sensor

   Homey: https://github.com/athombv/homey-arduino-library
   Wemos: search WeMos D1 mini V2
   DS18B20: Dalles 1 wire temperature sensor
   
*/

#include <ESP8266WiFi.h>        //ESP8266 library
#include <WiFiClient.h>         //ESP8266 library
#include <Homey.h>              //Athom Homey library
#include <OneWire.h>            //DS18B20 temperature library
#include <DallasTemperature.h>  //DS18B20 temperature library

//DS18B20 setup
#define ONE_WIRE_BUS D2     //DS18B20 is connector to D2
float currentTemperature;
float lastTemperature = 0;


// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

//Global variables used for the blink without delay example
unsigned long previousMillis = 0;
const unsigned long interval = 5000; //Interval in milliseconds

IPAddress ip;

//Arduino functions
void setup() {
  //Enable serial port
  Serial.begin(115200);
  Serial.println("Connect to wifi");

  //Connect to network
  WiFi.begin("maxxie", "xxx");
  while (WiFi.status() != WL_CONNECTED) { 
    delay(200); 
    Serial.print(".");
  }
  Serial.println("");

  //Print IP address
  ip = WiFi.localIP();
  Serial.print("IP address: ");
  Serial.println(ip.toString());
    
  //Start Homey library
  Homey.begin("Arduino DS18B20");
  Homey.setClass("sensor");
  Homey.addCapability("measure_temperature");  

  // Start DS18B20 library
  sensors.begin();
  
  Serial.println("Setup completed");
}

void loop() {
  //Handle incoming connections
  Homey.loop();
  /* Note:
      The Homey.loop(); function needs to be called as often as possible.
      Failing to do so will cause connection problems and instability.
      Avoid using the delay function at all times. Instead please use the
      method explaind on the following page on the Arduino website:
      https://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
  */

  //This is the 'blink without delay' code
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    //(This code will be executed every <interval> milliseconds.)
    
    // call sensors.requestTemperatures() to issue a global temperature 
    // request to all devices on the bus
    Serial.print("Requesting temperatures...");
    sensors.requestTemperatures(); // Send the command to get temperatures
    Serial.println("DONE");

    // Read temperature as Celsius (the default)
    float currentTemperature = sensors.getTempCByIndex(0); //there is only one shield
    if(lastTemperature!=currentTemperature){
      //Emit a trigger to Homey
      Homey.setCapabilityValue("measure_temperature", currentTemperature);
      lastTemperature = currentTemperature;
      Serial.print("Temperature: ");
      Serial.println(currentTemperature);
    }
    
  }
}


