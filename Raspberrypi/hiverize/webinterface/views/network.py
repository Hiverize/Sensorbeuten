import NetworkManager
import subprocess
import re
import uuid
import ipaddress
from flask import Blueprint, render_template, request, redirect, url_for

network = Blueprint('network', __name__)

nm = NetworkManager.NetworkManager
nm_settings = NetworkManager.Settings
wlan0 = nm.GetDeviceByIpIface('wlan0')
eth0 = nm.GetDeviceByIpIface('eth0')
nm_connections = {}

def getConnectionByName(name):
    global nm_settings
    for connection in nm_settings.ListConnections():
        settings = connection.GetSettings()
        connection_id = settings['connection']['id']
        connection_uuid = settings['connection']['uuid']
        if connection_id == name:
            return connection
    return None


def wifi_scan():
    # Create a listwith all visible SSIDS
    # A bit hacky, but NetworkManager won't scan while in AP mode
    res = subprocess.check_output('sudo iw dev wlan0 scan', 
                                   shell=True)
    ap_in_res = re.split('BSS [0-9a-f:]{17}.*\n', res.decode('utf-8'))
    ap_list = []

    re_ssid = re.compile('SSID: (.+)\n')
    for ap in ap_in_res[1:]:
        ssid = re.search(re_ssid, ap).group(1)
        if re.search('\tcapability:.*Privacy.*\n', ap) is not None:
            encryption = True
            if re.search('\t(RSN|WPA):', ap) is not None:
                key_mgmt = 'wpa-psk'
            else:
                key_mgmt = 'none'
        else:
            encryption = False
            key_mgmt = None
        ap_list.append({'ssid': ssid, 
                        'encryption': encryption, 
                        'key-mgmt': key_mgmt})
    
    return ap_list

def is_server_reachable(url):
    try:
        subprocess.check_call(["ping", "-c", "1", url])
    except subprocess.CalledProcessError:
        return False
    else:
        return True

@network.route('/')
def index():
    # Check if an internet connection is available 
    # and if the hiverize server can be reached
    hiverize_server_reachable = is_server_reachable(
            'bhive.informatik.uni-bremen.de')
    if not hiverize_server_reachable:
        cloudflare_reachable = is_server_reachable('1.1.1.1')
    else:
        cloudflare_reachable = True
    
    print(hiverize_server_reachable)

    eth0_connected = (eth0.State==100)
    if eth0_connected:
        eth0_ipv4 = eth0.Ip4Config.AddressData
    else: 
        eth0_ipv4 = None
        
    wlan0_connected = (wlan0.State==100)
    wlan0_ap = (wlan0.Mode==3)
    if wlan0_connected:
        wlan0_ipv4 = wlan0.Ip4Config.AddressData
    else:
        wlan0_ipv4 = None
    
    return render_template('network/index.html', 
                           hiverize_server_reachable=hiverize_server_reachable,
                           cloudflare_reachable=cloudflare_reachable,
                           eth0_connected=eth0_connected,
                           eth0_ipv4=eth0_ipv4,
                           wlan0_ap=wlan0_ap,
                           wlan0_connected=wlan0_connected,
                           wlan0_ipv4=wlan0_ipv4
                           ) 
    
@network.route('/lan', methods=['GET', 'POST'])
def lan_setup():
    return render_template('network/setup.html',
                           interface_name='eth0',
                           id='wired')

    
@network.route('/wifi', methods=['GET', 'POST'])
def wifi_setup():
    ap_list = wifi_scan()
    if request.method == 'GET':
        # Check if internet is available (maybe a cable is connected)
        network_state = nm.CheckConnectivity()
        
        return render_template('network/ap_list.html', 
                               title='SSID wählen', 
                               network_state=network_state,
                               ap_list=ap_list)
    else:
        if request.form['hidden_ssid']:
            ap_info = {'ssid': request.form['hidden_ssid'],
                       'encryption': True,
                       'key-mgmt': 'wpa-psk'}

        else:
            ssid = request.form['ssid']
            for ap in ap_list:
                if ap['ssid'] == ssid:
                    ap_info = ap
                    break

        return render_template('network/setup.html',
                               interface_name='wlan0',
                               id='wifi',
                               ap_info=ap_info)
    
@network.route('/save', methods=['POST'])
def save():
    connection = getConnectionByName(request.form['id'])
    if connection is not None:
        connection.Delete()

    network_settings = {'connection': {}}
    network_settings['connection']['uuid'] = str(uuid.uuid1())
    network_settings['connection']['id'] = request.form['id']
    network_settings['connection']['autoconnect'] = True
    network_settings['connection']['autoconnect-priority'] = 1
    interface_name = request.form['interface-name']
    network_settings['connection']['interface-name'] = interface_name
    if interface_name == 'wlan0':
        network_settings['connection']['type'] = '802-11-wireless'
        network_settings['802-11-wireless'] = {'ssid': request.form['ssid']}

        if request.form['encryption']:
            network_settings['802-11-wireless-security'] = {
                'key-mgmt': request.form['key-mgmt'],
                'psk': request.form['psk']
            }
        
    elif interface_name == 'eth0':
        network_settings['connection']['type'] = '802-3-ethernet'

    network_settings['ipv4'] = {'method': request.form['ipv4_method']}
    if request.form['ipv4_method'] == 'manual':
        network_settings['ipv4']['address-data'] = [{
                'address': request.form['address'],
                'prefix': request.form['prefix']}]
        network_settings['ipv4']['gateway'] = request.form['gateway']
        network_settings['ipv4']['dns'] = [ request.form['dns'] ]

    connection = nm_settings.AddConnection(network_settings)
    if interface_name == 'wlan0':    
        nm.ActivateConnection(connection, wlan0, '/')
    else:
        nm.ActivateConnection(connection, eth0, '/')

    return redirect( url_for('network.index') )
