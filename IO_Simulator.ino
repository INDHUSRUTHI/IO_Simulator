#include <WiFi.h>
#include <PubSubClient.h>


const char* ssid = "username";      
const char* password = "password"; 

const char* mqtt_server = "broker.emqx.io"; 
const char* mqtt_topic = "mae_qwerty23456"; 


const int relayPins[3] = {23, 22, 21}; 

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  // Setup GPIO pins
  for (int i = 0; i < 3; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW); 
  }

  
  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}


void setup_wifi() {
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println(" connected!");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); 
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    String clientId = "ESP32Client-";
    clientId += String(WiFi.macAddress()); 

    Serial.print("Client ID: ");
    Serial.println(clientId); 

   
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Message received: ");
  Serial.println(message);
  
  for (int i = 0; i < 3; i++) {
    if (i < message.length()) {
      if (message[i] == '1') {
        digitalWrite(relayPins[i], HIGH); 
      } else {
        digitalWrite(relayPins[i], LOW); 
      }
    }
  }
}
