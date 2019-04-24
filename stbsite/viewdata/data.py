import pandas as pd
import numpy as np
import sqlalchemy
import re
import datetime
from .values import DB_PATH


class Data:
    def __init__(self, board_ID, filename):
        self.board_ID = board_ID
        self.__openFile(filename)

    def __openFile(self, filename):
        self.df = pd.read_json(filename)
        self.timestring = re.match(
            '[0-9]*',
            re.search('[0-9]*\.json', filename).group()).group()
        self.timestamp = datetime.datetime.strptime(
            self.timestring, "%Y%m%d%H%M%S")
        return self.df

    def uploadDataToDB(self):
        engine = sqlalchemy.create_engine(DB_PATH, echo=False)
        try:
            self.df.to_sql(self.board_ID + '_test_' + self.timestring,
                           con=engine, if_exists='replace',
                           index_label='index')
        except ValueError:
            pass
