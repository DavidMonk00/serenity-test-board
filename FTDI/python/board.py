import subprocess
from glob import glob
from data import Data
import pandas as pd
import sqlalchemy
from values import default_voltages


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
        # TODO: Make this command dynamic
        subprocess.call(
            ['sudo',
             'LD_LIBRARY_PATH=/usr/local/lib/:$LD_LIBRARY_PATH',
             '/home/dmonk/serenity-test-board/FTDI/bin/main', '-l']
        )
        files = glob('data/*.dat')
        data = Data(self.ID, files[-1])
        data.getDataFrame()
        data.uploadDataToDB('data/db.sqlite')
        cols = ['timestamp'] + list(data.df.columns)
        row = [data.timestring] + list(data.df.mean().values)
        self.data_row = pd.DataFrame([row], columns=cols)
        self.__uploadToBoardTable(self.data_row, 'data/db.sqlite')

    def listMeasurements(self):
        engine = sqlalchemy.create_engine(
            'sqlite:///data/db.sqlite', echo=False)
        self.df = pd.read_sql("SELECT * FROM board_%s" % self.ID, con=engine)
        return self.df

    def getVoltageDelta(self):
        if not hasattr(self, 'df'):
            self.listMeasurements()
        delta = self.df.loc[len(self.df)-1] - default_voltages.loc[0]
        print(delta.sort_values())
