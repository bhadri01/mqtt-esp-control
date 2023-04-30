#!/usr/bin/env python
import pika

# Set up the user credentials
credentials = pika.PlainCredentials('guest', 'guest')

# Establish a connection to the RabbitMQ broker
connection = pika.BlockingConnection(
    pika.ConnectionParameters(host='localhost', credentials=credentials))

# Create a channel
channel = connection.channel()

# Declare the queue
channel.queue_declare(queue='hello')

# Publish a message to the queue
channel.basic_publish(exchange='', routing_key='hello', body='Hello World!')
print(" [x] Sent 'Hello World!'")

# Close the connection
connection.close()
