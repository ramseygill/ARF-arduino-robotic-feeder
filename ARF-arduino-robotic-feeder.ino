/*
   ===============================================================================================
   Title: ARF- Arduino Robotic Feeder
   Description: ESP8266 Based, Arduino compatible, Alexa commandable, dog feeder.
   Author: Ramsey Gill
   License MIT
   Homepage: https://hackaday.io/project/43858-arf-arduino-robotic-feeder
   GitHub: https://github.com/ramseygill/ARF-arduino-robotic-feeder
   ===============================================================================================
   NOTES, Credit, Sources, and Props
   NTP Library:https://github.com/arduino-libraries/NTPClient
   OLED https://github.com/adafruit/Adafruit_SSD1306
   EASY DRIVER https://learn.sparkfun.com/tutorials/easy-driver-hook-up-guide
*/
//------------------------------------------------------------------------------------------------
//LIBRARIES
//------------------------------------------------------------------------------------------------

#include <ESP8266WiFi.h>        // ESP8266 (ESP-12E) wifi library
#include "SSD1306.h"            // SSD1306 OLED display 
#include "fauxmoESP.h"          // Alexa suppport. (fake WeMo devices)
#include <NTPClient.h>          // Supports NTP server time queries. 
#include <WiFiUdp.h>            // Adds UDP protocol support, required by NTP service.

//------------------------------------------------------------------------------------------------
//VARIABLES AND GLOBALS
//------------------------------------------------------------------------------------------------

// PIN DEFINITION CONSTANTS
const int I2C_SDA = 0;      //I2C bus data pin (SCL). ESP-12E "D3" GPIO0
const int I2C_SCL = 14;     //I2C bus clock pin (SCL). ESP-12E "D5" GPIO14
const int stp = 13;         //EASY DRIVER STEP PIN
const int dir = 12;         //EASY DRIVER DIRECTION PIN

//Declare variables for functions (EASY DRIVER)
char user_input;
int x;
int y;
int state;

// GLOBAL CONSTANTS
const char *ssid     = "skynet-2GHz";   // WIFI network you want to connect to.
const char *password = "haxor1337";     // WIFI network password.
const int baudRate = 115200;            // USB serial baud rate. (DEBUG)

// LIBRARY INITIALIZATION
SSD1306  display(0x3c, I2C_SDA, I2C_SCL); // SSD1306 Pin Assignments
fauxmoESP fauxmo;                         // Initialize fauxmo
WiFiUDP ntpUDP;                           // Initalize WifiUDP library

// By default 'time.nist.gov' is used with 60 seconds update interval and no offset
// Seattle is UTC-8 or -28800 seconds.
// Update interval is one minute or 60000 miliseconds.
// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
NTPClient timeClient(ntpUDP, "0.us.pool.ntp.org", -28800, 60000);

//------------------------------------------------------------------------------------------------
//SETUP
//------------------------------------------------------------------------------------------------
void setup() {

  //START I2C COMMUNICATION
  display.init();   // SSD12306 - OLED screen start I2C comms

  //Flip Screen orientation in software, (pins on top)
  display.flipScreenVertically(); //SSD12306

  //CONFIGURE PIN USAGE
  pinMode(stp, OUTPUT); //EASY DRIVER
  pinMode(dir, OUTPUT); //EASY DRIVER

  //SET PIN INITIAL STATE

  // FAUXMO DEVICES
  // Each of these devices will appear in Alexa App.
  // Create a group from these devices "Front Yeard Sprinklers"
  // Each controller will have its own letter "A" followed by a zone "1"
  fauxmo.addDevice("Dog Feeder");

  fauxmo.onMessage(callback); //function call for fauxmo state check.

//------------------------------------------------------------------------------------------------
  Serial.begin(baudRate);                            //start UART serial comm at global deff baudrate
  Serial.println();                                  // add two blank lines
  Serial.println();
  Serial.println("ARF - Arduino Robotic Feeder"); //vanity title for serial window
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
  wifiSetup();     // Function call to kick off WiFi setup.
//------------------------------------------------------------------------------------------------
}

//------------------------------------------------------------------------------------------------
//FUNCTIONS (SUBROUTINES)
//------------------------------------------------------------------------------------------------

//UPDATE IP FUNCTION
//------------------------------------------------------------------------------------------------
void updateIP() {
  String ipaddress = WiFi.localIP().toString(); // convert IP address array to string
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "IP:");
  display.drawString(18, 0, (ipaddress));
}
//UPDATE TIME FUNCTION
//------------------------------------------------------------------------------------------------

void updateTime() {
  timeClient.update(); // Requests time update from NTP server
  String timeString = timeClient.getFormattedTime(); // convert time to string
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 16, "TIME:");
  display.drawString(45, 16, (timeString));
}

//FAUXMO CALLBACK FUNCTION add devices here
//------------------------------------------------------------------------------------------------
void callback(uint8_t device_id, const char * device_name, bool state) {
  Serial.printf("[MAIN] %s state: %s\n", device_name, state ? "ON" : "OFF");


  //ALEXA DEVICE "DOG FEEDER"
  if ( (strcmp(device_name, "Dog Feeder") == 0) ) {
    if (state) {
      //GPIO
    } else {
      // GPIO
    }
  }
} // end function

// WIFI setup function
//------------------------------------------------------------------------------------------------
void wifiSetup() {
  // Set WIFI module to STA mode
  WiFi.mode(WIFI_STA);

  // Connect
  Serial.printf("[WIFI] Connecting to %s ", ssid);
  WiFi.begin(ssid, password);

  // Wait
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  timeClient.begin(); // start NTP time client

  // Connected!
  Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

// Stepper Driver FORWARD (food augur)
//------------------------------------------------------------------------------------------------
//Default microstep mode function
void FeedForward(){
  Serial.println("Moving forward at default step mode.");
  digitalWrite(dir, HIGH); //Pull direction pin low to move "forward"
  for(x= 1; x<1000; x++)  //Loop the forward stepping enough times for motion to be visible
  {
    digitalWrite(stp,HIGH); //Trigger one step forward
    delay(1);
    digitalWrite(stp,LOW); //Pull step pin low so it can be triggered again
    delay(1);
  }
  Serial.println("Feed Done");
  Serial.println();
}

// Stepper Driver BACKWARD (food augur)
//------------------------------------------------------------------------------------------------
//Default microstep mode function
void FeedBackward(){
  Serial.println("Moving backwards at default step mode.");
  digitalWrite(dir, LOW); //Pull direction pin low to move "forward"
  for(x= 1; x<1000; x++)  //Loop the forward stepping enough times for motion to be visible
  {
    digitalWrite(stp,HIGH); //Trigger one step forward
    delay(1);
    digitalWrite(stp,LOW); //Pull step pin low so it can be triggered again
    delay(1);
  }
  Serial.println("Feed Back Done");
  Serial.println();
}
//------------------------------------------------------------------------------------------------

//================================================================================================
//MAIN LOOP
//================================================================================================
void loop() {
  display.clear();     // wipe display clean to refresh it
  updateIP();          // function call for display update
  updateTime();
  fauxmo.handle();     // call fauxmo device function
  display.display();   // write display buffer
  FeedForward();       // move augur forwardto dispense food. Uncomment to test augur 
  
  delay(250);           // loop governor, determine how fast this loop runs
}
//================================================================================================

