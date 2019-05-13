from django.shortcuts import render, redirect
from django.http import JsonResponse, HttpResponse
from django.core.mail import send_mail


def index(request):
    context = {}
    return render(request, 'request/index.html', context)


def requestFilm(request):
    metadata = [request.GET.get('film-name'),
                request.GET.get('film-year')]
    send_mail(
        'Film Request',
        'Here is the message.',
        'cuts.fpr@gmail.com',
        ['dm2614@ic.ac.uk'],
        fail_silently=False,
    )
    print(metadata)
    return redirect('/request')
