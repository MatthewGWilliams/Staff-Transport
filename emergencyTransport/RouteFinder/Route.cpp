#include "Route.h"

#include <random>
#include <chrono>
#include <algorithm>
#include <iostream>

using std::next;
using std::vector;
using std::max;

typedef std::uniform_int_distribution<> Distribution;
typedef std::minstd_rand Generator;
using namespace std::chrono;

Route::Route()
	:cost(0)
{
}

Route::Route(const Vehdet *vehDetails, const Times *travelTimes)
	: cost(0), vehicleDetails(vehDetails), times(travelTimes) 
{
	
	listLocation point = listOfLocations.end();
	listOfLocations.insert(point, Location(-1, true, &(vehicleDetails->vehReqa) , 0, double(100), cost));
	listOfLocations.insert(point, Location(-1, false, &(vehDetails->vehReqb), 0, double(100), cost));
	listLocation start = listOfLocations.begin();
	listLocation end = start;
	++end;
	start->set_travelTimeToNext(times->get_time(start->get_locationNo(), end->get_locationNo()));
	adjustTimes(start, end, false);
}

bool Route::adjustTimes(const listLocation &start, const listLocation &end, bool oldveh)
{
	
	listLocation it = start;
	listLocation other = start;
	if (start == listOfLocations.begin())// && start != listOfLocations.end())
	{
		it->set_earliestTime2(it->get_absEarliestTime());
		++it;
	}
	else
		--other;

	//int itLocationNo;
	//int otherLocationNo = other->get_locationNo();


	while (it != listOfLocations.end())
	{
		/*itLocationNo = it->get_locationNo();*/
		it->set_earliestTime2(other->get_earliestTime() + other->get_travelTimeToNext());
		if (it->get_earliestTime() > it->get_latestTime() && it != end && it != start)
		{
			if (oldveh)
			{
				std::cout << "earliest time" << std::endl;
			}
			return false;
		}
		++it;
		++other;
		/*otherLocationNo = itLocationNo;*/
	}
	
	it = end;
	other = end;
	++other;

	if (other == listOfLocations.end())
	{
		it->set_latestTime2(it->get_absLatestTime());
		if (it->get_earliestTime() > it->get_latestTime())
			return false;
		it->set_servedTime(it->get_absLatestTime(), cost);
	}
	else
	{
		++it;
		++other;
	}

	/*itLocationNo = it->get_locationNo();*/

	while (it != listOfLocations.begin())
	{
		/*otherLocationNo = itLocationNo;*/
		--it;
		--other;
		/*itLocationNo = it->get_locationNo();*/

		it->set_latestTime2(other->get_latestTime() - it->get_travelTimeToNext());
		if (it->get_earliestTime() > it->get_latestTime())
		{
			if (oldveh)
			{
				std::cout << "latest times" << std::endl;
			}
			return false;
		}
	}

	it = listOfLocations.begin();
	it->reset_working();
	it->chooseBestServedTime(cost);
	other = it;
	++it;
	it->reset_working();

	while (it != listOfLocations.end())
	{
		//it->reset_working();
		it->chooseBestServedTime(cost);
		double travelTime = other->get_travelTimeToNext() + it->get_linkedTimeBack();
		if (other->get_servedTime() + travelTime > it->get_servedTime())
		{
			it->update_working(other->get_workingCostBeta(), travelTime + other->get_workingLatestTime(), travelTime);
			other->linkToNext();
			while (other != listOfLocations.begin() && other->get_linkedToNext())
			{
				--other;
			}
			if (other->get_linkedToNext())
			{
				other = it;
				++it;
				if (it != listOfLocations.end())
				{
					it->reset_working();
				}
			}
		}
		else
		{
			other = it;
			++it;
			if (it != listOfLocations.end())
			{
				it->reset_working();
			}
		}
	}

	other = it; //it is listOfLocations.end()
	--it;
	do
	{
		--it;
		--other;
		if (it->get_linkedToNext())
		{
			it->set_servedTime(other->get_servedTime() - it->get_travelTimeToNext(), cost);
		}
	} while (it != listOfLocations.begin());

	return true;
}

