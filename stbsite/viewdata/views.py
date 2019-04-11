from django.shortcuts import render
from django.http import HttpResponse
from .main import getBoards

# Create your views here.


def index(request):
    df = getBoards()
    context = {
        'header': [('Column %d' % i) for i in range(4)],
        'results': [i for i in range(16)],
    }
    return render(request, 'index.html', context)
