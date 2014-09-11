from django import forms
from RouteFinder.models import Request
from datetimewidget.widgets import DateTimeWidget
from django.contrib.admin import widgets
from django.utils import timezone
import datetime

class RequestForm(forms.ModelForm):
    ideal_time = forms.DateTimeField(widget=DateTimeWidget(usel10n=True,
                                                           bootstrap_version=3,
                                                           options = {'pickerPosition': 'bottom-right',
                                                                      'clearBtn': 'true',
                                                                      'startView': '1',
                                                                      'startDate': timezone.now().date().isoformat() + ' ' + (timezone.now()  + datetime.timedelta(hours=5)).time().isoformat()[:8], #5 hours from current system time
                                                                      'endDate':  (timezone.now() + datetime.timedelta(days=2)).date().isoformat() + ' 00:00:00' #start of the day after tomorrow(today plus 2 days, time set to start of day)
                                                                      }))
    phone_number = forms.RegexField(regex=r'^\+?1?\d{3,6}\s*\d{3,6}\s*\d{0,4}$')
    class Meta:
        model = Request
        
        """widgets = {
            'ideal_time': DateTimeWidget(attrs={'id': 'id_ideal_time'}, usel10n=True, bootstrap_version=3, options = {'pickerPosition': 'bottom-right', 'clearBtn': 'true'})
        }"""
        fields = ['inOut', 'ideal_time', 'name', 'phone_number', 'home_location','work_location', 'essential', 'urgent', 'comments']

            
