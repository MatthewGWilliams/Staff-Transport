#ifndef LOCATION_H
#define LOCATION_H

#include <list>
using std::list;
using std::pair;

#include "Requestsetup.h"


typedef pair < double, bool > updateBackwardsReturn;

class Location
{
	typedef list < Location >::iterator listLocation;
public:
	Location();
	Location(int, bool, const Requestsetup*, int, double, double&);

	void fixValues();

	//for setting up change to route at insertion and removal points, before updating all earliest and latest times.

	void set_earliestTime2(const double&);
	void set_latestTime2(const double&);
	void set_servedTime(const double&, double&);
	void chooseBestServedTime(double&);
	void set_travelTimeToNext(const double&);
	void linkToNext();
	
	void reset_working();
	void update_working(const double&, const double&, const double&);

	bool update_passengersAfter(const int&, const int&);

	double change_collectNo(); //Currently does nothing. Will return walking cost.

	int get_requestNo() const;
	bool get_pickup() const;
	int get_locationNo() const;
	double get_earliestTime() const;
	double get_latestTime() const;
	double get_servedTime() const;
	double get_absEarliestTime() const;
	double get_absLatestTime() const;
	int get_passengersAfter() const;
	double get_cost() const;
	double get_travelTimeToNext() const;
	double get_workingCostBeta() const;
	double get_workingLatestTime() const;
	bool get_linkedToNext() const;
	double get_linkedTimeBack() const;

	void revert(double &);

private:
	int requestNo;
	bool pickup; //if false then delivery
	const Requestsetup *requestDetails;
	double directTime;
	
	double earliestTime;
	double latestTime;
	double servedTime;
	double slackBefore; // spare time between previous earliest time and this earliest time.
	double slackAfter; //spare time beteween latest time and latest time of next location.
	
	double costBeta;
	double workingCostBeta;
	double workingLatestTime;
	bool linkedToNext;
	double linkedTimeBack;

	double shiftTimeBeforeLatest; //keep track of changes in service time.
	int collectNo;
	
	int passengersAfter;
	double cost;
	double travelTimeToNext;

	//keep record before change so that can revert
	double fixed_earliestTime;
	double fixed_latestTime;
	double fixed_servedTime;
	double fixed_slackBefore;
	double fixed_slackAfter;
	int fixed_passengersAfter;
	double fixed_cost;
	double fixed_travelTimeToNext;
};

#endif