from django.shortcuts import render, redirect
from django.http import JsonResponse, HttpResponse


def index(request):
    context = {}
    return render(request, 'request/index.html', context)
