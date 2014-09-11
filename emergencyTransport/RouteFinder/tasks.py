from background_task import background
from django.contrib.auth.models import User
import time
from django.utils import timezone
import datetime
import decimal

import RouteFinder.PyProb
from GoogleDistances import CalculateDistance
from RouteFinder.models import Request, Time, Driver, RouteSegment, Location, Parameters

#To stay within Google API usage terms, can only store data for 24 hours.
@background(schedule=86400)
def clear_google_data(time_no):
    time_record = Time.objects.get(pk=time_no)
    time_record.GoogleEstimate = None
    time_record.save()

@background(schedule=0)
def find_route(reqs, drivers, times, earl, late):
    solved = False
    tightness = 5 
    while solved == False and tightness < 10:
        #Create instance of PyProblem
        optimiser = RouteFinder.PyProb.PyProblem(tightness)

        try:
            parameters = Parameters.objects.order_by('-id')[0]
        except:
            parameters = Parameters(LatLongEstExp=0.583769, LatLongEstMult = 81.97300, PoorWeather = decimal.Decimal(1))
            parameters.save()
        
        #Add times
        for time_no in times:
            tim = Time.objects.get(pk=time_no)

            if tim.GoogleEstimate is not None:
                optimiser.insert_time(tim.origin.pk, tim.destination.pk, tim.GoogleEstimate * parameters.PoorWeather)
            else:
                optimiser.insert_time(tim.origin.pk, tim.destination.pk, tim.LatLongEstimate * parameters.PoorWeather)
        
        #Add staff    
        for req_no in reqs:
            req = Request.objects.get(pk=req_no)

            if req.inOut == 'In':
                inOut = True
                ideal = req.ideal_time.hour * 60 + req.ideal_time.minute

                if req.other_time is not None:
                    earliest = req.other_time.hour * 60 + req.other_time.minute
                else:
                    earliest = -1

                latest = 150 + ideal
                pickup = [[req.home_loc.pk,0]]
                dropoff = [[req.work_loc.pk,0]]

            elif req.inOut == 'Out':
                inOut = False
                ideal = -1
                earliest = req.ideal_time.hour * 60 + req.ideal_time.minute

                if req.other_time is not None:
                    latest = req.other_time. hour * 60 + req.other_time.minute
                else:
                    latest = -1

                pickup = [[req.work_loc.pk,0]]
                dropoff = [[req.home_loc.pk,0]]
            
            if req.essential == True or req.inOut == 'Out':
                priority = 4
            else:
                priority = 1
            if req.urgent == True:
                priority = priority * 2

            req.earliest = earliest
            req.latest = latest
            req.save()
            optimiser.insert_Request(int(req_no), inOut, earliest, latest, priority, ideal, pickup, dropoff)
            #optimiser.insert_Request(1, True, -1, 510, 2, 360, [[1,0]], [[2,0]])
            #optimiser.insert_Request(1, True, -1, 580, 2, 420, [[1,0]], [[2,0]])

        #Add drivers
        for driver_no in drivers:
            driver = Driver.objects.get(pk=driver_no)
            optimiser.insert_Vehicle(driver_no, driver.sink_loc.pk, driver.source_loc.pk, driver.capacity, earl, late)
            #optimiser.insert_Vehicle(1, 1, 1, 4, 100, 700)
            
        #Find solution
        solution = []#[[[1.0], [1.0, 1.0, 393.7, 1.0, 1.0], [1.0, 0.0, 420.0, 2.0, 0.0]]]
        try:
            optimiser.setup_solution()
            solution = optimiser.returnSolution()
        except:
            pass
        if not solution or solution == []:
            solved = False
        else:
            solved = True

            time_change = False
            cont = True
            segments_created = {}
            while cont == True:
                if time_change == True:
                    try:
                        optimiser.updateSolutions()
                        solution = optimiser.returnSolution()
                        if not solution:
                            solved = False
                            break
                        else:
                            solved = True
                    except:
                        break
                    
                #segments_created = {}
                time_change = False
                for vehicle in solution:
                    last_seg = None
                    vehicle_route = Driver.objects.get(pk=int(vehicle[0][0])) #Associate solution vehicle with real driver in Django app.
                    seg = RouteSegment(driver = vehicle_route)
                    seg.save()
                    segments_created[seg.pk]=1
                    counter = 0
                    origin = ""
                    for stop in vehicle[1:]:
                        counter += 1
                        #Process all stops in vehicle's route
                        if int(stop[0]) != -1:
                            if last_seg is not None:
                                seg.save()
                                last_seg.nextSegment = seg
                                last_seg.save()
                            request = Request.objects.get(pk=int(stop[0]))
                            request.routeSegment = seg.pk #Assign request to current route segment
                            request.save()
                            seg.save()
                            segments_created[seg.pk]=1
                        elif int(stop[1] == 0 and counter == 1):
                            seg = last_seg #add driver sink to end of last route segment
                            counter = old_counter + 1

                        current_location = Location.objects.get(pk=int(stop[3]))
                        destination = str(current_location.latitude) + "," + str(current_location.longitude)

                        if(origin != ""):
                            if current_location.pk > previous_location.pk:
                                time_record = Time.objects.get(origin = previous_location, destination = current_location)
                            else:
                                time_record = Time.objects.get(origin = current_location, destination = previous_location)
                            old_travel_time = time_record.LatLongEstimate
                            
                            if time_record.GoogleEstimate is not None:
                                travel_time = time_record.GoogleEstimate
                                old_travel_time = travel_time
                            else:
                                distance = CalculateDistance(Origin=origin, Destination=destination)
                                time.sleep(0.2) #to stay within Google API limit
                                if len(distance) == 2 and type(distance[0]) is float: #check google returned travel time
                                    travel_time = decimal.Decimal(distance[0])
                                    time_record.GoogleEstimate = travel_time
                                    time_record.save()
                                    clear_google_data(time_record.pk) #schedule to forget after 24 hours
                                else:
                                    travel_time = time_record.LatLongEstimate

                            #Update travel time in optimiser if Google suggests it is different to LatLongEstimate
                            if abs(travel_time - old_travel_time) > 1:
                                optimiser.insert_time(time_record.origin.pk, time_record.destination.pk, travel_time * parameters.PoorWeather)
                                old_segments = RouteSegment.objects.filter(pk__in=segments_created.keys())
                                old_segments.delete()
                                segments_created = {}
                                time_change = True
                                for req_no in reqs:
                                    req = Request.objects.get(pk=req_no)
                                    req.routeSegment = None
                                break
                    
                        time_in_minutes = int(round(stop[2])) #stop[2] is time served in minutes from 0 point (choosen as midnight, start of day for whatever day ideal times are in).
                        hours = time_in_minutes / 60
                        minutes = time_in_minutes % 60
                        seg.save()
                        segments_created[seg.pk]=1
                        seg.set_fields(counter, str(int(stop[0])) + "," + str(int(stop[1])) + "," + str(hours) + "," + str(minutes)) #stop[0] is request number, stop[1] is binary integer indicating if picking up (instead of dropping off). 
                        seg.save()
                        
                        
                        origin = destination
                        previous_location = current_location
                        #Split into segments at points where vehicle is empty
                        if (stop[4] == 0 and (stop[0] != -1)) or counter == 20:
                            if last_seg is not None:
                                seg.lastSegment = last_seg
                            last_seg = seg
                            old_counter = counter
                            seg.save()
                            segments_created[seg.pk]=1
                            vehicle_route = Driver.objects.get(pk=int(vehicle[0][0]))
                            seg = RouteSegment(driver = vehicle_route)
                            counter = 0
                            

                    if time_change == True:
                        cont = True
                        break
                    else:
                        cont = False
        if solved == False:
            tightness += 0.25

    import winsound
    Freq = 1500 # Set Frequency To 2500 Hertz
    Dur = 100 # Set Duration To 1000 ms == 1 second
    winsound.Beep(Freq,Dur)
    
            

    
