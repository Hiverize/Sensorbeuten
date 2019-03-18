from machine import SD
import os
import time

class CSV_logger:
    def __init__(self, dir="/sd/hiverizelog"):
        self.dir = dir
        # Mount SD
        # Pins are Dat0: P8, SCLK: P23, CMD: P4, at least I think so
        # Apparently Pins can not be changed
        sd = SD()
        os.mount(sd, '/sd')
        # Check if directory, eg. hiverizelog, was already created,
        # and create it, if not existing:
        try:
            os.listdir(dir)
        except OSError:
            os.mkdir(dir)
        print("Initialised CSV logger in directory " +dir)


    def add(self, sensor, value):
        # Get Time
        write_time = time.time()
        time_list = time.localtime(write_time)
        time_string = "{}-{}-{}H{}".format(time_list[0], time_list[1], time_list[2], time_list[3])
        # concat filepath
        file_path = self.dir + "/" +time_string + ".csv"
        # Write header, if file did not exist before
        try:
            f = open(file_path, 'r')
            f.close()
        except OSError:
            f = open(file_path, 'w')
            f.write('Time, Sensor, Value\n')
            f.close()
            print("Logging measurements to " +file_path)
        # Get full timestamp
        full_time_string = "{}-{}-{} {}:{}:{}".format(*time_list[0:6])
        # Append Value
        f = open(file_path, 'a')
        f.write("{}, {}, {}\n".format(full_time_string, sensor, value))
        f.close()
