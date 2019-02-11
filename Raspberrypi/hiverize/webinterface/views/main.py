from flask import Blueprint, render_template, send_file
import subprocess
import pathlib
import io
import zipfile

main = Blueprint('main', __name__)

@main.route('/')
def index():
    return render_template('main/index.html')

@main.route('/restart')
def restart():
    res = subprocess.check_output('sudo systemctl enable hiverize', shell=True)
    res = subprocess.check_output('sudo shutdown --reboot -t 0', shell=True)
    return 'Bye, bye'

@main.route('/data.zip')
def download_zip():
    filename = '/home/pi/hiverizelog.zip'
    res = subprocess.check_output('zip -ur /home/pi/hiverizelog.zip /home/pi/hiverizelog/', shell=True)

    return send_file(
        filename,
        mimetype='application/zip',
        as_attachment=True,
        attachment_filename='data.zip'
    )