#ifndef SOLUTION_H
#define SOLUTION_H

#include "Route.h"
#include "Times.h"

#include <vector>
#include <random>

using std::vector;
typedef map< int, Requestsetup, less< int > > Reqmap;
typedef std::minstd_rand Generator;


class Solution
{
public:
	Solution();
	Solution(const Reqmap*, const vector<Vehdet>*, const Times*, const vector<int> &, const vector<int> &);
	bool solve();
	bool updateTimes();

	vector<Location> get_route(int);
	double get_cost() const;
	bool get_feasible() const;

private:
	bool attemptInsert(const int&);
	bool ChooseBestVehicle(const int&, int oldveh=-1);
	bool improvingShift(bool);
	bool imporvingExchange();

	vector<Route> vehicles;
	const Reqmap *requests;
	const Times *times;
	double cost;
	vector<int> mustServe;
	vector<int> otherRequests;
	vector<int> unservedRequests;
	vector<vector<int>> whereHaveTheRequestsGone;
	list< vector< double > > sortedVehicles;
	bool feasible;
	Generator g;
	//tabuContainer tabuList;
	//int tabuCount;
};

#endif