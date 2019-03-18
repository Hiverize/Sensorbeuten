import time
from machine import Pin, I2C, Timer
import binascii

import onewire
import sensors.ds18x20
import sensors.bme280
import sensors.hx711

import logger.csv

from microDNSSrv import MicroDNSSrv
from microWebSrv import MicroWebSrv


MicroDNSSrv.Create({ '*' : '10.10.10.1' })
mws = MicroWebSrv(bindIP='10.10.10.1')
mws.Start(threaded=True)
mws.SetNotFoundPageUrl("http://hiverize.wifi")

@MicroWebSrv.route('/test')
def handlerFuncGet(httpClient, httpResponse) :
    print("TEST")

ow = onewire.OneWire(Pin('P3'))
ds = sensors.ds18x20.DS18X20(ow)

hx = sensors.hx711.HX711('P9', 'P11')

i2c = I2C(0, I2C.MASTER, pins=('P21', 'P22'))
bme = sensors.bme280.BME280(address=0x77, i2c=i2c)

perf = Timer.Chrono()

# Start CSV logger on SD
# Pins are Dat0: P8, SCLK: P23, CMD: P4, at least I think so
# Apparently Pins can not be changed, when using the SD module
csv_logger = logger.csv.CSV_logger()
while True:
    perf.start()
    temps = {}
    for rom in ds.roms:
        ds.start_conversion(rom=rom)
    time.sleep_ms(750)
    for rom in ds.roms:
        ds_measurement = ds.read_temp_async(rom=rom)
        temps[binascii.hexlify(rom).decode('utf-8')] = ds_measurement
        csv_logger.add(binascii.hexlify(rom).decode('utf-8'), ds_measurement)
    print(temps)
#     print(bme.values)
    hx_measurement = hx.read_average(times=10)
    print(hx_measurement)
    csv_logger.add("hx711", hx_measurement)
    perf.stop()
    print('Seconds elapsed: {:.4f}'.format(perf.read()))
    perf.reset()
    time.sleep(5)
