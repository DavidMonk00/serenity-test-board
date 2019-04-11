import subprocess
import pandas as pd
import sqlalchemy
from .databasing import dropTable, listTables, viewTable
from .board import Board
from .values import PATH


def buildCCode(clean=False):
    if clean:
        subprocess.call(['make', 'clean'])
    subprocess.call(['make'])


def createBoard(metadata):
    engine = sqlalchemy.create_engine(
        'sqlite:///'+PATH+'/data/db.sqlite', echo=False)
    if ('boards' in listTables(PATH+'/data/db.sqlite')):
        df = pd.read_sql("SELECT * FROM boards", con=engine)
        max_id = df.ID.astype(int).max()
        id = max_id + 1
    else:
        id = 0
    cols = ['ID', 'Version', 'Date Built']
    row = [str(id)] + metadata
    df = pd.DataFrame([row], columns=cols)
    df.to_sql('boards', con=engine, if_exists='append', index=False)


def getBoards():
    engine = sqlalchemy.create_engine(
        'sqlite:///'+PATH+'/data/db.sqlite', echo=False)
    df = pd.read_sql("SELECT * FROM boards", con=engine)
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
