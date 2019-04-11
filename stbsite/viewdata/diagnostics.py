import subprocess
import os
from .values import PATH


class Diagnostics:
    def __init__(self):
        pass

    def checkFTDIOnline(self):
        return b'FT2232' in subprocess.check_output('lsusb')

    def checkCodeUpToDate(self):
        os.chdir(PATH)
        return subprocess.check_output(['make'])

    def testI2CFanout(self):
        os.chdir(PATH)
