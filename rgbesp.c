#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char *ssid = "BHADRINATHAN";
const char *password = "83000551055";
const char *mqtt_server = "192.168.1.4";
const int mqtt_port = 1883;
const char *mqtt_username = "guest";
const char *mqtt_password = "guest";
const char *mqtt_topic_receive = "car_receive";
const char *mqtt_topic_send = "car_send";

const int RED_PIN = 5;
const int GREEN_PIN = 4;
const int BLUE_PIN = 0;

const int TRIG_PIN = 13;
const int ECHO_PIN = 15;
const float SOUND_SPEED = 0.0343 / 2;

bool Front_Dis = false;
bool Back_Dis = false;

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(9600);
  delay(1000);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  client.setServer(mqtt_server, mqtt_port);
  while (!client.connected())
  {
    Serial.println("Connecting to MQTT broker...");
    if (client.connect("ESP32-client", mqtt_username, mqtt_password))
    {
      Serial.println("Connected to MQTT broker");
    }
    else
    {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  client.setCallback(callback);         // Set the callback function for receiving messages
  client.subscribe(mqtt_topic_receive); // Subscribe to the topic
}

void loop()
{
  client.loop(); // Call the loop method of the PubSubClient library

  FrontUlt(10);
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.println("Received a message:");
  Serial.print("Topic: ");
  Serial.println(topic);

  Serial.print("Payload: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  // Forward message to another MQTT topic
  client.publish(mqtt_topic_send, payload, length);

  if (Front_Dis == 1)
  {
    analogWrite(RED_PIN, 0);
    analogWrite(GREEN_PIN, 0);
    analogWrite(BLUE_PIN, 0);
  }
  else
  {
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, (char *)payload);
    int redValue, greenValue, blueValue;
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
    }
    // Extract values from JSON message and control RGB LED
    if (doc.is<JsonArray>())
    {
      JsonArray arr = doc.as<JsonArray>();
      if (arr.size() == 3)
      {
        int redValue = arr[0];
        int greenValue = arr[1];
        int blueValue = arr[2];
        analogWrite(RED_PIN, redValue);
        analogWrite(GREEN_PIN, greenValue);
        analogWrite(BLUE_PIN, blueValue);
        Serial.print("RGB LED values set to (");
        Serial.print(redValue);
        Serial.print(",");
        Serial.print(greenValue);
        Serial.print(",");
        Serial.print(blueValue);
        Serial.println(")");
      }
      else
      {
        Serial.println("Invalid array size");
      }
    }
    else
    {
      Serial.println("Invalid JSON input");
    }
    Serial.println();
  }
}

void FrontUlt(int dis)
{
  digitalWrite(TRIG_PIN, LOW); // set trigger signal low for 2us
  delayMicroseconds(2);

  /*send 10 microsecond pulse to trigger pin of HC-SR04 */
  digitalWrite(TRIG_PIN, HIGH); // make trigger pin active high
  delayMicroseconds(10);        // wait for 10 microseconds
  digitalWrite(TRIG_PIN, LOW);  // make trigger pin active low

  /*Measure the Echo output signal duration or pulss width */
  long duration = pulseIn(ECHO_PIN, HIGH); // save time duration value in "duration variable
  int distance = duration * SOUND_SPEED;   // Convert pulse duration into distance

  if (distance < dis)
  {
    analogWrite(RED_PIN, 0);
    analogWrite(GREEN_PIN, 0);
    analogWrite(BLUE_PIN, 0);
    Front_Dis = true;
  }
  else
  {
    Front_Dis = false;
  }
}