from django.conf.urls import patterns, include, url
from django.views.generic.base import RedirectView

from django.contrib import admin
admin.autodiscover()

from RouteFinder.models import Request
from views import logout_page

urlpatterns = patterns('',
    # Examples:
    # url(r'^$', 'emergencyTransport.views.home', name='home'),
    # url(r'^blog/', include('blog.urls')),

    url(r'^admin/', include(admin.site.urls)),
    url(r'^login/$', 'django.contrib.auth.views.login'),
    url(r'^logout/$', logout_page),
                       
    url(r'^req/', include('RouteFinder.urls', namespace='RouteFinder')),
    url(r'^.*$', RedirectView.as_view(url='/req/', permanent=False)),
)
