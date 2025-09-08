#include <WiFi.h>
#include <PubSubClient.h>

// WiFi credentials - REPLACE WITH YOUR VALUES
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// MQTT Broker settings - REPLACE WITH YOUR VALUES
const char* mqtt_server = "your-broker-url.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "your-hivemq-username";
const char* mqtt_pass = "your-hivemq-password";

// Built-in LED pin (usually GPIO 2 on most ESP32 boards)
const int ledPin = 2;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Check if the message is for the LED topic
  if (String(topic) == "myhome/led") {
    if (message == "ON") {
      digitalWrite(ledPin, HIGH);  // Turn LED ON
      Serial.println("LED turned ON");
    } else if (message == "OFF") {
      digitalWrite(ledPin, LOW);   // Turn LED OFF
      Serial.println("LED turned OFF");
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      // Subscribe to the topic
      client.subscribe("myhome/led");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  // Start with LED off
  
  Serial.begin(115200);
  setup_wifi();
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}