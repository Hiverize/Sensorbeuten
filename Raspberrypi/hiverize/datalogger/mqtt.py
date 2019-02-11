import paho.mqtt.publish as publish
import paho.mqtt.client as mqtt
import logging
import json

class HiverizeMQTTLogger:

    def __init__(self, mqtt_server, mqtt_topic):
        self.mqtt_server = mqtt_server
        self.mqtt_topic = mqtt_topic
        self.backend = mqtt.Client(clean_session=True)
        self.backend.connect(mqtt_server)

    def send(self, data):
        self.backend.publish(self.mqtt_topic, json.dumps(data))