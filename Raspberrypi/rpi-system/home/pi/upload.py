#!/usr/bin/python3 -u

import pandas as pd
import numpy as np
import os
import paho.mqtt.publish as publish
import paho.mqtt.client as mqtt
import time
import argparse
import configparser
import logging
from datetime import datetime, timedelta

# Manage a queue of files to upload
# Format is one filename per line
def load_queue(path):
    with open(path, 'r') as queue_file:
        files = queue_file.readlines()
    return files

def save_queue(path, files):
    with open(path, 'w') as queue:
        for f in files:
            queue.write('{}'.format(f))

def upload_queue(config, skip_current):
    try:
        mqtt_server = config['Upload']['Server']
        mqtt_topic = u'{realm}/{network}/{gateway}/{node}/data.json'.format( 
            realm = config['Upload']['Realm'],
            network = config['Upload']['Netzwerk'],
            gateway = config['Upload']['Ort'],
            node = config['Upload']['Name'] )

        path = config['Allgemein']['Pfad']
    except:
        print("Failed to read parameters from the config file.")
        raise
        
    queue_path = 'queue.txt'
    files = load_queue(queue_path)
    
    if not skip_current:
        t = datetime.now() - timedelta(hours=1)
        last_hour = ( path + t.strftime('%Y%m%d') 
                    + '/' + t.strftime('%Y%m%d_%H00') )
        files.append('{}\n'.format(last_hour))
    
    while files:
        filename = files.pop().rstrip('\n')
        try:
            upload_file(filename, mqtt_server, mqtt_topic)
        except:
            print('Current file: {}'.format(filename))
            raise
        finally:
            save_queue(queue_path, files)


def upload_file(filename, server, topic):
    data = pd.read_csv( filename, header=None, 
                        names=['date', 'weight', 'dht-humidity', 'dht-temperature', 'ds1820-1', 'ds1820-2', 'ds1820-3', 'ds1820-4'], 
                        na_values='False', 
                        dtype={0: str, 1: np.float64, 2: np.float64, 3: np.float64, 4: np.float64, 5: np.float64, 6: np.float64, 7: np.float64} )
    data = data.dropna()
    
    # Prepare a list of messages where each entry 
    # is a row in the csv.
    msgs = [ '{{"time": "{}", "weight": "{}"}}'.format(
                  row.date, row.weight)
            for idx, row in data.iterrows() ]

    # Connect to server and send each row with a delay
    # Without the delay we get a "Connection pool is full"
    # error, which is a problem between Kotori and InfluxDB.
    # Manual testing showed we can submit 50 messages per 
    # second without this happening.
    # Note that this might be a problem if we sedn data from
    # multiple devices!
    backend = mqtt.Client(clean_session=True)
    backend.connect(server)
    for row in msgs:
        msg = backend.publish(topic, row)
        msg.wait_for_publish()
        time.sleep(0.02)
    backend.disconnect()

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-f', '--file', type=str, help='path to config',
                        default='/home/pi/hiverize/hiverizewaage.conf')
    loglevels = ['CRITITCAL', 'ERROR', 'WARNING', 'INFO', 'DEBUG']
    parser.add_argument('--loglevel', type=str, choices=loglevels,
                        help='set loglevel', default='WARNING')
    parser.add_argument('--skip', help='skip upload of the last hour', 
                        action="store_true")
    args = parser.parse_args()
    
    # Read config file
    config = configparser.ConfigParser()
    config.read(args.file)
    
    # Setup logging
    logger = logging.getLogger('__main__')
    logger.addHandler(logging.StreamHandler())
    upload_queue(config, args.skip)
