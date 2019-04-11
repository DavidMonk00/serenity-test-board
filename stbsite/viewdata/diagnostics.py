import subprocess
import os
from .values import PATH
from numpy.random import random


class Diagnostics:
    def __init__(self):
        pass

    def checkFTDIOnline(self):
        return b'FT2232' in subprocess.check_output('lsusb')

    def checkCodeUpToDate(self):
        os.chdir(PATH)
        return subprocess.check_output(['make'])

    def testI2CFanout(self):
        if random() > 0.5:
            test_value = '0x80'
            test_value_b = b'0x80'
        else:
            test_value = '0x8'
            test_value_b = b'0x8'
        try:
            subprocess.call([
                PATH+'/bin/main', '-a', '0xE0', '-d', test_value, '-w'
            ])
            stdout = subprocess.check_output([
                PATH+'/bin/main', '-a', '0xE0', '-r'
            ])
            return (b'done' in stdout and test_value_b in stdout)
        except subprocess.CalledProcessError:
            return False

    def testADC(self):
        try:
            stdout = subprocess.check_output([
                PATH+'/bin/main', '-c', '-N', '1'
            ])
        except subprocess.CalledProcessError:
            return 'ERROR'
        try:
            out = stdout.decode().split(" ")
            return out[2] + " V"
        except IndexError:
            return 'ERROR'
