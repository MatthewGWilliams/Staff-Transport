#!/usr/bin/env python
# -*- coding: utf-8 -*- 

import json
import urllib
import time
import datetime

#From PatMap by Jason Young, available on GitHub at github.com/JasYoung314/PatMap
#Function to get distance between 2 points from google maps. By default route is by car, distance is given in miles and time in minutes.
def CalculateDistance(Origin = False,Destination = False, Method = "driving",TimeUnits = "Minutes",DistUnits = "Miles"):
	
	#this is the start of a distnace matrix url
	base = "https://maps.googleapis.com/maps/api/distancematrix/json?"
	
	#Converts the variables to the required format
	urlorigin = "origins=%s&".encode('utf-8') %(Origin)
	urldestination = "destinations=%s&".encode('utf-8') %(Destination)
	urlmethod = "mode=%s&" %(Method)
	if DistUnits == "Kilometers" or DistUnits == "Meters":
		urlunits = "units=metric&"
	else:
		urlunits = "units=imperial&"
	#constructs the completed url
	url = base.decode('utf-8') + urlorigin.decode('utf-8') + urldestination.decode('utf-8') + urlmethod.decode('utf-8') + urlunits.decode('utf-8') + "language=en-EN&sensor=false".decode('utf-8')
	
	#Interprets the json data recieved
	try:
		result= json.load(urllib.urlopen(url))
	except:
		return 'ERROR','ERROR'
	
	#Reads the status code and takes the appropriate action
	if result["status"] == "OK":
		if result["rows"][0]["elements"][0]["status"] == "OK":
			time =  result["rows"][0]["elements"][0]["duration"]["value"]
			distance = result["rows"][0]["elements"][0]["distance"]["value"]
			
			if TimeUnits == "Minutes":
				time = time/60.0
			elif TimeUnits == "Hours":
				time = time/3600.0
				
			if DistUnits == "Kilometres":
				distance = distance/1000.0
			elif DistUnits == "Yards":
				distance = distance*1.0936133
			elif DistUnits == "Miles":
				distance = distance*0.000621371192
					
			return time,distance
		else:
			return result["rows"][0]["elements"][0]["status"],result["rows"][0]["elements"][0]["status"]
	else:
		return result["status"]
