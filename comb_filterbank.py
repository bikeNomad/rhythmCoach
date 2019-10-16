from array import array

class CombFilterbank:
    def __init__(self, maxlen):
        self.delay_line = array('f')
        self.filters = array('f')
        self.maxlen = maxlen
        for i in range(maxlen+1):
            self.filters.append(0.0)
            self.delay_line.append(0.0)

    def add(self, num):
        self.delay_line.append(num)
        for i in range(len(self.filters)):
            self.filters[i] = self.filters[i] + self.delay_line[-(i+1)]
