from django.shortcuts import render
from django.http import HttpResponseRedirect, HttpResponse
from django.core.urlresolvers import reverse
from django import forms
from django.views import generic
from django.utils import timezone
from django.core.urlresolvers import reverse_lazy
from django.contrib.auth import authenticate, login
from django.contrib.auth.decorators import login_required
from django.utils.decorators import method_decorator
from decimal import Decimal, ROUND_HALF_UP
from django.core.management import call_command
from django.contrib.auth.models import User
import datetime


from RouteFinder.models import Request, Location, Parameters, Time, Driver, RouteSegment
from RouteFinder.forms import RequestForm
from RouteFinder.tasks import find_route

#Protected view classes that require users to be logged in

class ProtectedListView(generic.ListView):
    @method_decorator(login_required)
    def dispatch(self, *args, **kwargs):
        return super(ProtectedListView, self).dispatch(*args, **kwargs)

class ProtectedCreateView(generic.edit.CreateView):
    @method_decorator(login_required)
    def dispatch(self, *args, **kwargs):
        return super(ProtectedCreateView, self).dispatch(*args, **kwargs)

class ProtectedUpdateView(generic.edit.UpdateView):
    @method_decorator(login_required)
    def dispatch(self, *args, **kwargs):
        return super(ProtectedUpdateView, self).dispatch(*args, **kwargs)

class ProtectedDeleteView(generic.edit.DeleteView):
    @method_decorator(login_required)
    def dispatch(self, *args, **kwargs):
        return super(ProtectedDeleteView, self).dispatch(*args, **kwargs)

# Views for general user request submitions

class RequestList(ProtectedListView):
    """Front page after user login. Displays their requests."""

    template_name = 'requests/list.html'
    context_object_name = 'users_request_list'

    def get_queryset(self):
        """Return future requests from user"""
        return Request.objects.filter(ideal_time__gte=timezone.now(), user = self.request.user.id)


class RequestCreate(ProtectedCreateView):
    """Adds new request from user, via Model Form"""

    model = Request
    fields = ['name', 'inOut', 'home_location', 'work_location', 'ideal_time', 'essential', 'urgent', 'comments']
    template_name = 'RouteFinder/request_add.html'
    success_url = '/req/'
    form_class = RequestForm

    def form_valid(self, form):
        form.instance.user_id = self.request.user.id
        return super(RequestCreate, self).form_valid(form)


class RequestUpdate(ProtectedUpdateView):
    """Goes to filled in form of selected request."""

    model = Request
    fields = ['name', 'inOut', 'home_location', 'work_location', 'ideal_time', 'essential', 'urgent', 'comments']
    template_name = 'RouteFinder/request_add.html'
    success_url = '/req/'
    form_class = RequestForm


class RequestDelete(ProtectedDeleteView):
    """Deletes selected request."""

    model = Request
    success_url = '/req/'


#Views for administrator to start optimisation engine and examine solution

