#include <Arduino.h>
#include "WiFiNINA.h"
#include "ThingSpeak.h"
#include <PubSubClient.h>

const char* ssid = "JioFi3_1E61F1"; // Replace with your Wi-Fi network SSID
const char* password = "h43dyas71e"; // Replace with your Wi-Fi network password


const unsigned long channelID = 2515894;
const char* writeAPIKey = "VOIESPM9U5SWX072";

const char* server = "mqtt3.thingspeak.com";

const char* mqttUserName = "BTowNxowEQ4vNSsrIgkQHSc";

const char* mqttPass = "GSRmGKwD0IuyAkRBTY43Cf2n";

const char* clientID = "BTowNxowEQ4vNSsrIgkQHSc";
int port = 1883;

const int analogInPin1 = A0; // Analog input pin for sensor 1
const int analogInPin2 = A1; // Analog input pin for sensor 2
const float referenceVoltage = 5.0; // Arduino's reference voltage
const int numSamples = 1000; // Number of samples to take
unsigned long sumSquares1 = 0;
unsigned long sumSquares2 = 0;
int count1 = 0;
int count2 = 0;
int downtime1 = 0;
int downtime2 = 0;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
 Serial.begin(9600);
 delay(100);
 Serial.print("Connecting to ");
 Serial.println(ssid);

 WiFi.begin(ssid, password);

 while (WiFi.status() != WL_CONNECTED) {
   delay(1000);
   Serial.print(".");
 }

 Serial.println("");
 Serial.println("WiFi Connected!");
 Serial.print("IP Address: ");
 Serial.println(WiFi.localIP());

 ThingSpeak.begin(wifiClient);
mqttClient.setServer(server, port);
}

void loop() {
  if (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    while (!mqttClient.connect(clientID, mqttUserName, mqttPass)) {
      Serial.print(".");
    }
    Serial.println("Connected to MQTT");
  }
  mqttClient.loop();

  for (int i = 0; i < numSamples; i++) {
    // Read the analog voltages
    int rawValue1 = analogRead(analogInPin1);
    int rawValue2 = analogRead(analogInPin2);

    // Convert analog readings to voltages
    float voltage1 = ((rawValue1 / 1023.0) * referenceVoltage);
    float voltage2 = ((rawValue2 / 1023.0) * referenceVoltage);

    // Square the voltages and add them to the sum of squares
    sumSquares1 += voltage1 * voltage1;
    sumSquares2 += voltage2 * voltage2;

    // Wait a short delay between samples
    delayMicroseconds(100); // Adjust delay as needed
  }

  // Calculate the mean squares
  float meanSquare1 = sumSquares1 / float(numSamples);
  float meanSquare2 = sumSquares2 / float(numSamples);

  // Calculate the RMS values (square roots of the mean squares)
  float rmsCurrent1 = (sqrt(meanSquare1))*2.2*1.2;
  float rmsCurrent2 = (sqrt(meanSquare2))*2.2;

  // Print the RMS values to serial monitor
  Serial.print("RMS Current 1: ");
  Serial.println(rmsCurrent1);
  Serial.print("RMS Current 2: ");
  Serial.println(rmsCurrent2);

  // Reset sumSquares for the next cycle
  sumSquares1 = 0;
  sumSquares2 = 0;
  String dataString = "field1=" + String(rmsCurrent1);
  dataString += "&field2=" + String(rmsCurrent2);
  String topicString = "channels/" + String(channelID) + "/publish";

  // Publish data to MQTT
  if (mqttClient.publish(topicString.c_str(), dataString.c_str())) {
    Serial.println("Field 1 published successfully.");
    Serial.println("Field 2 published successfully.");
  } else {
    Serial.println("Failed to publish fields.");
  }

  delay(5000);

  //  // Send RMS current values to ThingSpeak
  //  int status1 = ThingSpeak.setField(1, rmsCurrent1);
  //  int status2 = ThingSpeak.setField(2, rmsCurrent2);
  //  ThingSpeak.writeFields(channelID, writeAPIKey);

  //  // Check if data sent successfully to all fields
  //  if (status1 == 200 && status2 == 200) {
  //    Serial.println("Data sent to ThingSpeak successfully.");
  //  } else {
  //    Serial.println("Failed to send data to ThingSpeak.");
  //  }

  //  delay(15000); 
}
