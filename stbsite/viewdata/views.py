from django.shortcuts import render
from django.http import JsonResponse
from .main import getBoards, createBoard
from .board import Board
from .databasing import viewTable
from .values import PATH
from datetime import datetime
from .diagnostics import Diagnostics
from django.shortcuts import redirect

# Create your views here.


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


def checkFTDIStatus(request):
    D = Diagnostics()
    context = {
        'ftdi_connected': D.checkFTDIOnline(),
    }
    return JsonResponse(context)


def checkI2CStatus(request):
    D = Diagnostics()
    context = {
        'i2c_fanout_connected': D.testI2CFanout(),
        'adc_connected': D.testADC(),
    }
    return JsonResponse(context)


def measure(request):
    context = displayDataTable(getBoards)
    context['boards'] = getBoards().values
    return render(request, 'measure.html', context)


def newBoard(request):
    context = {
        'timestring': datetime.strftime(datetime.now(), '%Y%m%d')
    }
    return render(request, 'new-board.html', context)


def submitNewBoard(request):
    metadata = [request.GET.get('version'), request.GET.get('timestring')]
    createBoard(metadata)
    return redirect('/viewdata/measure')


def getMostRecentMeasurement(request):
    boards = getBoards()
    board = Board(boards.ID.values[int(request.GET.get('boards'))])
    board.measure()
    context = displayDataTable(board.listMeasurements)
    recent = board.listMeasurements().timestamp
    context['header'] = ['Measurement Number'] + context['header'][1:]
    context['results'] = [list(i) for i in viewTable(
        PATH+'/data/db.sqlite',
        "%s_test_%s" % (request.GET.get('boards'), recent.max())
    )]
    return JsonResponse(context)
