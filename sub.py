import paho.mqtt.client as mqtt

broker_address = "localhost"  # Replace with your MQTT broker address
topic = "my_topic"

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe(topic)

def on_message(client, userdata, message):
    print("Received message '" + str(message.payload) + "' on topic '"
          + message.topic + "' with QoS " + str(message.qos))

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(broker_address)

client.loop_forever()
