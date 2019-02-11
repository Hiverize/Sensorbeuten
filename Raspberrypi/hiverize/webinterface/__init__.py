from flask import Flask
from .views.main import main
from .views.network import network
from .views.sensors import sensors

app = Flask(__name__)
app.register_blueprint(main)
app.register_blueprint(network, url_prefix='/network')
app.register_blueprint(sensors, url_prefix='/sensors')