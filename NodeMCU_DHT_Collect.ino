#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <dht.h>

#define BAUD_RATE   115200

const char* ssid = "ssid";          // WiFi SSID
const char* password = "password";  // WiFi Password
const int led = LED_BUILTIN; // This code uses the built-in led for visual feedback that the button has been pressed

const char* mqtt_server = "192.168.0.115";  // MQTT IP Address //collect.sensors.in
const char* mqtt_topic = "DHT_Topic";    // MQTT Topic Name
const char* clientID = "ESP8266";     // The client id identifies the ESP8266 device.

WiFiClient wifiClient;       // Initialise the WiFi
PubSubClient client(mqtt_server, 1883, wifiClient);   // 1883 is the listener port for the Broker

void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH); // Switch the on-board LED off to start with
  Serial.begin(BAUD_RATE);

  Serial.println("\nHardware Serial Started in NodeMCU");

  ConnectToWiFi();

  delay(500);
  if (client.connect(clientID)){                  // Connect to MQTT Broker
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}

void loop() {
  SendDataToServer()
  delay(300000) // 5 min dealy
} // void loop ends


void SendDataToServer(){
  String payload="";
    
  payload = buildDataStream();
  if (client.publish(mqtt_topic, payload.c_str())) {      // PUBLISH to the MQTT Broker (topic = mqtt_topic, defined at the beginning)
      Serial.println("Payload sent!"); 
  }
  //client.publish will return a boolean value depending on whether it succeded or not.
  else {         
    Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID);
    delay(10);                                              // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(mqtt_topic, payload.c_str());
  }
} // send data ends

void buildDataStream() {
  DHT.read11(DHT11_PIN);
  float t = DHT.temperature;
  float h = DHT.humidity;
  
  String data = "";
  data = "Temperature=";
  data += String(t);
  data += ",";          //Add a new line
  data = "Humidity=";
  data += String(h);
  data += "\n";          //Add a new line
  // Temperature=28,Humidity=30
  return data
}

void ConnectToWiFi(void) {
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println("WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}
