from django.urls import path

from . import views

urlpatterns = [
    path('', views.index, name='index'),
    path('boards/<int:board_id>', views.board, name='board'),
    path('boards/<int:board_id>/data/<timestring>', views.data, name='data'),
    path('check_status', views.checkStatus, name='status')
]
