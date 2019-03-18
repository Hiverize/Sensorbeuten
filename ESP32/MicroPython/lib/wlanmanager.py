import network
import config
import time

class WLanManager():

    def __init__(self):
        self.reload_profiles()
        self.wlan = network.WLAN(id=0)

    def reload_profiles(self):
        conf = config.Config()
        self.ap_profile = conf.config['wlan']['profiles']['ap']
        self.client_profile = conf.config['wlan']['profiles']['client']

    def scan(self):
        # Initial scan to find all available SSIDs
        self.wlan.init(mode=network.WLAN.STA)
        return self.wlan.scan()

    def enable_ap(self):
        profile = self.ap_profile
        profile['last_scan'] = self.scan()
        
        # Resolve mode and sec to their numeric codes
        mode = getattr(network.WLAN, profile.get('mode'))
        sec = getattr(network.WLAN, profile.get('sec', 'WPA2'))

        self.wlan.init(mode=mode,
                       ssid=profile.get('ssid'),
                       auth=(sec, profile.get('key', 'hiverize')),
                       channel=profile.get('channel', 1))
        self.wlan.ifconfig(id=1, 
                           config=('10.10.10.1',
                                   '255.255.255.0',
                                   '10.10.10.1',
                                   '10.10.10.1'))

    def enable_client(self):
        profile = self.client_profile

        # Resolve mode and sec to their numeric codes
        mode = getattr(network.WLAN, profile.get('mode'))
        sec = getattr(network.WLAN, profile.get('sec'))

        self.wlan.init(mode=mode)
        self.wlan.connect(profile.get('ssid'),
                          auth=(sec, profile.get('key')),
                          bssid=profile.get('bssid', None),
                          timeout=profile.get('timeout', None))
        if profile.get('config') == 'dhcp':
            self.wlan.ifconfig(id=0, config='dhcp')
        else:
            ip_config=(profile.get('ip'),
                       profile.get('subnet'),
                       profile.get('gateway'),
                       profile.get('dns'))
            self.wlan.ifconfig(id=0, config=ip_config)

        time.sleep(5)
        if not self.wlan.isconnected():
            self.enable_ap()

    
    def toggle(self):
        if self.wlan.mode() == network.WLAN.AP:
            self.enable_client()
        else:
            self.enable_ap()
