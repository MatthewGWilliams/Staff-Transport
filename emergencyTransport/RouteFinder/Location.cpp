#include "Location.h"
#include "Requestsetup.h"
#include <list>
#include <algorithm>

using std::list;
using std::max;

typedef list < Location >::iterator listLocation;

namespace
{
	double sqDiff(const double &x,  const double &y)
	{
		if (x <= y)
			return 0;
		else
		{
			double temp = x - y;
			return temp * temp;
		}
	}
}

Location::Location()
	:requestNo(0), pickup(0), directTime(0), earliestTime(0), latestTime(0), servedTime(0), slackBefore(0), slackAfter(0), linkedTimeBack(0), shiftTimeBeforeLatest(0), collectNo(0), passengersAfter(0), cost(0)
{}

Location::Location(int reqNo, bool pick, const Requestsetup *reqDet, int collNo, double direct, double &routeCost)
	: requestNo(reqNo), pickup(pick), requestDetails(reqDet), directTime(direct), earliestTime(0), latestTime(0), servedTime(0), slackBefore(0), slackAfter(0), linkedTimeBack(0), shiftTimeBeforeLatest(0), collectNo(collNo), passengersAfter(0), cost(0), travelTimeToNext(0)
{
	if (!pickup && !requestDetails->get_inOrOut())
	{
		cost = -requestDetails->get_absEarl() / direct;
		costBeta = 1 / direct;
	}
	else if (pickup && requestDetails->get_inOrOut())
	{
		cost = requestDetails->get_absLate() / direct;
		costBeta = -1 / direct;
	}
	else
	{
		cost = 0;
		costBeta = 0;
	}
	workingCostBeta = costBeta;
	routeCost += cost;
}

void Location::fixValues()
{
	fixed_earliestTime = earliestTime;
	fixed_latestTime = latestTime;
	fixed_servedTime = servedTime;
	fixed_slackBefore = slackBefore;
	fixed_slackAfter = slackAfter;
	fixed_passengersAfter = passengersAfter;
	fixed_cost = cost;
	fixed_travelTimeToNext = travelTimeToNext;
}

//for setting up change to route at insertion and removal points, before updating all earliest and latest times.
void Location::set_earliestTime2(const double &time)
{
	double temp = requestDetails->get_absEarl() - time;
	if (temp <= 0)
	{
		earliestTime = time;
		slackBefore = 0;
	}
	else
	{
		earliestTime = requestDetails->get_absEarl();
		slackBefore = temp;
	}
}

void Location::set_latestTime2(const double &time)
{
	double temp = time - requestDetails->get_absLate();
	if (temp <= 0)
	{
		latestTime = time;
		slackAfter = 0;
	}
	else
	{
		latestTime = requestDetails->get_absLate();
		slackAfter = temp;
	}
}

void Location::set_servedTime(const double &time, double &routeCost)
{
	if (servedTime != time)
	{	
		double tempCost = 0;
		{
			if (!pickup)
			{
				if (requestDetails->get_inOrOut() == true)
					tempCost = (sqDiff(time, requestDetails->get_idealTime()) - sqDiff(servedTime, requestDetails->get_idealTime())) * requestDetails->get_priority() * 10000;
				else
					tempCost = (time - servedTime) / directTime; //adjust to include 3rd objective function
			}
			else if (requestDetails->get_inOrOut() == true)
				tempCost = (servedTime - time) / directTime;
		}
		servedTime = time;
		routeCost += tempCost;
		cost += tempCost;
	}
}

void Location::chooseBestServedTime(double &routeCost)
{
	if (requestDetails->get_inOrOut() && !pickup)
	{
		if (workingCostBeta <= 0)
			set_servedTime(max(earliestTime, requestDetails->get_idealTime()), routeCost);
		else
			set_servedTime(earliestTime, routeCost);
	}
	else if (workingCostBeta < 0)
		set_servedTime(workingLatestTime, routeCost);
	else if (workingCostBeta > 0 || (!requestDetails->get_inOrOut() && pickup))
		set_servedTime(earliestTime, routeCost);
	else
		set_servedTime((workingLatestTime + earliestTime) / 2, routeCost);
}

void Location::set_travelTimeToNext(const double &time)
{
	travelTimeToNext = time;
}

void Location::linkToNext()
{
	linkedToNext = true;
}

void Location::reset_working() 
{
	workingCostBeta = costBeta;
	workingLatestTime = latestTime;
	linkedToNext = false;
	linkedTimeBack = 0;
}

void Location::update_working(const double &otherBeta, const double &newWorkingLatest, const double &otherTime)
{
	workingCostBeta += otherBeta;
	workingLatestTime = newWorkingLatest;
	linkedTimeBack = otherTime;
}

bool Location::update_passengersAfter(const int &capacity, const int &changeInPassengers)
{
	passengersAfter += changeInPassengers;
	if (passengersAfter > capacity)
		return false;
	else
		return true;
}

double Location::change_collectNo()
{
	return 0;
}

int Location::get_requestNo() const
{
	return requestNo;
}

bool Location::get_pickup() const
{
	return pickup;
}

int Location::get_locationNo() const
{	
	if (pickup == true)
		return (requestDetails->get_pickup()[collectNo].locationNo);
	else
		return (requestDetails->get_dropoff()[collectNo].locationNo);
}


double Location::get_earliestTime() const
{
	return earliestTime;
}

double Location::get_latestTime() const
{
	return latestTime;
}

double Location::get_servedTime() const
{
	return servedTime;
}

double Location::get_absEarliestTime() const
{
	return requestDetails->get_absEarl();
}

double Location::get_absLatestTime() const
{
	return requestDetails->get_absLate();
}

int Location::get_passengersAfter() const
{
	return passengersAfter;
}

double Location::get_cost() const
{
	return cost;
}

double Location::get_travelTimeToNext() const
{
	return travelTimeToNext;
}

double Location::get_workingCostBeta() const
{
	return workingCostBeta;
}

double Location::get_workingLatestTime() const
{
	return workingLatestTime;
}

bool Location::get_linkedToNext() const
{
	return linkedToNext;
}

double Location::get_linkedTimeBack() const
{
	return linkedTimeBack;
}

void Location::revert(double &routeCost)
{
	routeCost += fixed_cost - cost;

	earliestTime = fixed_earliestTime;
	latestTime = fixed_latestTime;
	servedTime = fixed_servedTime;
	slackBefore = fixed_slackBefore;
	slackAfter = fixed_slackAfter;
	passengersAfter = fixed_passengersAfter;
	cost = fixed_cost;
	travelTimeToNext = fixed_travelTimeToNext;
}