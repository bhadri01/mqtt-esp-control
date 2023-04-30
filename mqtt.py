import paho.mqtt.client as mqtt
import time

broker_address = "localhost"  # Replace with your MQTT broker address
topic = "my_topic"

client = mqtt.Client()
client.connect(broker_address)

for i in range(1000):
    message = "Value: " + str(i)
    client.publish(topic, message)
    print("Sent message '" + message + "' to topic '" + topic + "'")
    time.sleep(0.01)  # Wait for 1 second before sending the next message

client.disconnect()
