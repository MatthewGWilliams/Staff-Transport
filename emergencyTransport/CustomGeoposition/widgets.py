from __future__ import unicode_literals

import json
from django import forms
from django.template.loader import render_to_string
from django.utils import six
from django.utils.translation import ugettext_lazy as _
from .conf import settings


class GeopositionWidget(forms.MultiWidget):
    def __init__(self, attrs=None):
        widgets = (
            forms.TextInput(),
            forms.TextInput(),
            forms.TextInput(attrs={'size':100}),
        )
        super(GeopositionWidget, self).__init__(widgets, attrs)

    def decompress(self, value):
        if isinstance(value, six.text_type):
            return value.rsplit(',')
        if value:
            return [value.latitude, value.longitude, value.address]
        return [None,None,None]

    def format_output(self, rendered_widgets):
        return render_to_string('geoposition/widgets/geoposition.html', {
            'latitude': {
                'html': rendered_widgets[0],
                'label': _("latitude"),
            },
            'longitude': {
                'html': rendered_widgets[1],
                'label': _("longitude"),
            },
            'address': {
                'html': rendered_widgets[2],
                'label': _("address"),
            },
            'config': {
                'map_widget_height': settings.CUSTOMGEOPOSITION_MAP_WIDGET_HEIGHT,
                'map_options': json.dumps(settings.CUSTOMGEOPOSITION_MAP_OPTIONS),
                'marker_options': json.dumps(settings.CUSTOMGEOPOSITION_MARKER_OPTIONS),
            }
        })

    class Media:
        js = (
            'https://maps.google.com/maps/api/js?v=3&key='+settings.CUSTOMGEOPOSITION_API_KEY+'&region=GB',
            'geoposition/geoposition.js',
        )
        css = {
            'all': ('geoposition/geoposition.css',)
        }
