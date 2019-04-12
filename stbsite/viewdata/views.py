from django.shortcuts import render, redirect
from django.http import JsonResponse

from datetime import datetime
import numpy as np

from .main import getBoards, createBoard
from .databasing import viewTable
from .values import PATH
from .diagnostics import Diagnostics
from .analysistools import custom_strftime, displayDataTable, getBoard
from .analysistools import getFooterStats

# Create your views here.


def index(request):
    context = displayDataTable(getBoards)
    return render(request, 'index.html', context)


def board(request, board_id):
    context = displayDataTable(getBoard(board_id).listMeasurements)
    context['board_id'] = board_id
    return render(request, 'board.html', context)


def data(request, board_id, timestring):
    context = displayDataTable(getBoard(board_id).listMeasurements)
    date_fmt = datetime.strptime(timestring, '%Y%m%d%H%M%S')
    context['date_fmt'] = custom_strftime(
        '%A {S} %B %Y at %-I:%M:%S %p',
        date_fmt)
    context['board_id'] = board_id
    context['timestring'] = timestring
    context['header'] = ['Measurement Number'] + context['header'][1:]

    data = viewTable(
        PATH+'/data/db.sqlite',
        "%d_test_%s" % (board_id, timestring)
    )
    context['results'] = data
    context['footer'] = getFooterStats(data)
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
    board = getBoard(int(request.GET.get('boards')))
    board.measure()
    context = displayDataTable(board.listMeasurements)
    recent = board.listMeasurements().timestamp
    context['header'] = ['Measurement Number'] + context['header'][1:]

    data = [list(i) for i in viewTable(
        PATH+'/data/db.sqlite',
        "%s_test_%s" % (request.GET.get('boards'), recent.max())
    )]
    context['results'] = data
    context['footer'] = getFooterStats(data)
    return JsonResponse(context)
