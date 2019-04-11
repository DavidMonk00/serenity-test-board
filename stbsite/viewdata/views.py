from django.shortcuts import render
from django.http import JsonResponse
from .main import getBoards
from .board import Board
from .databasing import viewTable
from .values import PATH
from datetime import datetime
from .diagnostics import Diagnostics

# Create your views here.


def displayDataTable(df_func):
    df = df_func()
    context = {
        'header': list(df.columns),
        'table_width': len(list(df.columns)),
        'results': list(df.values),
    }
    return context


def suffix(d):
    return 'th' if 11 <= d <= 13 else {1: 'st', 2: 'nd', 3: 'rd'}.get(
        d % 10, 'th')


def custom_strftime(format, t):
    return t.strftime(format).replace('{S}', str(t.day) + suffix(t.day))


def index(request):
    context = displayDataTable(getBoards)
    return render(request, 'index.html', context)


def board(request, board_id):
    boards = getBoards()
    board = Board(boards.ID.values[board_id])
    context = displayDataTable(board.listMeasurements)
    context['board_id'] = board_id
    return render(request, 'board.html', context)


def data(request, board_id, timestring):
    boards = getBoards()
    board = Board(boards.ID.values[board_id])
    context = displayDataTable(board.listMeasurements)
    date_fmt = datetime.strptime(timestring, '%Y%m%d%H%M%S')
    print(date_fmt)
    context['date_fmt'] = custom_strftime(
        '%A {S} %B %Y at %-I:%M:%S %p',
        date_fmt)
    context['board_id'] = board_id
    context['timestring'] = timestring
    context['header'] = ['Measurement Number'] + context['header'][1:]
    context['results'] = viewTable(
        PATH+'/data/db.sqlite',
        "%d_test_%s" % (board_id, timestring)
    )
    return render(request, 'data.html', context)


def checkStatus(request):
    D = Diagnostics()
    context = {
        'ftdi_connected': D.checkFTDIOnline(),
        'i2c_fanout_connected': D.testI2CFanout(),
        'adc_connected': D.testADC(),
    }
    return JsonResponse(context)