bool Route::insert_RequestAtPoint(const int &requestNo, const Requestsetup *requestDetails, int pickupPointNumber,  int dropoffPointNumberAfterPickup, bool oldVeh)//, tabuContainer &tabuList, int &tabuCounter, bool checkTabu)
{
	double direct = times->get_time(requestDetails->get_pickup().at(0).locationNo, requestDetails->get_dropoff().at(0).locationNo);

	for (Location &locn : listOfLocations)
	{
		locn.fixValues();
	}

	//insert pickup
	pickupPoint = next(listOfLocations.begin(), pickupPointNumber); //iterator to inserted pickup Location. First points to Location after.

	listOfLocations.insert(pickupPoint, Location(requestNo, true, requestDetails, 0, direct, cost));
	--pickupPoint; //Now points to pickup point
	
	//insert dropoff
	listLocation afterDropoffPoint = next(pickupPoint, dropoffPointNumberAfterPickup); //iterator to location after inserted dropoff location.

	listOfLocations.insert(afterDropoffPoint, Location(requestNo, false, requestDetails, 0, direct, cost));
	dropoffPoint = afterDropoffPoint; //iterator to inserted dropoff Location
	--dropoffPoint; 

	listLocation beforeDropoffPoint = dropoffPoint;
	--beforeDropoffPoint;

	/*if (checkTabu)
	{
		timekey temporary_key;

		if (dropoffPoint != listOfLocations.end())
		{
			temporary_key = timekey(dropoffPoint->get_locationNo(), afterDropoffPoint->get_locationNo());
			temporary_key.arrange();
			tabuContainer::iterator entry = tabuList.find(temporary_key);
			if (entry != tabuList.end() && (entry->second + 100 > tabuCounter) )
			{
				return false;
			}
		}

		temporary_key = timekey(beforeDropoffPoint->get_locationNo(), dropoffPoint->get_locationNo());
		temporary_key.arrange();
		tabuContainer::iterator entry = tabuList.find(temporary_key);
		if (entry != tabuList.end() && (entry->second + 100 > tabuCounter) )
		{
			return false;
		}
	}*/

	//Add extra passenger at start
	if (pickupPoint == listOfLocations.begin())
	{
		pickupPoint->update_passengersAfter(vehicleDetails->get_capacity(), 0);
	}
	else
	{
		listLocation before = pickupPoint;
		--before;
		if (!pickupPoint->update_passengersAfter(vehicleDetails->get_capacity(), before->get_passengersAfter()))
		{
			this->undo_insert();
			if (oldVeh)
			{
				std::cout << "capacity1" << std::endl;
			}
			return false;
		}
		before->set_travelTimeToNext(times->get_time(before->get_locationNo(), pickupPoint->get_locationNo()));//change travel time of previous location
	}


	dropoffPoint->update_passengersAfter(vehicleDetails->get_capacity(), beforeDropoffPoint->get_passengersAfter()); //set dropoff point passenger number by previous location

	//Now check capacity constraint
	listLocation it = pickupPoint;
	while (it != dropoffPoint)
	{
		if (!it->update_passengersAfter(vehicleDetails->get_capacity(), 1))
		{
			this->undo_insert();
			if (oldVeh)
			{
				std::cout << "capacity2" << std::endl;
			}
			return false;
		}
		++it;
	}

	//set travel times
	if (dropoffPointNumberAfterPickup != 1)
	{
		listLocation afterPickupPoint = pickupPoint;
		++afterPickupPoint;
		pickupPoint->set_travelTimeToNext(times->get_time(pickupPoint->get_locationNo(), afterPickupPoint->get_locationNo()));
		beforeDropoffPoint->set_travelTimeToNext(times->get_time(beforeDropoffPoint->get_locationNo(), dropoffPoint->get_locationNo()));
	}
	else
	{
		pickupPoint->set_travelTimeToNext(times->get_time(pickupPoint->get_locationNo(), dropoffPoint->get_locationNo()));
	}

	if (afterDropoffPoint != listOfLocations.end())
	{
		dropoffPoint->set_travelTimeToNext(times->get_time(dropoffPoint->get_locationNo(), afterDropoffPoint->get_locationNo()));
	}
	else
	{
		dropoffPoint->set_travelTimeToNext(0);
	}

	//Now update all times and cost
	if (!adjustTimes(pickupPoint, dropoffPoint, oldVeh))
	{
		
		if (oldVeh)
		{
			std::cout << "Adj times" << std::endl;
		}
		this->undo_insert();
		return false;
	}
	else
		return true;
}

