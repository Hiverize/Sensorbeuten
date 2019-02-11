import logging
from hiverize.sensors.sensor import Sensor

# Change the name of the class from Example to the name of the sensor
# Like DHT22, DS1820, etc.
class Example(Sensor):
    def __init__(self, pinout, interval, name = 'Example'):

        # Initialize the parent Sensor class
        super().__init__(interval, name)

        # This is the logger which should be used
        # It gets its name from the name supplied in the parameter 
        # of the __init__ function
        # In this case the logger is called 'hiverize.Example'
        logger = logging.getLogger('hiverize.{}'.format(self.name))

        # Set the GPIO pins this sensor uses
        # This can also be hardcoded but a parameter allows for more flexibility
        self.pinout = pinout

    def get_data(self):
        # Here goes the code how we get data from the sensor
        # This example just gives a constant value
        data = 21.123

        # The return value of this function should be the data.
        # For every measurement this is written into a queue
        # and gathered periodically by the main thread in measure.py.
        
        # If the sensor returns multiple values the function should return
        # a tuple. In this case additional code in measure.py is needed.

        return data