class PrepareSolutionView(ProtectedListView):
    
    template_name = 'RouteFinder/PrepareSolution.html'

    def get(self, request):
        staff_requests = request.GET.getlist('ids',[])[0].split(",")
        locationsUsed = []
        #Enter request locations into database
        earliest_ideal_time = None#datetime.datetime(year=1, month=1, day=1)
        latest_ideal_time = None#datetime.datetime(year=9999, month=1, day=1)
        for location in staff_requests:
            staff_request = Request.objects.get(pk=location)

            try:
                route_segment = RouteSegment.objects.get(pk=staff_request.routeSegment)
                route_segment.delete()
            except:
                pass
            staff_request.routeSegment = None
            
            if latest_ideal_time is None or staff_request.ideal_time > latest_ideal_time:
                latest_ideal_time = staff_request.ideal_time
            if earliest_ideal_time is None or staff_request.ideal_time < earliest_ideal_time:
                earliest_ideal_time = staff_request.ideal_time

            
            
            lat = Decimal(staff_request.home_location.latitude).quantize(Decimal('0.000001'), rounding=ROUND_HALF_UP)
            lon = Decimal(staff_request.home_location.longitude).quantize(Decimal('0.000001'), rounding=ROUND_HALF_UP)
            try:
                temp = Location.objects.get(latitude = lat, longitude = lon)
            except Location.DoesNotExist:
                temp = Location(latitude = lat, longitude = lon)
                temp.save()
            else:
                pass
            locationsUsed.append(temp)
            staff_request.home_loc = temp

            lat = Decimal(staff_request.work_location.latitude).quantize(Decimal('0.000001'), rounding=ROUND_HALF_UP)
            lon = Decimal(staff_request.work_location.longitude).quantize(Decimal('0.000001'), rounding=ROUND_HALF_UP)
            try:
                temp = Location.objects.get(latitude = lat, longitude = lon)
            except Location.DoesNotExist:
                temp = Location(latitude = lat, longitude = lon)
                temp.save()
            else:
                pass
            locationsUsed.append(temp)
            staff_request.work_loc = temp
            staff_request.save()
                    
        #Add drivers
        
        #First need to determine which drivers are available
        aStaffMember = Request.objects.get(pk=staff_requests[0])
        ATime = aStaffMember.ideal_time
        if ATime.hour > 12:
            earl = 780 #13 * 60
            late = 1500 #25 * 60
            if timezone.now().hour >= 12:
                session = "first"
            else:
                session = "second"
        else:
            earl = 60 #1 * 60
            late = 780 #13 * 60
            if timezone.now().hour > 12:
                session = "second"
            else:
                session = "first"

        earl_ideal_time = earliest_ideal_time.hour * 60 + earliest_ideal_time.minute
        late_ideal_time = latest_ideal_time.hour * 60 + latest_ideal_time.minute
        if earl_ideal_time < earl or late_ideal_time > late:
            return HttpResponse("<p>Time frame of requests doesn't fit into a single schedulling session. Please use the filter provided to select a scheduling period before starting scheduling.</p><p><a href='/admin/RouteFinder/request/'>Return to admin</a></p>")
        
        if session == "first":
            drivers = Driver.objects.filter(available_for_next_session = True)
        else:
            drivers = Driver.objects.filter(available_for_second_next_session = True)

        #Now add their associated locations
        for driver in drivers:
            lat = Decimal(driver.sink.latitude).quantize(Decimal('0.000001'), rounding=ROUND_HALF_UP)
            lon = Decimal(driver.sink.longitude).quantize(Decimal('0.000001'), rounding=ROUND_HALF_UP)
            try:
                temp = Location.objects.get(latitude = lat, longitude = lon)
            except Location.DoesNotExist:
                temp = Location(latitude = lat, longitude = lon)
                temp.save()
            else:
                pass
            locationsUsed.append(temp)
            driver.sink_loc = temp

            lat = Decimal(driver.source.latitude).quantize(Decimal('0.000001'), rounding=ROUND_HALF_UP)
            lon = Decimal(driver.source.longitude).quantize(Decimal('0.000001'), rounding=ROUND_HALF_UP)
            try:
                temp = Location.objects.get(latitude = lat, longitude = lon)
            except Location.DoesNotExist:
                temp = Location(latitude = lat, longitude = lon)
                temp.save()
            else:
                pass
            locationsUsed.append(temp)
            driver.source_loc = temp
            driver.save()

        #Estimate times between locations
        time_dict = {}
        try:
            parameters = Parameters.objects.order_by('-id')[0]
        except:
            parameters = Parameters(LatLongEstExp=0.583769, LatLongEstMult = 81.97300, PoorWeather = Decimal(1.0))
            parameters.save()
        for loc1 in locationsUsed:
            for loc2 in locationsUsed:
                if loc1.pk < loc2.pk:
                    try:
                        tim = Time.objects.get(origin = loc1, destination = loc2)
                        time_dict[tim.pk]=1
                    except Time.DoesNotExist:
                        tim = Time(origin = loc1, destination = loc2, LatLongEstimate = (Decimal(parameters.LatLongEstMult) * (((loc1.latitude - loc2.latitude) ** Decimal(2) + Decimal(0.387748) * (loc1.longitude - loc2.longitude) ** Decimal(2)) ** (Decimal(0.5) * Decimal(parameters.LatLongEstExp))).quantize(Decimal('0.000001'), rounding=ROUND_HALF_UP)))#Local cartesian projection with correction for latitude of Cardiff
                        tim.save()
                        time_dict[tim.pk]=1
                    except MultipleObjectsReturned:
                        pass
                elif loc1.pk == loc2.pk:
                    try:
                        tim = Time.objects.get(origin = loc1, destination = loc2)
                    except Time.DoesNotExist:
                        tim = Time(origin = loc1, destination = loc2, LatLongEstimate = 0, GoogleEstimate = 0)
                        tim.save()
                        #time_list.append(tim.pk) 
                    except MultipleObjectsReturned:
                        pass
                   

        #Start optimisation engine
        driver_list = [driver.pk for driver in drivers]

        find_route(staff_requests, driver_list, time_dict.keys(), earl, late)

        #call_command('process_tasks')
        
        context = {'object_list': Request.objects.filter(pk__in=staff_requests)}
        return render(request, 'RouteFinder/PrepareSolution.html', context)   

