# Class for the DHT11 humidity/temperature sensor
# Basic DHT11 code is Copyright (c) 2014 Adafruit Industries
# Author: Tony DiCola
# Hiverize edit

import Adafruit_DHT
import logging

from hiverize.sensors.sensor import Sensor

class DHT22(Sensor):

    #Initiate with the GPIO-Pin
    def __init__(self, pinout, interval, name = 'DHT22'):
        super().__init__(interval, name)
        self.pinout = pinout

    #Get the Data
    def get_data(self):
        logger = logging.getLogger('hiverize.dht22')
        for i in range(1, 3):
            logger.debug('Reading data')
            try:
                humidity, temperature = Adafruit_DHT.read(
                    Adafruit_DHT.DHT22, self.pinout)
                if humidity is not None and temperature is not None:
                    return humidity, temperature
                #time.sleep(2)
                logger.warning('{}. DHT Measurement failed. Starting next try'.format(i))
            except ValueError:
                logger.debug('Sensor not found')

        return False, False