import pandas as pd
import numpy as np
import sqlalchemy
import re
import datetime


class Data:
    def __init__(self, board_ID, filename):
        self.board_ID = board_ID
        data = self.__openFile(filename).strip().split(',')
        data = [i for i in data if i]
        try:
            self.npoints = int(data[0])
            data = data[1:]
        except ValueError:
            pass
        self.data = np.array(data)
        self.data = self.data.reshape(int(self.data.size/(self.npoints+1)),
                                      int(self.npoints + 1))
        self.data = self.data.T

    def __openFile(self, filename):
        with open(filename) as f:
            datastring = f.read()
        self.timestring = re.match(
            '[0-9]*',
            re.search('[0-9]*\.dat', filename).group()).group()
        self.timestamp = datetime.datetime.strptime(
            self.timestring, "%Y%m%d%H%M%S")
        return datastring

    def getDataFrame(self):
        self.df = pd.DataFrame(
            self.data[1:],
            columns=[i.strip() for i in self.data[0]]).astype(float)
        return self.df

    def uploadDataToDB(self, dbname):
        if not hasattr(self, 'df'):
            self.getDataFrame()
        engine = sqlalchemy.create_engine(
            'sqlite:///'+dbname, echo=False)
        try:
            self.df.to_sql(self.board_ID + '_test_' + self.timestring,
                           con=engine, if_exists='replace',
                           index_label='index')
        except ValueError:
            pass
