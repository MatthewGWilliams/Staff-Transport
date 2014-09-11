#Django modules
from django.db import models
import datetime
from django.utils import timezone
from django.contrib.auth.models import User

#My modules
from CustomGeoposition.fields import GeopositionField

# Create your models here.

class Location(models.Model):
    latitude = models.DecimalField(max_digits=10, decimal_places=6)
    longitude = models.DecimalField(max_digits=10, decimal_places=6)
    
    def __unicode__(self):
        return unicode(self.latitude) + "," + unicode(self.longitude)
    def get_absolute_url(self):
        return reverse('locations-detail', kwargs={'pk': self.pk})
    

class Driver(models.Model):
    driver_name = models.CharField(max_length=50)
    source = GeopositionField()
    source_loc = models.ForeignKey(Location, related_name='sou', blank=True, null=True)
    sink = GeopositionField()
    sink_loc = models.ForeignKey(Location, related_name='sin', blank=True, null=True)
    capacity_Choices = (
        (1, 1),
        (2, 2),
        (3, 3),
        (4, 4),
        (5, 5),
        (6, 6),
        (7, 7),
        (8, 8),
        (9, 9),
        (10, 10),
    )
    capacity = models.IntegerField(choices=capacity_Choices)
    available_for_next_session = models.BooleanField(default=True)
    available_for_second_next_session = models.BooleanField(default=True)

    def __unicode__(self):
        return self.driver_name
    def get_absolute_url(self):
        return reverse('Driver-detail', kwargs={'pk': self.pk})
    

class RouteSegment(models.Model):
    driver = models.ForeignKey(Driver)

    pos_1 = models.CommaSeparatedIntegerField(max_length=200, blank=True, null=True)
    pos_2 = models.CommaSeparatedIntegerField(max_length=200, blank=True, null=True)
    pos_3 = models.CommaSeparatedIntegerField(max_length=200, blank=True, null=True)
    pos_4 = models.CommaSeparatedIntegerField(max_length=200, blank=True, null=True)
    pos_5 = models.CommaSeparatedIntegerField(max_length=200, blank=True, null=True)
    pos_6 = models.CommaSeparatedIntegerField(max_length=200, blank=True, null=True)
    pos_7 = models.CommaSeparatedIntegerField(max_length=200, blank=True, null=True)
    pos_8 = models.CommaSeparatedIntegerField(max_length=200, blank=True, null=True)
    pos_9 = models.CommaSeparatedIntegerField(max_length=200, blank=True, null=True)
    pos_10 = models.CommaSeparatedIntegerField(max_length=200, blank=True, null=True)
    pos_11 = models.CommaSeparatedIntegerField(max_length=200, blank=True, null=True)
    pos_12 = models.CommaSeparatedIntegerField(max_length=200, blank=True, null=True)
    pos_13 = models.CommaSeparatedIntegerField(max_length=200, blank=True, null=True)
    pos_14 = models.CommaSeparatedIntegerField(max_length=200, blank=True, null=True)
    pos_15 = models.CommaSeparatedIntegerField(max_length=200, blank=True, null=True)
    pos_16 = models.CommaSeparatedIntegerField(max_length=200, blank=True, null=True)
    pos_17 = models.CommaSeparatedIntegerField(max_length=200, blank=True, null=True)
    pos_18 = models.CommaSeparatedIntegerField(max_length=200, blank=True, null=True)
    pos_19 = models.CommaSeparatedIntegerField(max_length=200, blank=True, null=True)
    pos_20 = models.CommaSeparatedIntegerField(max_length=200, blank=True, null=True)

    nextSegment = models.ForeignKey("self", related_name='next', blank=True, null=True)
    lastSegment = models.ForeignKey("self", related_name='last', blank=True, null=True)
    
    def __unicode__(self):
        return unicode(self.id)
    def get_absolute_url(self):
        return reverse('routeSegment-detail', kwargs={'pk': self.pk})
    def set_fields(self, num, value):
        attname = 'pos_{0}'.format(num)
        setattr(self, attname, value)
    def get_field(self, num):
        attname = 'pos_{0}'.format(num)
        return getattr(self, attname)
    #def attrs(self):
    #    for field in self._meta.fields:
    #        yield field.name, getattr(self, field.name)
    

class Request(models.Model):
    user = models.ForeignKey(User,
                             default=User)    
    name = models.CharField(max_length=50)
    IN = 'In'
    OUT = 'Out'
    inOut_Choices = (
        (IN, 'Coming to work'),
        (OUT, 'Going home'),
    )
    inOut = models.CharField('Purpose of Journey',
                             max_length=3,
                             choices=inOut_Choices,
                             help_text="If coming to work set ideal time as shift start. If going home set ideal time as shift end.")
    home_location = GeopositionField()
    home_loc = models.ForeignKey(Location, related_name='hom', null=True, blank=True)
    work_location = GeopositionField()
    work_loc = models.ForeignKey(Location, related_name='wor', null=True, blank=True)
    ideal_time = models.DateTimeField()
    other_time = models.DateTimeField(help_text="Other end of time window.", blank=True, null=True)
    essential = models.BooleanField(default=False)
    urgent = models.BooleanField(default=False)
    comments = models.TextField(blank=True, help_text="Special requirements including extra time limits may be submitted here if necessary.")
    phone_number = models.CharField(max_length=15, blank=True)
    routeSegment = models.IntegerField(blank=True, null=True)
    earliest = models.IntegerField(blank=True, null=True )
    latest = models.IntegerField(blank=True, null=True)
    
    def __unicode__(self):
        return self.name
    def get_absolute_url(self):
        return reverse('req-detail', kwargs={'pk': self.pk})
    

class Time(models.Model):
    origin = models.ForeignKey(Location, related_name='first_loc')
    destination = models.ForeignKey(Location, related_name='second_loc')
    LatLongEstimate = models.DecimalField(max_digits=10, decimal_places=6, blank=True)
    GoogleEstimate = models.DecimalField(max_digits=10, decimal_places=6, blank=True, null=True)
    routeSegment = models.ForeignKey(RouteSegment, blank=True, null=True)

    def __unicode__(self):
        return unicode(self.origin) + "," + unicode(self.destination)
    def get_absolute_url(self):
        return reverse('times-detail', kwargs={'pk': self.pk})

class Parameters(models.Model):
    LatLongEstExp = models.DecimalField(max_digits=10, decimal_places = 9)
    LatLongEstMult = models.DecimalField(max_digits=10, decimal_places = 5)
    PoorWeather = models.DecimalField(max_digits=10, decimal_places = 5)
