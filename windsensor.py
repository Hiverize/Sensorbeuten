#!/usr/bin/python3
import spidev
import time

spi = spidev.SpiDev()
spi.open(0, 0)
supplyVoltage = 5
zeroWindAdjustment = 0.2


def read_data(channel):
    spidata = spi.xfer2([64 + channel * 32, 0])
    analog_readout_speed = ((spidata[0] & 3) << 8) + spidata[1]
    return analog_readout_speed


def read_windspeed():
    RV_windspeed_AD = read_data(0)
    RV_windspeed_V = RV_windspeed_AD * supplyVoltage / 1024

    TMP_rawData = read_data(1)
    zeroWind_AD = -0.0006*TMP_rawData * TMP_rawData
    + 1.0727 * TMP_rawData + 47.172
    zeroWind_V = (zeroWind_AD * supplyVoltage / 1024) - zeroWindAdjustment

    windSpeed_MPH = pow(((RV_windspeed_V - zeroWind_V) / 0.2300), 2.7265)
    windSpeed_KMH = windSpeed_MPH * 1.60934

    print('WindSpeed: {} km/h'.format(windSpeed_KMH))


if __name__ == '__main__':
    while True:
        read_windspeed()
        sleep(10)
