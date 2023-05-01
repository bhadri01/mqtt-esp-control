#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char *ssid = "Winter is Coming";
const char *password = "Ideapad330";
const char *mqtt_server = "192.168.0.135";
const int mqtt_port = 1883;
const char *mqtt_username = "guest";
const char *mqtt_password = "guest";
const char *mqtt_topic_receive = "car_receive";
const char *mqtt_topic_send = "car_send";

// Motor pins
const int ENA = 5;
const int IN1 = 4;
const int IN2 = 0;
const int ENB = 14;
const int IN3 = 12;
const int IN4 = 13;

const int FRONT_TRIG_PIN = 16;
const int FRONT_ECHO_PIN = 5;

const int BACK_TRIG_PIN = 4;
const int BACK_ECHO_PIN = 0;

const float SOUND_SPEED = 0.0343 / 2;

bool Front_Dis = false;
bool Back_Dis = false;

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  // Set motor pins as outputs
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(FRONT_TRIG_PIN, OUTPUT);
  pinMode(FRONT_ECHO_PIN, INPUT);

  pinMode(BACK_TRIG_PIN, OUTPUT);
  pinMode(BACK_ECHO_PIN, INPUT);

  // Set PWM frequency for motor enable pins
  analogWriteFreq(100);
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
  BACKUlt(10);
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
      int acc = arr[0];
      int angle = arr[1];
      int dir = arr[2];
      // digitalWrite(IN1, HIGH);
      // digitalWrite(IN2, LOW);
      analogWrite(ENA, acc); // Set motor speed
      analogWrite(ENB, acc); // Set motor speed

      if (angle < 60)
      {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
      }
      else if (angle > 120)
      {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
      }
      else
      {
        if (dir == 1)
        {
          digitalWrite(IN1, HIGH);
          digitalWrite(IN2, LOW);
          digitalWrite(IN3, HIGH);
          digitalWrite(IN4, LOW);
        }
        else if (dir == 0)
        {
          digitalWrite(IN1, LOW);
          digitalWrite(IN2, HIGH);
          digitalWrite(IN3, LOW);
          digitalWrite(IN4, HIGH);
        }
      }
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

void FrontUlt(int dis)
{
  digitalWrite(FRONT_TRIG_PIN, LOW); // set trigger signal low for 2us
  delayMicroseconds(2);

  /*send 10 microsecond pulse to trigger pin of HC-SR04 */
  digitalWrite(FRONT_TRIG_PIN, HIGH); // make trigger pin active high
  delayMicroseconds(10);              // wait for 10 microseconds
  digitalWrite(FRONT_TRIG_PIN, LOW);  // make trigger pin active low

  /*Measure the Echo output signal duration or pulss width */
  long duration = pulseIn(FRONT_ECHO_PIN, HIGH); // save time duration value in "duration variable
  int distance = duration * SOUND_SPEED;         // Convert pulse duration into distance

  if (distance < dis)
  {
    Front_Dis = true;
  }
  else
  {
    Front_Dis = false;
  }
}

void BACKUlt(int dis)
{
  digitalWrite(BACK_TRIG_PIN, LOW); // set trigger signal low for 2us
  delayMicroseconds(2);

  /*send 10 microsecond pulse to trigger pin of HC-SR04 */
  digitalWrite(BACK_TRIG_PIN, HIGH); // make trigger pin active high
  delayMicroseconds(10);             // wait for 10 microseconds
  digitalWrite(BACK_TRIG_PIN, LOW);  // make trigger pin active low

  /*Measure the Echo output signal duration or pulss width */
  long duration = pulseIn(BACK_ECHO_PIN, HIGH); // save time duration value in "duration variable
  int distance = duration * SOUND_SPEED;        // Convert pulse duration into distance

  if (distance < dis)
  {
    Back_Dis = true;
  }
  else
  {
    Back_Dis = false;
  }
}