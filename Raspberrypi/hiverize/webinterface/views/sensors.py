from flask import Blueprint, render_template, request, redirect, url_for
import configparser
from multiprocessing import Pool

from hiverize.sensors.hx711 import HX711
from hiverize.measure import setup_sensors

sensors = Blueprint('sensors', __name__)

config = configparser.ConfigParser()
config.read('/home/pi/hiverize/hiverizewaage.conf')

@sensors.route('/')
def index():
    sensor_list = setup_sensors(config)

    measurements = {}
    for sensor in sensor_list:
        # Workaround for DHT22
        # It is the only sensor which returns a tuple
        if sensor.name == 'DHT22':
            data = sensor.get_data()
            measurements['dht22-humidity'] = data[0]
            measurements['dht22-temperature'] = data[1]
        else:
            measurements[sensor.name] = sensor.get_data()

    return render_template('sensors/index.html', measurements=measurements)

@sensors.route('/calibrate-scale/<int:step>', methods=['GET'])
def calibrate_scale(step):
    hx = HX711(dout_pin=config.getint('HX711', 'Pin_DOUT'), 
               pd_sck_pin=config.getint('HX711', 'Pin_PD_SCK'), 
               interval=1, 
               gain_channel_A=64, select_channel='A')
    if step==1:
        config['HX711']['Taragewicht'] = '{:d}'.format(int(hx.get_data_mean(5)))
    elif step==2:
        config['HX711']['Kalibrierfaktor'] = str(
                hx.get_data_mean(5) - config['HX711'].getint('Taragewicht'))
        with open('/home/pi/hiverize/hiverizewaage.conf', 'w') as conf_file:
            config.write(conf_file)
        return redirect(url_for('sensors.index'))

    return render_template('sensors/calibrate-scale.html', step=step)