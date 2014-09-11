# -*- coding: utf-8 -*-
from django.conf import settings
from appconf import AppConf


class GeopositionConf(AppConf):
    MAP_WIDGET_HEIGHT = 480
    MAP_OPTIONS = {}
    MARKER_OPTIONS = {}
    API_KEY = 'AIzaSyBSku0M4uAMZY76pEdR9EOUOj5KtWmjgSs'
    
    class Meta:
        prefix = 'customgeoposition'
