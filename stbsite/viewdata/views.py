from django.shortcuts import render, redirect
from django.http import JsonResponse

from datetime import datetime

from .main import getBoards, createBoard
from .databasing import viewTable
from .values import PATH
from .diagnostics import Diagnostics
from .analysistools import custom_strftime, displayDataTable, getBoard
from .analysistools import getFooterStats


def index(request):
    context = displayDataTable(getBoards())
    return render(request, 'viewdata/index.html', context)


def board(request, board_id):
    context = {}
    context['all'] = displayDataTable(
        getBoard(board_id).listMeasurements('all'))
    context['services'] = displayDataTable(
        getBoard(board_id).listMeasurements('services'))
    context['x0'] = displayDataTable(
        getBoard(board_id).listMeasurements('x0'))
    context['x1'] = displayDataTable(
        getBoard(board_id).listMeasurements('x1'))
    context['board_id'] = board_id
    return render(request, 'viewdata/board.html', context)


def data(request, board_id, timestring, type):
    context = {}
    date_fmt = datetime.strptime(timestring, '%Y%m%d%H%M%S')
    context['date_fmt'] = custom_strftime(
        '%A {S} %B %Y at %-I:%M:%S %p',
        date_fmt)
    context['board_id'] = board_id
    context['timestring'] = timestring
    data = viewTable(
        PATH+'/data/db.sqlite',
        "%d_%s_%s" % (board_id, type, timestring)
    )
    context['header'] = ['Measurement Number'] + data.columns.tolist()[1:]
    context['results'] = data.values.tolist()
    context['table_width'] = len(list(data.columns))
    context['footer'] = getFooterStats(data)
    return render(request, 'viewdata/data.html', context)


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
    context = displayDataTable(getBoards())
    context['boards'] = getBoards().values
    print(context)
    return render(request, 'viewdata/measure.html', context)


def newBoard(request):
    context = {
        'timestring': datetime.strftime(datetime.now(), '%Y%m%d')
    }
    return render(request, 'viewdata/new-board.html', context)


def submitNewBoard(request):
    metadata = [
        request.GET.get('id'),
        request.GET.get('version'),
        request.GET.get('timestring')]
    createBoard(metadata)
    return redirect('/viewdata/measure')


def deleteBoard(request):
    print(request.GET.get('board_id'))
    board = getBoard(int(request.GET.get('board_id')))
    board.deleteBoard()
    # metadata = [request.GET.get('version'), request.GET.get('timestring')]
    # createBoard(metadata)
    return JsonResponse({})


def getMostRecentMeasurement(request):
    board = getBoard(int(request.GET.get('boards')))
    board.measure(
        request.GET.get('type'),
        request.GET.get('measurement-slider'))
    context = displayDataTable(board.listMeasurements(request.GET.get('type')))
    recent = board.listMeasurements(request.GET.get('type')).timestamp
    context['header'] = ['Measurement Number'] + context['header'][1:]
    data = viewTable(
        PATH+'/data/db.sqlite',
        "%s_%s_%s" % (
            request.GET.get('boards'),
            request.GET.get('type'),
            recent.max())
    )
    context['header'] = ['Measurement Number'] + data.columns.tolist()[1:]
    context['results'] = data.values.tolist()
    context['table_width'] = len(list(data.columns))
    context['footer'] = getFooterStats(data)
    return JsonResponse(context)
