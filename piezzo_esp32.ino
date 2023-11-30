//Libraries
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <ArduinoJson.h>

//Wifi
#define WLAN_SSID  "Mogesacom."
#define WLAN_PASS  "man9'a2e"

// MQTT Broker
#define AIO_SERVER      "156.0.232.201"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "iotlab"
#define AIO_KEY         "80d18b0d"

//PIEZO
const int piezoPin = 32;    // Analog pin connected to the piezo sensor
const int threshold = 50;   // Adjust this threshold value to suit your sensor and environment
int pulseCount = 0;         // Variable to store the pulse count
bool detect = true;

//WiFi Client
WiFiClient client;

//MQTT Client
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT,"IOTLAB",AIO_USERNAME, AIO_KEY);

//Topics
//Publishing
Adafruit_MQTT_Publish PIEZO  = Adafruit_MQTT_Publish(&mqtt,"PIEZO");


/********* Sketch Code ************/
void MQTT_connect();

void setup() {
  Serial.begin(115200);
  pinMode(piezoPin, INPUT);

  //Connecting to WiFi access point.
  WiFi.begin(WLAN_SSID, WLAN_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    // Wait for successful connection
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println(F("PIEZO"));
}

uint32_t x=0;

void loop() {

  MQTT_connect();

    //PIEZO
    int sensorValue = analogRead(piezoPin); // Read the analog value from the piezo sensor

    if (sensorValue > threshold) {
    // Vibration detected, increment the pulse count
    pulseCount++;
    
    Serial.print("Vibration detected! Pulse Count: ");
    Serial.println(pulseCount);
    //Publishing
    StaticJsonDocument<256> jsonDoc;
    jsonDoc["Pulse Count"]=int(pulseCount);

    String jsonString;
    serializeJson(jsonDoc,jsonString);

    PIEZO.publish(jsonString.c_str()); 
    
    delay(3000);
}
    
   
 }

//FUNCTIONS
// Check if Wi-Fi is still connected
void WIFI_connect(){
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected");
    // Turn off the LED

    // Attempt to reconnect to Wi-Fi
    WiFi.begin(WLAN_SSID, WLAN_PASS);

    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Reconnecting to WiFi...");
    }
    Serial.println("Reconnected to WiFi");
  }
}

// Connecting and reconnecting to MQTT.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;

    if (retries == 0) {
      Serial.println("Max retries reached. Unable to connect to MQTT.");
      return;  // Exit the function without resetting the device
    }
  }

  Serial.println("MQTT Connected!");
}