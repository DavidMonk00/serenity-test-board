from django.urls import path
from django.conf.urls.static import static
from django.conf import settings

from . import views

urlpatterns = [
    path('', views.index, name='index'),
    path('measure', views.measure, name='measure'),
    path('new-board', views.newBoard, name='new-board'),
    path('boards/<int:board_id>', views.board, name='board'),
    path('boards/<int:board_id>/data/<timestring>', views.data, name='data'),
    path('check_ftdi_status', views.checkFTDIStatus, name='ftdi-status'),
    path('check_i2c_status', views.checkI2CStatus, name='i2c-status'),
    path('submit-new-board', views.submitNewBoard, name='submit-new-board'),
] + static(settings.STATIC_URL, document_root=settings.STATIC_ROOT)
