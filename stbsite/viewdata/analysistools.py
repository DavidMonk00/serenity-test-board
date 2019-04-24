import numpy as np
import pandas as pd
from datetime import datetime
from .board import Board
from .main import getBoards
from .values import default_voltages_list


def getBoard(board_id):
    boards = getBoards()
    board = Board(boards.loc[boards.ID == str(board_id)].ID.values[0])
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
    tolerance = 0.05
    np_data = np.array(data)
    mean = np.mean(np_data, axis=0)
    std = np.std(np_data, axis=0)
    footer = []
    score = [0]
    for i in range(len(default_voltages_list)):
        if (
            abs(mean[i+1]-default_voltages_list[i]) /
            default_voltages_list[i] < tolerance
        ):
            score.append(0)
        elif (
            default_voltages_list[i] > (mean[i+1] - std[i+1]) and
            default_voltages_list[i] < (mean[i+1] + std[i+1])
        ):
            score.append(1)
        else:
            score.append(2)

    for i in range(len(np_data.T)):
        footer.append([
            "%0.3f (%0.3f)" % (mean[i], std[i]),
            score[i]
        ])
    return footer
