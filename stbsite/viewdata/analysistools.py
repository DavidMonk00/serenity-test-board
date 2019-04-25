import numpy as np
from .board import Board
from .main import getBoards
from .values import default_voltages


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
    score = []
    for column in data:
        mean = data[column].mean()
        std = data[column].std()
        if (
            abs(mean-default_voltages[column].loc[0]) /
            default_voltages[column].loc[0] < tolerance
        ):
            score = 0
        elif (
            default_voltages[column].loc[0] > (mean - std) and
            default_voltages[column].loc[0] < (mean - std)
        ):
            score = 1
        else:
            score = 2
        footer.append([
            "%0.3f (%0.3f)" % (mean, std),
            score
        ])
    return footer
