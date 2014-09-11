#ifndef ROUTE_H
#define ROUTE_H

#include "Location.h"
#include "Requestsetup.h"
#include "Times.h"
#include "Vehdet.h"

typedef list < Location >::iterator listLocation;
//typedef map < timekey, int > tabuContainer;

class Route
{
public:
	Route();
	Route(const Vehdet*, const Times*);

	bool insert_RequestAtPoint(const int&, const Requestsetup*, int, int, bool);//, tabuContainer&, int&, bool);
	bool BestInsert_Request(const int&, const Requestsetup*, bool oldveh = false);//tabuContainer&, int&, bool, bool oldveh = false);
	bool remove_Request(const int&, bool);//, tabuContainer&, int&, bool);

	void undo_insert();

	double get_cost() const;
	const list <Location>* get_locations();
	vector<Location> selectedRoute();
	bool updateTimes();

private:
	bool adjustTimes(const listLocation&, const listLocation&, bool);

	list <Location> listOfLocations;
	double cost;
	const Vehdet *vehicleDetails;
	const Times *times;

	listLocation pickupPoint;
	listLocation dropoffPoint;
	int oldi;
	int oldj;
};

#endif