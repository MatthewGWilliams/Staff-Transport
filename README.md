Staff-Transport
===============

A web application for routing of vehicles to pick up hospital staff in extreme weather conditions.

C++ implementation of heuristic methods to solve a variant of the pickup and delivery problem with time windows, with a web based user interface for submitting transport requests and finding a set of routes.

Requirements:
=============

* Python 2.7
* Django 1.6
* django-datetime-widget 0.9 (from pypi)
* django-bootstrap3
* django-background-task
* GCC including g++ with C++11 support

Installation
============

emergencyTransport is the main project. This can be copied if you have the secret key from me, or it can be used as a template for setting up a new project. An
API key can be obtained from Google, it is used for geopositioning in the form widget.
The CustomGeoposition and templates folders just need to be copied to the corresponding position for you project. The RouteFinder app should also be copied into the corresponding location, but the command
"python setup.py build_ext --inplace" must then be called from the apps directory. This compiles the C++ route finder into a python module that is used by the web interface to plan vehicle routes.

From the project directory containing manage.py, run "python manage.py syncdb" to create the database used by the website.

It is important that the background-task module turned on with the web site, or the route planner will not be called.

Acknowledgements
================

CustomGeoposition has been adapted from django-geoposition, github.com/philippbosch/django-geoposition.
The RouteFinder app uses a function from PatMap, github.com/JasYoung314/PatMap, to submit a request to, and parse the response from, Google Maps Webservices Distance Matrix API.
The RouteFinder app also uses the Google Maps Javascript API v3 to display suggested routes.
