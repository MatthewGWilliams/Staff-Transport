from django.conf.urls import patterns, url

from RouteFinder import views

urlpatterns = patterns('',
    #User request submission views
    url(r'^$', views.RequestList.as_view(), name='request_list'),
    url(r'add/$', views.RequestCreate.as_view(), name='request_add'),
    url(r'(?P<pk>\d+)/$', views.RequestUpdate.as_view(), name='request_update'),
    url(r'(?P<pk>\d+)/delete/$', views.RequestDelete.as_view(), name='request_delete'),

    #Planning views
    url(r'PrepareSolution/$', views.PrepareSolutionView.as_view(), name='preparation'),
    url(r'ExamineSolution/$', views.ExamineSolutionView.as_view(), name='solution'),

    #Test data
    url(r'Populatewithtest/$', views.PopulateTest, name='populate')
)

