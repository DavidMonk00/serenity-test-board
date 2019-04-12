import pandas as pd

default_voltages = pd.DataFrame([{
    'X1_POWER_MGTVCCAUX_N': 1.8,
    'X1_POWER_MGTAVTT_S': 1.2,
    'X1_POWER_MGTAVCC_S': 0.8,
    'X1_POWER_MGTAVCC_N': 0.8,
    'X1_POWER_MGTAVTT_N': 1.2,
    'X1_POWER_VCCAUX': 1.8,
    'X1_POWER_+1.8V': 1.8,
    'X1_POWER_+3.3V': 3.3,
    'SERVICES+1.5V': 1.5,
    'SERVICES+1.0V': 1.0,
    'SERVICES+1.8V': 1.8,
    'SERVICES+3.3V': 3.3,
    '5V_SCALED': 5,
    'X1_POWER_VCCINT': 0.8,
    'X1_POWER_MGTVCCAUC_S': 1.8,
    'X1_POWER_+1.8V_ANALOGUE': 1.8,
    'X0_POWER_MGTVCCAUC_N': 1.8,
    'X0_POWER_+1.8V_ANALOGUE': 1.8,
    'X0_POWER_MGTVCCAUC_S': 1.8,
    'X0_POWER_VCCINT': 0.8,
    'X0_POWER_+3.3V': 3.3,
    'ARTIX_MGTAVTT': 1.2,
    'ARTIX_MGTAVCC': 1.0,
    'SERVICES_POWER_STANDBY_+3.3V': 3.3,
    'Not Used': 0.0,
    '+12_SCALED': 12.0,
    'X0_POWER_+1.8V': 1.8,
    'X0_POWER_VCCAUX': 1.8,
    'X0_POWER_MGTAVTT_N': 1.2,
    'X0_POWER_MGTAVCC_N': 0.8,
    'X0_POWER_MGTAVCC_S': 0.8,
    'X0_POWER_MGTAVTT_S': 1.2
}])

PATH = '/home/dmonk/serenity-test-board/FTDI'