from django.contrib import admin
from django.contrib.auth.models import User
import datetime
from django.utils import timezone
from django.utils.translation import ugettext_lazy as _
from django.http import HttpResponseRedirect
from django.contrib.contenttypes.models import ContentType


from RouteFinder.models import Request, Driver, RouteSegment, Location, Time
# Register your models here.

"""class MyFormSet(BaseInlineFormSet):
    def get_queryset(self):
        if not hasattr(self, '_queryset'):
            qs = super(MyFormSet, self).get_queryset().filter(user=user)
            self._queryset = qs
        return self._queryset"""

class IdealTimeListFilter(admin.SimpleListFilter):
    title = _('Schedule period')
    parameter_name = 'period'

    def lookups(self, request, model_admin):
        return (
            ('first', _('the next scheduling period')),
            ('second', _('the second scheduling period'))
        )
    def queryset(self, request, queryset):
        if self.value() == 'first':
            if timezone.now().hour >= 12:
                return queryset.filter(ideal_time__gte=timezone.now().replace(hour=14, minute=0, second=0),
                                       ideal_time__lte=(timezone.now().replace(hour=0, minute=0, second=0) + datetime.timedelta(days=1))
                                       )
            else:
                return queryset.filter(ideal_time__gte=timezone.now().replace(hour=2, minute=0, second=0),
                                       ideal_time__lte=timezone.now().replace(hour=12, minute=0, second=0)
                                       )
        if self.value() == 'second':
            if timezone.now().hour < 12:
                return queryset.filter(ideal_time__gte=timezone.now().replace(hour=14, minute=0, second=0),
                                       ideal_time__lte=(timezone.now().replace(hour=0, minute=0, second=0) + datetime.timedelta(days=1))
                                       )
            else:
                return queryset.filter(ideal_time__gte=(timezone.now().replace(hour=2, minute=0, second=0) + datetime.timedelta(days=1)),
                                       ideal_time__lte=(timezone.now().replace(hour=12, minute=0, second=0) + datetime.timedelta(days=1))
                                       )



class RequestAdmin(admin.ModelAdmin):
    list_display = ('name', 'user', 'inOut', 'ideal_time', 'urgent', 'essential', 'comments', 'home_location', 'work_location', 'routeSegment',)
    list_filter = [IdealTimeListFilter,]
    exclude = ('home_loc', 'work_loc', 'earliest', 'latest')

    actions = ['schedule_vehicles', 'examine_routes']
    
    def schedule_vehicles(modeladmin, request, queryset):
        selected = request.POST.getlist(admin.ACTION_CHECKBOX_NAME)
        #ct = ContentType.objects.get_for_model(queryset.model)
        return HttpResponseRedirect("/req/PrepareSolution/?ids=%s" % (",".join(selected)))

    def examine_routes(modeladmin, request, queryset):
        selected = request.POST.getlist(admin.ACTION_CHECKBOX_NAME)
        return HttpResponseRedirect("/req/ExamineSolution/?ids=%s" % (",".join(selected)))

class LocationAdmin(admin.ModelAdmin):
    list_display = ('latitude', 'longitude')
    

class RouteSegmentAdmin(admin.ModelAdmin):
    list_display = ('id', 'driver',)
    list_filter = ['driver']  
    def has_add_permission(self, request):
        return False
    #def has_delete_permission(self, request):
     #   return False

class TimeAdmin(admin.ModelAdmin):
    list_display = ('id', 'GoogleEstimate',)
    list_filter = ['GoogleEstimate',]
    def has_add_permission(self, request):
        return False

class LocationAdmin (admin.ModelAdmin):
    def has_add_permission(self, request):
        return False
class DriverAdmin(admin.ModelAdmin):
    exclude = ('source_loc', 'sink_loc')
    
admin.site.register(Request, RequestAdmin)
admin.site.register(Driver, DriverAdmin)
admin.site.register(RouteSegment, RouteSegmentAdmin)
admin.site.register(Location, LocationAdmin)
admin.site.register(Time, TimeAdmin)
