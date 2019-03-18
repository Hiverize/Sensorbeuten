import ujson

class Config:
    def __init__(self, path=None):
        if not path:
            self.path = 'hiverize.json'
        else:
            self.path = path
        self.read()

    def read(self):
        with open(self.path, 'r') as file_:
            self.config = ujson.load(file_)
    
    def write(self, data):
        with open(self.path, 'w') as file_:
            file_.write(ujson.dumps(file_))