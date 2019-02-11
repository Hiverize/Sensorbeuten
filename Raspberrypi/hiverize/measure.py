#!/usr/bin/python3 -u

import RPi.GPIO as GPIO
import argparse
import configparser
import signal
from datetime import datetime
import time
import os
import logging
import threading

from hiverize.sensors.hx711 import HX711
from hiverize.datalogger.csv import HiverizeCSVLogger
from hiverize.datalogger.mqtt import HiverizeMQTTLogger
from hiverize.sensors.dht22 import DHT22
from hiverize.sensors.ds1820 import DS1820

def collect_measurements(sensor_list):
    measurements = {}
    for sensor in sensor_list:
        if not sensor.queue.empty():

            data = sensor.queue.get()
            # Workaround for DHT22
            if sensor.name == 'DHT22':
                measurements['dht22-humidity'] = data[0]
                measurements['dht22-temperature'] = data[1]
            else:
                measurements[sensor.name] = data

    return measurements

def measurement_loop(sensor_list, config):
    logger = logging.getLogger('hiverize.measure.main')
    exit = threading.Event()
    def signal_handler(signum, frame):
        exit.set()
        
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)

    logger.debug('Starting measurement threads for each sensor')
    for sensor in sensor_list:
        sensor.start()

    logger.info('Starting measurements')
    
    # Initialize MQTTLogger
    mqtt_server = config['Upload']['Server']
    mqtt_topic = u'{realm}/{network}/{gateway}/{node}/data.json'.format( 
            realm = config['Upload']['Realm'],
            network = config['Upload']['Netzwerk'],
            gateway = config['Upload']['Ort'],
            node = config['Upload']['Name'] )

    mqtt_logger = HiverizeMQTTLogger(mqtt_server, mqtt_topic)

    while not exit.is_set():
        # Rotate file every hour
        file_creation_time = datetime.now()
        root_folder = config['Allgemein']['Pfad']
        if root_folder.endswith('/'):
            root_folder = root_folder[:-1]
        folder = file_creation_time.strftime('%Y%m%d')
        filename = file_creation_time.strftime('%Y%m%d_%H00')
        path = '{}/{}/{}'.format(root_folder, folder, filename)
        time_now = datetime.now()
        
        # This is way to complicated
        # We need a better way to generate the field list
        fieldnames = [ 'time', sensor_list[0].name,
                       'dht22-humidity', 'dht22-temperature', 
                       *[ s.name for s in sensor_list[2:] ] ]
                        
        with HiverizeCSVLogger(path, fieldnames, cachesize=60) as csv_logger:
            while not exit.is_set():
                measurements = collect_measurements(sensor_list)
                measurements['time'] = time_now.isoformat()
                csv_logger.append(measurements)
                mqtt_logger.send(measurements)

                waittime = (time_now.timestamp() 
                            + 1 
                            - time.time())     
                logger.debug('Wait for {:.3f}s'.format(waittime))
                
                if waittime < 0:
                    logger.warning('Due to retries obtaining a measurement in the specified interval failed')
                else:
                    exit.wait(waittime)
                
                time_now = datetime.now()
                if time_now.hour!=file_creation_time.hour:
                    logger.debug('Rotate file.')
                    break

    GPIO.cleanup()

def setup_sensors(config):
    logger = logging.getLogger('hiverize.measure.setup')

    # Initialize all sensors
    logger.debug('Initializing sensors')

    hx = HX711(dout_pin=config.getint('HX711', 'Pin_DOUT'), 
               pd_sck_pin=config.getint('HX711', 'Pin_PD_SCK'), 
               interval=1, 
               gain_channel_A=64, select_channel='A')
    
    if ('Kalibrierfaktor' and 'Taragewicht') in config['HX711']:         
        ratio = config['HX711'].getfloat('Kalibrierfaktor')
        offset = config['HX711'].getint('Taragewicht')
        hx.set_scale_ratio(channel='A', gain_A=64, scale_ratio=ratio)
        hx.set_offset(offset=offset, channel='A', gain_A=64)
    
    hx_test_measurement = hx.get_data()
    if not hx_test_measurement:
        logger.error('Initialization of HX711 failed.')
    else:
        logger.debug(
                'HX711 found. Initial measurement: {:.2f}'.format(
                    hx_test_measurement))


    dht = DHT22(pinout=config.getint('DHT22', 'Pin_DOUT'), 
                interval = config.getint('DHT22', 'Messintervall'))
    dht_test_measurement = dht.get_data()
    if False in dht_test_measurement:
        logger.error('Initialization of DHT22 failed.')
    else:
        logger.debug(
                'DHT22 found. Initial measurement: {:.2f}%, {:.1f}C'.format(
                    *dht_test_measurement))

    w1_device_dir = '/sys/bus/w1/devices/'
    ds_device_list = [ os.path.join(w1_device_dir, directory) 
                       for directory in os.listdir(w1_device_dir)
                       #for directory in [ '28-1122334455', '28-66778899aa' ] 
                       if directory.startswith('28-') ]
    ds_list = [ DS1820(path, config.getint('DS1820', 'Messintervall')) 
                for path in ds_device_list ]
    logger.debug('Found {:d} DS1820 sensors.'.format(len(ds_list)))

    sensor_list = [ hx, dht, *ds_list ]
    return sensor_list

if __name__ == "__main__":
    # Setup logging
    logger = logging.getLogger('hiverize')
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    sh = logging.StreamHandler()
    sh.setFormatter(formatter)
    logger.addHandler(sh)

    parser = argparse.ArgumentParser()
    parser.add_argument('-f', '--file', type=str, help='path to config',
                        default='/home/pi/hiverize/hiverizewaage.conf')
    loglevels = ['CRITITCAL', 'ERROR', 'WARNING', 'INFO', 'DEBUG']
    parser.add_argument('--loglevel', type=str, choices=loglevels,
                        help='set log-level', default='WARNING')
    args = parser.parse_args()
    
    # Read config file
    config = configparser.ConfigParser()
    if not config.read(args.file):
        logger.critical('Configuration file not found.')
    
    if 'Logdatei' in config['Allgemein']:
        fh = logging.FileHandler(config['Allgemein']['Logdatei'])
        fh.setFormatter(formatter)
        logger.addHandler(fh)
        
    logger.setLevel(args.loglevel)

    # Start setup process
    logger.info('Starting setup process')
    
    sensor_list = setup_sensors(config)
    measurement_loop(sensor_list, config)