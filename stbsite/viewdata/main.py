import subprocess
import pandas as pd
import sqlalchemy
from .databasing import dropTable, listTables, viewTable
from .board import Board
from .values import PATH, DB_PATH
from sqlite3 import OperationalError


def buildCCode(clean=False):
    if clean:
        subprocess.call(['make', 'clean'])
    subprocess.call(['make'])


def createBoard(metadata):
    engine = sqlalchemy.create_engine(DB_PATH, echo=False)
    if ('boards' in listTables(PATH+'/data/db.sqlite')):
        df = pd.read_sql("SELECT * FROM boards", con=engine)
        max_key = df.Key.astype(int).max()
        key = max_key + 1
    else:
        key = 0
    cols = ['Key', 'ID', 'Version', 'Date Built', 'Deleted']
    row = [str(key)] + metadata + [False]
    print(row)
    df = pd.DataFrame([row], columns=cols)
    df.to_sql('boards', con=engine, if_exists='append', index=False)
    Board(str(key))


def getBoards():
    engine = sqlalchemy.create_engine(DB_PATH, echo=False)
    try:
        df = pd.read_sql("SELECT * FROM boards WHERE Deleted=0", con=engine)
    except Exception:
        df = pd.DataFrame([])
    return df


def main():
    buildCCode(clean=False)
    boards = getBoards()
    print(boards)
    board = Board(boards.ID.values[0])
    board.measure()
    # print(sum(np.abs(board.getVoltageDelta()['delta'])))
    # print(listTables('data/db.sqlite'))


if __name__ == '__main__':
    main()
