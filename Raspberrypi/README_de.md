**Die deutsche README ist nicht vollständig. Bitte die englische README nutzen. Wir suchen Freiwillige, die die README übersetzen können.**

# Ordnerstruktur

## hiverize

In dem Ordner `hiverize` ist der gesamte Python Code für die Messung und das Webinterface.

Wenn bereits ein Raspbian vorhanden ist kann dieser Ordner auf den RaspberryPi kopiert werden. Für das Programm werden folgende Pakete benötigt:

```bash
apt install python3-rpi.gpio python3-numpy python3-flask python3-pydbus python3-pip gunicorn3 network-manager wireless-tools dnsmasq zip
pip3 install python-networkmanager Adafruit_DHT paho.mqtt
```

Nachdem man die Datei `hiverize/hiverizewaage.conf` angepasst hat kann die Messung gestartet werden:

```bash
python3 -m hiverize.measure
```

Um die Messung und das Webinterface mit systemd zu steuern siehe `rpi-system`.

## rpi-system

In diesem Ordner liegen einige zusätzlichen Dateien um das Programm ohne erweiterte Rechte ausführen zu können, die Konfigurationsdateien für den WiFi Access Point und die service-Dateien für systemd. Die Ordnerstruktur entspricht dem root-Verzeichnis auf dem RaspberryPi.

Es ist zu beachten, welche Rechte die Dateien haben müssen. Für eine Auflistung aller Berechtigungen siehe die Datei `pi-gen/stage2/99-hiverize/01-run.sh`.

## pi-gen

Dieser Ordner ist eine Hilfe um mit dem Programm `pi-gen` ein angepasstet Raspbian Image zu erstellen, in dem alle Anpassung für das Hiverize-Programm enthalten sind. Für eine Anleitung siehe `pi-gen/README.md`.
