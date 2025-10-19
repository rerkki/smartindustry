// This example uses an ESP32 Development Board
// to connect to shiftr.io.
//
//
// Based on code by Joël Gähwiler
// https://github.com/256dpi/arduino-mqtt

#include <WiFi.h>
#include <MQTT.h>

//////DHT Sensor initialization
#include "DHT.h"
#define DHTPIN 4 
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
//////

////// Give database information here:
String myDB = "db_test"; //unique database name
String myColl = "coll_test"; //unique collection name
String myID = "id"; //serial number, mac address or other unique identification for your device
//////

////// Give WiFi credentials here
const char ssid[] = "XXXXXXXX";
const char pass[] = "YYYYYYYY";
//////

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("ESP-32-eki", "automaatio", "Z0od2PZF65jbtcXu")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("automaatio");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
  // by Arduino. You need to set the IP address directly.
  client.begin("automaatio.cloud.shiftr.io", net);
  client.onMessage(messageReceived);

  connect();

  dht.begin();
}

void loop() {
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every 30 seconds.
  if (millis() - lastMillis > 30000) {
    lastMillis = millis();

///// Here, a JSON message to be published is parsed.
///// first, we include database and ID information:
  String PubString = "{\"db_name\":\"" + myDB + "\"" + "," +
                      "\"coll_name\":\"" + myColl + "\"" + "," +
                      "\"id\":\"" + myID + "\"" + "," +

///// Each sensor will be included into JSON as functions.
///// The output of the sensor functions must be JSON-compatible strings
///// You can include as many sensors as you can fit into a message of 250 characters
                       Sensor1() + "," +
                       Sensor2() +

///// Finally, the JSON is closed
                       "}";

///// The string is then published
    client.publish("automaatio", PubString);
  }
}


//////Here are the functions to read the sensors

String Sensor1(){
//////Prototype for reading an analog voltage sensor signal
  int ADC1 = 35; // GPIO 35 is input for sensor 1;

  int val = analogRead(ADC1);

//////Output is formatted as JSON compatible string
  String sensor1 = "\"S1\":" + String(val);

  return sensor1.c_str();

}

String Sensor2(){
/////DHT21 humidity & temperature sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  //Set both values to zero if the sensor is faulty
  if (isnan(h) || isnan(t)) {
    h = 0; t = 0;
  }

  //Output contains both humidity and temperature values in JSON-compatible format
  String sensor2 = "\"H\":" + String(h) + "," + "\"T\":" + String(t);

  return sensor2.c_str();

}

