import network
import time
from config import Config
from wlanmanager import WLanManager
from machine import Pin

def toggle_wlan_mode(arg):
    print('Toggle wlan')
    getattr(arg, 'toggle')()

if __name__ == '__main__':
    # Set up Button on Ext Board to toggle WLan AP
    wlan = WLanManager()
    button_s1 = Pin('P10', mode=Pin.IN, pull=Pin.PULL_UP)
    button_s1.callback(Pin.IRQ_RISING, handler=toggle_wlan_mode, arg=wlan)
