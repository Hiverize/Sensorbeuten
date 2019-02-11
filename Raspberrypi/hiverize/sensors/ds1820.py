# Class for the ds1820 temperature sensor
# Author of ds1820 code: Andre Jo
# Hiverize Edit

import logging
import os

from hiverize.sensors.sensor import Sensor

class DS1820(Sensor):

    #Initiate with the base_dir
    def __init__(self, device_dir, interval):   
        name = os.path.basename(device_dir)
        super().__init__(interval, name)
        self.device_dir = device_dir
        self.device_file = self.device_dir + '/w1_slave'

    def read_temp_raw(self):
        f = open(self.device_file, 'r')
        lines = f.readlines()
        f.close()
        return lines

    def read_temp(self):
        logger = logging.getLogger('hiverize.ds1820')
        logger.debug('Reading temperature')
        lines = self.read_temp_raw()
        while lines[0].strip()[-3:] != 'YES':
            lines = self.read_temp_raw()
        equals_pos = lines[1].find('t=')
        if equals_pos != -1:
            temp_string = lines[1][equals_pos+2:]
            temp = float(temp_string) / 1000.0
            return temp
        else:
            return False

    def get_data(self):
        return self.read_temp()