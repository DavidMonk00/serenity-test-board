import subprocess
from glob import glob
import numpy as np
from data import Data
from board import Board


path = '/home/dmonk/serenity-test-board/FTDI/'

CHANNEL_LIST = [
    ['X1_POWER_MGTVCCAUX_N',
     'X1_POWER_MGTAVTT_S',
     'X1_POWER_MGTAVCC_S',
     'X1_POWER_MGTAVCC_N',
     'X1_POWER_MGTAVTT_N',
     'X1_POWER_VCCAUX',
     'X1_POWER_+1.8V',
     'X1_POWER_+3.3V'],
    ['SERVICES+1.5V',
     'SERVICES+1.0V',
     'SERVICES+1.8V',
     'SERVICES+3.3V',
     '5V_SCALED',
     'X1_POWER_VCCINT',
     'X1_POWER_MGTVCCAUC_S',
     'X1_POWER_+1.8V_ANALOGUE'],
    ['X0_POWER_MGTVCCAUC_N',
     'X0_POWER_+1.8V_ANALOGUE',
     'X0_POWER_MGTVCCAUC_S',
     'X0_POWER_VCCINT',
     'X0_POWER_+3.3V',
     'ARTIX_MGTAVTT',
     'ARTIX_MGTAVCC',
     'SERVICES_POWER_STANDBY_+3.3V'],
    ['Not Used',
     '+12_SCALED',
     'X0_POWER_+1.8V',
     'X0_POWER_VCCAUX',
     'X0_POWER_MGTAVTT_N',
     'X0_POWER_MGTAVCC_N',
     'X0_POWER_MGTAVCC_S',
     'X0_POWER_MGTAVTT_S']
]


class CallibrationBoard(Board):
    def measure(self, input_voltage, connected_pins):
        # TODO: Make this command dynamic
        # THIS IS NOT SECURE OR PORTABLE
        stdout = subprocess.check_output(
            ['sudo', '-S',
             'LD_LIBRARY_PATH=/usr/local/lib/:$LD_LIBRARY_PATH',
             '/home/dmonk/serenity-test-board/FTDI/bin/main', '-l',
             '<', path+'psswd']
        )
        print(stdout)
        files = glob(path + 'data/*.dat')
        data = Data(self.ID, files[-1])
        return data.getDataFrame()[connected_pins]


def runCallibration():
    c_board = CallibrationBoard(0)
    results = c_board.measure(3.0, CHANNEL_LIST[0]+CHANNEL_LIST[1][3:])
    print(results.describe())


if __name__ == '__main__':
    runCallibration()
