#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "Friday";
const char* password = "Ideapad330";

// MQTT server details
const char* mqtt_server = "rabbitmq.youngstorage.in";
const int mqtt_port = 1883;
const char* mqtt_username = "guest";
const char* mqtt_password = "guest";
const char* mqtt_topic_receive = "test_1";
const char* mqtt_topic_send = "test";

// WiFi and MQTT client objects
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Connect to MQTT broker
  mqttClient.setServer(mqtt_server, mqtt_port);
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (mqttClient.connect("ESP32-client", mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed with state ");
      Serial.print(mqttClient.state());
      delay(2000);
    }
  }

  // Subscribe to MQTT topic to receive messages
  mqttClient.setCallback(callback);
  mqttClient.subscribe(mqtt_topic_receive);
}

void loop() {
  // Handle incoming MQTT messages
  mqttClient.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Print received message to serial monitor
  Serial.println("Received message:");
  Serial.print("  Topic: ");
  Serial.println(topic);  
  Serial.print("  Payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Forward message to another MQTT topic
  mqttClient.publish(mqtt_topic_send, payload, length);
}
