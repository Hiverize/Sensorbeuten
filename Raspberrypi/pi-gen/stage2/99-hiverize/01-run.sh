#!/bin/bash -e

# Install python3 packages not available via apt
on_chroot <<EOF
pip3 install python-networkmanager
pip3 install Adafruit_DHT
pip3 install influxdb
pip3 install paho-mqtt
EOF

# Install and unpack the python package
install -m 644 files/hiverize.tar.gz "${ROOTFS_DIR}/home/pi/hiverize.tar.gz"
on_chroot <<EOF
tar -xzf /home/pi/hiverize.tar.gz -C /home/pi
chown pi:pi -R /home/pi/hiverize
EOF

# Allow the user pi to use NetworkManager without authentication
install -m 644 files/etc/polkit-1/localauthority/50-local.d/org.freedesktop.NetworkManager.pkla "${ROOTFS_DIR}/etc/polkit-1/localauthority/50-local.d/org.freedesktop.NetworkManager.pkla"

# Configure dnsmasq
cat >> "${ROOTFS_DIR}/etc/dnsmasq.conf" <<EOF
interface=wlan0
dhcp-range=10.10.10.10,10.10.10.100,12h
EOF

# Install profile for wifi ap
install -m 700 files/etc/NetworkManager/system-connections/wifi-ap "${ROOTFS_DIR}/etc/NetworkManager/system-connections/wifi-ap"

# Install dispatcher scripts
install -m 700 files/etc/NetworkManager/dispatcher.d/last-online.sh "${ROOTFS_DIR}/etc/NetworkManager/dispatcher.d/last-online.sh"

# Install uploader script and its wrapper
install -m 755 files/home/pi/upload-wrapper.sh "${ROOTFS_DIR}/home/pi/upload-wrapper.sh"
install -m 755 files/home/pi/upload.py "${ROOTFS_DIR}/home/pi/upload.py"

# Install webinterface services
install -m 644 files/etc/systemd/system/webinterface.service "${ROOTFS_DIR}/etc/systemd/system/webinterface.service"

# Install hiverize service
install -m 644 files/etc/systemd/system/hiverize.service "${ROOTFS_DIR}/etc/systemd/system/hiverize.service" 

# Allow user to enable hiverize service and restart
on_chroot <<EOF
echo "pi localhost=NOPASSWD: /bin/systemctl enable hiverize.service" >> /etc/sudoers
echo "pi localhost=NOPASSWD: /sbin/shutdown" >> /etc/sudoers
EOF

# Change SSH port

# Enable/Disable services
on_chroot <<EOF
systemctl disable dhcpcd
systemctl enable NetworkManager
systemctl enable ssh
systemctl enable webinterface
EOF

#Activate 1-Wire
cat >> "${ROOTFS_DIR}/etc/modules" << EOF
# 1-wire
w1-gpio pullup=1
w1-therm
i2c-dev
EOF

cat >> "${ROOTFS_DIR}/boot/config.txt" << EOF
dtoverlay=w1-gpio
EOF

# Change password of user pi
on_chroot << EOF
echo "pi:bienenprojekt" | chpasswd
EOF
