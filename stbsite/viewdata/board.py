import subprocess
from glob import glob
from .data import Data
import pandas as pd
import sqlalchemy
from .values import default_voltages, PATH, DB_PATH
from .databasing import dropTable, listTables
from matplotlib import pyplot as plt
import numpy as np


class Board:
    def __init__(self, key):
        self.key = key

    def deleteBoard(self):
        engine = sqlalchemy.create_engine(DB_PATH, echo=False)
        engine.execute(
            "UPDATE '%s' SET Deleted=1 WHERE Key = %s" % ('boards', self.key)
        )
        dropTable(PATH+'/data/db.sqlite', 'board_%s' % self.key)
        for i in listTables(PATH+'/data/db.sqlite'):
            if ('%s_' % self.key) in i:
                dropTable(PATH+'/data/db.sqlite', i)

    def __uploadToBoardTable(self, df, type):
        engine = sqlalchemy.create_engine(
            DB_PATH, echo=False)
        self.data_row.to_sql('board_' + self.key + "_" + type,
                             con=engine, if_exists='append',
                             index=False)

    def measure(self, type='all', N='10'):
        subprocess.call(
            [PATH + '/bin/main', '--list', '%s' % type, '-N', N]
        )
        files = glob(PATH+'/data/readings/%s*.json' % type)
        data = Data(self.key, files[-1])
        data.uploadDataToDB()
        cols = ['timestamp'] + list(data.df.columns)
        row = ["%s" % (data.timestring)] + list(data.df.mean().values)
        self.data_row = pd.DataFrame([row], columns=cols)
        self.__uploadToBoardTable(self.data_row, type)

    def listMeasurements(self, type='all'):
        engine = sqlalchemy.create_engine(DB_PATH, echo=False)
        try:
            self.df = pd.read_sql(
                "SELECT * FROM board_%s_%s" % (self.key, type), con=engine)
            return self.df
        except sqlalchemy.exc.OperationalError:
            return pd.DataFrame()

    def getVoltageDelta(self):
        if not hasattr(self, 'df'):
            self.listMeasurements()
        data_columns = list(self.df.columns[1:])
        data_columns.pop(data_columns.index('Not Used'))
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
