import subprocess
# import os
import json
from .values import PATH
from numpy.random import random


class Diagnostics:
    def __init__(self):
        pass

    def checkFTDIOnline(self):
        return b'FT2232' in subprocess.check_output('lsusb')

    # def checkCodeUpToDate(self):
    #     os.chdir(PATH)
    #     return subprocess.check_output(['make'])

    def testI2CFanout(self):
        if random() > 0.5:
            test_value = '0x80'
            test_value_b = b'0x80'
        else:
            test_value = '0x8'
            test_value_b = b'0x8'
        with open(PATH+'/data/status.json', 'r') as json_file:
            data = json.load(json_file)
            try:
                subprocess.call([
                    PATH+'/bin/main', '-a', '0xE0', '-d', test_value, '-w'
                ])
                stdout = subprocess.check_output([
                    PATH+'/bin/main', '-a', '0xE0', '-r'
                ])
                if (b'done' in stdout and test_value_b in stdout):
                    data['i2c_fanout'] = True
                else:
                    data['i2c_fanout'] = False
            except subprocess.CalledProcessError:
                pass
        with open(PATH+'/data/status.json', 'w') as json_file:
            json.dump(data, json_file)
        with open(PATH+'/data/status.json', 'r') as json_file:
            data = json.load(json_file)
            return data['i2c_fanout']

    def testADC(self):
        with open(PATH+'/data/status.json', 'r') as json_file:
            data = json.load(json_file)
            try:
                stdout = subprocess.check_output([
                    PATH+'/bin/main', '-c', '-N', '1'
                ])
                try:
                    out = stdout.decode().split(" ")
                    data['adc_reading'] = out[2] + " V"
                except IndexError:
                    data['adc_reading'] = 'ERROR'
            except subprocess.CalledProcessError:
                pass
        with open(PATH+'/data/status.json', 'w') as json_file:
            json.dump(data, json_file)
        with open(PATH+'/data/status.json', 'r') as json_file:
            data = json.load(json_file)
            return data['adc_reading']
