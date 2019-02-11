#!/usr/bin/python3 -u

import csv
import os
import logging

# Log rows of a 2d list to a CSV file
# Note:
#   The cachesize decides how often data is written to the file.
#   If the program crashes data which is not written may be lost.
#   Many small writes lead to a high wear on the flash memory.
class HiverizeCSVLogger:
    
    def __init__(self, path, fieldnames, cachesize=1):
        self.logger = logging.getLogger('hiverize.csv_logger')
        self.logger.debug('Created new HiverizeCSVLogger')
        
        self.fieldnames = fieldnames
        self.logger.debug('Fieldnames: {}'.format(repr(self.fieldnames)))
        self.data = []
        self.last_write = 0
        self.cachesize = cachesize
        self.path = path
        folder, filename = os.path.split(path)
        if os.path.isdir(folder):
            return
        try:
            os.makedirs(folder, exist_ok=True)
        except OSError as e:
            self.logger.exception('Unable to create folder')
        finally:
            self.logger.debug('Folder {} should now exist'.format(folder))
        self.write_header()
            
    def __enter__(self):
        return self
        
    def __exit__(self, exc_type, exc_value, traceback):
        self.logger.debug('Write data and close CSV file')
        self.write()
        
    def append(self, row):
        if row and type(row) is dict:
            self.data.append(row)
            self.logger.debug('Appended a row: {}'.format(repr(row)))
        else:
            self.logger.warning('Empty row or wrong type')
        
        if len(self.data)-self.last_write >= self.cachesize:
            self.logger.debug('Cached data exceeds cachesize, write data to file')
            self.write()
            
    def write(self):
        with open(self.path, 'a') as csv_file:
            writer = csv.DictWriter(csv_file, fieldnames=self.fieldnames)
            writer.writerows(self.data[self.last_write:])
            
        self.logger.debug('Wrote data {:d} to {:d} to file'.format(
                self.last_write, len(self.data)))
        self.last_write = len(self.data)

    def write_header(self):
        with open(self.path, 'a') as csv_file:
            writer = csv.DictWriter(csv_file, fieldnames=self.fieldnames)
            writer.writeheader()