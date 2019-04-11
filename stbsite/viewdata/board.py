import subprocess
from glob import glob
from .data import Data
import pandas as pd
import sqlalchemy
from .values import default_voltages, PATH
from matplotlib import pyplot as plt
from sklearn import preprocessing
from scipy import stats
import numpy as np


class Board:
    def __init__(self, ID):
        self.ID = ID

    def __uploadToBoardTable(self, df, dbname):
        engine = sqlalchemy.create_engine(
            'sqlite:///'+dbname, echo=False)
        self.data_row.to_sql('board_' + self.ID,
                             con=engine, if_exists='append',
                             index=False)

    def measure(self):
        subprocess.call(
            [PATH + '/measure_all.sh', '10']
        )
        files = glob(PATH+'/data/*.dat')
        data = Data(self.ID, files[-1])
        data.getDataFrame()
        data.uploadDataToDB(PATH+'/data/db.sqlite')
        cols = ['timestamp'] + list(data.df.columns)
        row = [data.timestring] + list(data.df.mean().values)
        self.data_row = pd.DataFrame([row], columns=cols)
        self.__uploadToBoardTable(self.data_row, PATH+'/data/db.sqlite')

    def listMeasurements(self):
        engine = sqlalchemy.create_engine(
            'sqlite:///'+PATH+'/data/db.sqlite', echo=False)
        try:
            self.df = pd.read_sql(
                "SELECT * FROM board_%s" % self.ID, con=engine)
            return self.df
        except sqlalchemy.exc.OperationalError:
            return pd.DataFrame()

    def getVoltageDelta(self):
        if not hasattr(self, 'df'):
            self.listMeasurements()
        data_columns = list(self.df.columns[1:])
        data_columns.pop(data_columns.index('Not Used'))
        data_columns.pop(data_columns.index('5V_SCALED'))
        data_columns.pop(data_columns.index('+12_SCALED'))
        print(data_columns)
        delta = (self.df.loc[len(self.df)-1] - default_voltages.loc[0])
        r_delta = delta / default_voltages.loc[0]
        delta = delta[data_columns].astype(float)
        self.df_delta = pd.DataFrame(
            {'label': delta.index, 'delta': delta.values})
        self.df_delta['relative delta'] = r_delta[data_columns].values
        self.df_delta['default voltages'] = default_voltages.loc[0][
            data_columns].values

        x = pd.np.r_[self.df_delta['default voltages']]
        y = pd.np.r_[self.df_delta['delta']]
        # yerr = pd.np.r_[c['std']]
        m, c = np.polyfit(x, y, 1)
        line5 = c + m*x

        plt.scatter(x, y)
        plt.xlim(0, 13)
        plt.plot(x, line5, 'k-')
        plt.show()

        return self.df_delta