class ExamineSolutionView(ProtectedListView):
    def get(self, request):
        staff_request_ids = request.GET.getlist('ids',[])[0].split(",")
        staff_requests = Request.objects.filter(pk__in=staff_request_ids)

        route_segment_ids = {}
        for req_no in staff_request_ids:
            route_segment_id = Request.objects.get(pk=req_no).routeSegment
            if route_segment_id != None:
                route_segment_ids[route_segment_id] = 1

        route_segments = RouteSegment.objects.filter(pk__in=route_segment_ids.keys())

        stops = {}
        for route_segment_no in route_segment_ids:
            route_segment = RouteSegment.objects.get(pk=route_segment_no)
            stops[route_segment.id] = {}
            for counter in range(1, 21):
                stop_value = route_segment.get_field(counter)
                if stop_value is not None:
                    stop_list = stop_value.split(",")
                    if int(stop_list[0]) == -1:
                        if int(stop_list[1]) == 1:
                            location = route_segment.driver.source
                            name = "Driver start"
                        else:
                            location = route_segment.driver.sink
                            name = "Driver end"
                    else:
                        staff_member = Request.objects.get(pk=int(stop_list[0]))
                        if int(stop_list[1]) == 1:
                            name = staff_member.name + " collection"
                            if staff_member.inOut == 'In':
                                location = staff_member.home_location
                            else:
                                location = staff_member.work_location
                        else:
                            name = staff_member.name + " drop-off"
                            if staff_member.inOut == 'Out':
                                location = staff_member.home_location
                            else:
                                location = staff_member.work_location
                    minutes = stop_list[3]
                    if len(minutes) == 1:
                        minutes = "0" + minutes
                    time_string = stop_list[2] + ":" + minutes
                    stops[route_segment.id][counter] = {'name': name, 'location': location, 'time': time_string}

        driver_ids = {}
        for route_segment_no in route_segment_ids.keys():
            driver_id = RouteSegment.objects.get(pk=route_segment_no).driver.id
            driver_ids[driver_id] = 1

        drivers = Driver.objects.filter(pk__in=driver_ids.keys())

        context = {'staff_requests': staff_requests, 'route_segments': route_segments, 'drivers': drivers, 'stops': stops}
    
        return render(request, 'RouteFinder/ExamineSolution.html', context)
            
#View to populate database with test data
def PopulateTest(request):
    import csv
    from CustomGeoposition import Geoposition
    temp = open('RouteFinder/SatAmReqs.csv', 'rb')
    staff_members = csv.reader(temp)
    staff_members = [[row[0], Decimal(row[1]), Decimal(row[2]), row[3], Decimal(row[4]), Decimal(row[5]), int(row[6]), int(row[7]), int(row[8]), int(row[9])] for row in staff_members]
    temp.close()

    user = User.objects.get(pk=request.user.id)
    now = timezone.now()
    if now.hour >=12:
        base = datetime.datetime(now.year, now.month, now.day + 1)
    else:
        base = datetime.datetime(now.year, now.month, now.day, hour=12)

    counter=0
    for staff in staff_members:
        counter += 1
        if staff[9] == 1:
            urg = False
            ess = False
        elif staff[9] == 2:
            urg = True
            ess = False
        elif staff[9] == 4:
            urg = False
            ess = True
        elif staff[9] == 8:
            urg = True
            ess = True
            
        if staff[6] == -1:
            ideal = base + datetime.timedelta(minutes=staff[8])
            Request(user=user, name=str(counter), inOut='In', home_location=Geoposition(latitude=staff[1], longitude=staff[2], address=staff[0]), work_location=Geoposition(latitude=staff[4], longitude=staff[5], address=staff[3]), essential=ess, urgent=urg, phone_number="0000000000", ideal_time=ideal).save()
        elif staff[8] == 0:
            ideal = base + datetime.timedelta(minutes=staff[6])
            Request(user=user, name=str(counter), inOut='Out', home_location=Geoposition(latitude=staff[4], longitude=staff[5], address=staff[3]), work_location=Geoposition(latitude=staff[1], longitude=staff[2], address=staff[0]), essential=ess, urgent=urg, phone_number="0000000000", ideal_time=ideal).save()
            
    return HttpResponse("Request database populated with test data.")