bool Route::BestInsert_Request(const int& requestNo, const Requestsetup *requestDetails, bool oldveh)//, tabuContainer &tabuList, int &tabuCounter, bool checkTabu, bool oldveh)
{
	//find earliest feasible insertion points (ignoring travel times)
	//pickup
	int fromp = 1;
	listLocation it = listOfLocations.begin();
	++it;
	while (it != listOfLocations.end() && it->get_latestTime() < requestDetails->get_absEarl())
	{
		++it;
		++fromp;
	}
	//dropoff
	int fromd = fromp;
	double check = requestDetails->get_absEarl() + times->get_time(requestDetails->get_pickup().at(0).locationNo, requestDetails->get_dropoff().at(0).locationNo);
	while (it != listOfLocations.end() && it->get_latestTime() < check)
	{
		++it;
		++fromd;
	}

	//find latest feasible insertion point (ignoring travel times)
	//dropoff
	it = listOfLocations.end();
	--it;
	int tod = listOfLocations.size();
	do
	{
		--it;
		--tod;
	} while (it != listOfLocations.begin() && it->get_earliestTime() > requestDetails->get_absLate());

	//pickup
	int top = tod + 1;
	check = requestDetails->get_absLate() - times->get_time(requestDetails->get_pickup().at(0).locationNo, requestDetails->get_dropoff().at(0).locationNo);
	++it;
	do
	{
		--it;
		--top;
	}
	while (it != listOfLocations.begin() && it->get_earliestTime() > check);

	//if (fromp > tod)
	//	return false;

	//try all possible insertions
	bool feasible= false;
	Route bestRoute (*this);
	Route tempRoute(*this);
	double bestCost = 1.0e30;
	//int besti;
	//int bestj;

	for (int i = fromp; i <= top; ++i)
	{
		for (int j = 1 + max(fromd - i, 0); j <= tod + 1 - i; ++j)
		{	
			if (tempRoute.insert_RequestAtPoint(requestNo, requestDetails, i, j, (oldveh && i==oldi && j==oldj)))//, tabuList, tabuCounter, checkTabu))
			{
				if (tempRoute.get_cost() < bestCost)
				{
					feasible = true;
					bestRoute = tempRoute;
					bestCost = tempRoute.get_cost();
					//besti = i;
					//bestj = j;
				}
				tempRoute.undo_insert();
			}
		}
	}
	*this = bestRoute;
	if (!feasible && oldveh)
	{
		std::cout << "Bah" << std::endl;
	}
	/*else if (feasible)
	{
		listLocation before = next(listOfLocations.begin(), besti + bestj - 1);
		listLocation middle = before;
		++middle;
		listLocation after = middle;
		++after;
		if (after != listOfLocations.end())
		{
			timekey temporary_key = timekey(middle->get_locationNo(), after->get_locationNo());
			temporary_key.arrange();
			tabuList[temporary_key] = tabuCounter;
			++tabuCounter;
			temporary_key = timekey(before->get_locationNo(), middle->get_locationNo());
			temporary_key.arrange();
			tabuList[temporary_key] = tabuCounter;
			++tabuCounter;
		}
	}*/
	return feasible;
}

