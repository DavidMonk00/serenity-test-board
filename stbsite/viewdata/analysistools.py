import numpy as np
import pandas as pd
from datetime import datetime
from .board import Board
from .main import getBoards


def getBoard(board_id):
    boards = getBoards()
    board = Board(boards.ID.values[board_id])
    return board


def displayDataTable(df_func):
    df = df_func()
    context = {
        'header': list(df.columns),
        'table_width': len(list(df.columns)),
        'results': df.values.tolist(),
    }
    return context


def suffix(d):
    return 'th' if 11 <= d <= 13 else {1: 'st', 2: 'nd', 3: 'rd'}.get(
        d % 10, 'th')


def custom_strftime(format, t):
    return t.strftime(format).replace('{S}', str(t.day) + suffix(t.day))


def getFooterStats(data):
    np_data = np.array(data)
    mean = np.mean(np_data, axis=0)
    std = np.std(np_data, axis=0)
    footer = [
        "%0.3f (%0.3f)" % (mean[i], std[i]) for i in range(len(np_data.T))]
    return footer
