import logging
import threading
import time
import queue

# Parent class for every sensor
# Schedules measurements in a separate thread 
class Sensor(threading.Thread):
    def __init__(self, interval, name):
        self.interval = interval
        self.queue = queue.Queue(1)
        super().__init__(name=name, daemon=True)
        
    def run(self):
        logger = logging.getLogger('hiverize.{}'.format(self.name))
        logger.debug('Started a new measurement thread')
        exit = threading.Event()
        while not exit.is_set():
            now = time.time()
            data = self.get_data()
            self.queue.put(data)
            waittime = (now - time.time() + self.interval)
            logger.debug('Wait for {:.2f} seconds'.format(waittime))
            exit.wait(waittime)
                   
    def get_data(self):
        pass