bool Route::remove_Request(const int &requestNo, bool checkWorthMoving)// , tabuContainer &tabuList, int &tabuCounter, bool checkTabu)
{
	bool worthMoving = false;

	//Find pickup point
	listLocation pickupPoint = listOfLocations.begin();
	oldi = 0;
	oldj = 0;
	while (pickupPoint->get_requestNo() != requestNo)
	{
		++pickupPoint;
		++oldi;
	}

	//Check pickup is making an avoidable contribution to cost
	if (pickupPoint->get_linkedToNext())
		worthMoving = true;
	else if (pickupPoint != listOfLocations.begin())
	{
		listLocation before = pickupPoint;
		--before;
		if (before->get_linkedToNext())
			worthMoving = true;
	}

	//Find associated dropoff
	listLocation dropoffPoint = pickupPoint;
	do
	{
		++dropoffPoint;
		++oldj;
	} while (dropoffPoint->get_requestNo() != requestNo);


	listLocation before = dropoffPoint;
	--before;

	

	//Check dropoff is making an avoidable contribution to cost
	if (dropoffPoint->get_linkedToNext())
	{
		worthMoving = true;
	}
	if (before->get_linkedToNext())
	{
		worthMoving = true;
	}

	if (!worthMoving && checkWorthMoving)
	{	
		//std::cout << "Not worth moving" << std::endl;
		return false;
	}

	/*listLocation after = dropoffPoint;
	++after;
	if (after != listOfLocations.end() && checkTabu)
	{
		if (before == pickupPoint && pickupPoint!= listOfLocations.begin())
		{
			--before;
		}
		if (before != pickupPoint)
		{
			timekey temporary_key = timekey(before->get_locationNo(), after->get_locationNo());
			temporary_key.arrange();
			tabuContainer::iterator entry = tabuList.find(temporary_key);
			if (entry != tabuList.end() && (entry->second + 100 > tabuCounter))
			{
				return false;
			}
			else
			{
				tabuList[temporary_key] = tabuCounter;
				++tabuCounter;
			}
		}
	}*/

	//update passenger numbers to exclude request
	for (listLocation it = pickupPoint; it != dropoffPoint; ++it)
		it->update_passengersAfter(vehicleDetails->get_capacity(), -1);

	//remove cost of request
	cost -= pickupPoint->get_cost();
	cost -= dropoffPoint->get_cost();

	//Remove locations and adjust times for remaining locations
	listLocation start;
	if (pickupPoint != listOfLocations.begin())
	{
		listLocation beforeStart = pickupPoint;
		--beforeStart;

		start = listOfLocations.erase(pickupPoint);
		if (start == dropoffPoint)
			++start;
		if (start != listOfLocations.end())
			beforeStart->set_travelTimeToNext(times->get_time(beforeStart->get_locationNo(), start->get_locationNo()));
	}
	else
	{
		start = listOfLocations.erase(pickupPoint);
		if (start == dropoffPoint)
			++start;
	}

	listLocation end;
	
	if (dropoffPoint != listOfLocations.begin())
	{
		listLocation beforeEnd = dropoffPoint;
		--beforeEnd;

		end = listOfLocations.erase(dropoffPoint);
		if (end != listOfLocations.end())
			beforeEnd->set_travelTimeToNext(times->get_time(beforeEnd->get_locationNo(), end->get_locationNo()));
	}
	else
	{
		end = listOfLocations.erase(dropoffPoint);
	}
	
	if (!listOfLocations.empty())
	{
		if (end == listOfLocations.end())
			--end; //start may be after end here. This isn't a problem in adjustTimes 
		if (start == listOfLocations.end())
			--start;
		adjustTimes(start, end, false);
	}


	return true;
}

void Route::undo_insert()
{
	cost -= pickupPoint->get_cost();
	cost -= dropoffPoint->get_cost();
	listOfLocations.erase(pickupPoint);
	listOfLocations.erase(dropoffPoint);
	for (Location &locn : listOfLocations)
	{
		locn.revert(cost);
	}
}

double Route::get_cost() const
{
	return cost;
}


const list <Location>* Route::get_locations()
{
	return &listOfLocations;
}

vector<Location> Route::selectedRoute()
{
	vector<Location> answer;
	for (listLocation it = listOfLocations.begin(); it != listOfLocations.end(); ++it)
		answer.push_back(*it);
	return answer;
}

bool Route::updateTimes()
{
	listLocation start = listOfLocations.begin();
	listLocation end = listOfLocations.end();
	--end;
	return adjustTimes(start, end, false);
}