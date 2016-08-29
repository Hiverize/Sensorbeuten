
# Basic DHT11 code is Copyright (c) 2014 Adafruit Industries
# Author: Tony DiCola
# Author of ds1820 code:: Andre Jo
# Logging code taken from raspberrypi.org/learning
# Caro edit 2016
import os
import glob
import multiprocessing
from collections import OrderedDict
import Adafruit_DHT
import datetime
import time
from time import sleep
from math import ceil


os.system('modprobe w1-gpio')
os.system('modprobe w1-therm')

base_dir = '/sys/bus/w1/devices/'
# Save folder where data is written
save_dir = '/home/pi/data'

# ---------------------------------------------------------------------#
# Setup of DHT11 Sensor
# the ds1820 sensors have to be connected to pin GPIO 4,
# the DHT11 to pin GPIO 17.

# DHT11
# Try to grab a sensor reading.  Use the read_retry method which will retry up
# to 10 times to get a sensor reading (waiting 2 seconds between each retry).


def get_DHT11_data():
    # Note that sometimes you won't get a reading and
    # the results will be null (because Linux can't
    # guarantee the timing of calls to read the sensor).
    # If this happens try again!

    for i in range(1, 3):
        print('started')
        try:
            humidity, temperatureDHT = Adafruit_DHT.read(
                Adafruit_DHT.DHT11, 17)
            if humidity is not None and temperatureDHT is not None:
                return humidity, temperatureDHT
            sleep(2)
            print('{}. DHT Measurement failed. Starting next try'.format(i))
        except ValueError:
            print('Sensor not found')

    return "nan", "nan"

    # Note that sometimes you won't get a reading and
    # the results will be null (because Linux can't
    # guarantee the timing of calls to read the sensor).
    # If this happens try again!


# ---------------------------------------------------------------------#
# Setup of all DS1820 Sensors


def read_temp_raw(input_file):
    f = open(input_file, 'r')
    lines = f.readlines()
    f.close()
    return lines


def read_temp(input_file):
    lines = read_temp_raw(input_file)

    while lines[0].strip()[-3:] != 'YES':
        # time.sleep(0.2)
        lines = read_temp_raw(input_file)

    equals_pos = lines[1].find('t=')
    if equals_pos != -1:
        temp_string = lines[1][equals_pos+2:]
        temp_c = float(temp_string)/1000.0
        print('Done: Sensor: {}, Temp: {}'
              .format(input_file.split('/')[-2], temp_c))
        return (input_file.split('/')[-2], str(temp_c))


def print_result(result):
    for elem in result:
        print(elem)


def start_process():
    print('Starting {0}'.format(multiprocessing.current_process()
                                .name))


def writeFileHeader(tempDict):
    tempString = '##'
    tempString += ', '.join("{}".format(val) for val in tempDict.keys())
    tempString += ', Humidity, temperatureDHT ##\n'
    return tempString


def measure_all_DS1820():
    device_folder = glob.glob(base_dir + '28*')
    device_folder = ['{0}/w1_slave'.format(elem)
                     for elem in device_folder]

    pool = multiprocessing.Pool(processes=4,
                                initializer=start_process)

    pool_outputs = pool.map(read_temp, device_folder)

    result_dict = dict(pool_outputs)
    result_dict = OrderedDict(sorted(result_dict.items()))

    pool.close()  # no more tasks
    pool.join()  # wrap up current tasks

    return result_dict


def logging():

    # First take measurement of all temperature sensors in a dict
    ds1820Temps = measure_all_DS1820()
    print('temps Measured')
    # Then get Dict with DHT11 measurement
    dht11 = {}
    dht11['humidity'], dht11['temperatureDHT'] = get_DHT11_data()
    print('DHT measured')
    # Check if folder with actual date exist, if not create
    actualDateTime = datetime.datetime.today()
    folderName = save_dir + '/' + actualDateTime.date().strftime('%Y%m%d')

    print(folderName)
    try:
        os.makedirs(folderName, mode=0o777)
    except OSError, e:
	if e.errno != 17:
        	print('Error creating folder: ' + str(e) ) # errno 17= File existed 
	else: 
		pass 
    print('Folder created')
    # Form String out of measurement
    #tempString = str(ceil(datetime.datetime.timestamp(actualDateTime)))
    tempString = str(time.mktime(actualDateTime.timetuple()))
    
    tempString += ', ' + ', '.join("{}".
                                   format(val) for val in ds1820Temps.values())
    tempString += ', ' + ', '.join("{}".
                                   format(val) for val in dht11.values())

    print(tempString)
    # write to file
    fileTimeStamp = actualDateTime.strftime('%Y%m%d_%H00')
    fileName = fileTimeStamp + '_ds1820_dht11'
    print(fileName)
    if not os.path.isfile(folderName + '/' + fileName):
        loggFIle = open(folderName + '/' + fileName,  'a')
        loggFIle.write(writeFileHeader(ds1820Temps))
    else:
        loggFIle = open(folderName + '/' + fileName,  'a')
    loggFIle.write(tempString + '\n')
    loggFIle.close()


if __name__ == '__main__':
    logging()
