//Problem class definition. Class will hold details of Pickup and Delivery with Time Windows problem,
//solution objects to the problem and methods to optimise the solution.
#ifndef PROBLEM_H
#define PROBLEM_H

#include "Requestsetup.h"
#include "Times.h"
#include "Vehdet.h"
#include "Solution.h"

#include <map>
#include <vector>
using std::vector;
using std::map;
using std::less;

typedef map< int, Requestsetup, less< int > > Reqmap;
typedef map< int, Vehdet > Vehmap;

class Problem
{
public:
	Problem();
	Problem(double);

	void insert_Request(int, bool, double, double, int, double, const Loc&, const Loc&);
	void insert_Vehicle(int, int, int, int, double, double);
	void insert_time(const int&, const int&, const double&);
	void partitionRequests();
	void setup_solution();

	Requestsetup get_request(int);
	Vehdet get_vehicle(int);
	double get_time(const int&, const int&);

	vector <vector <vector < double > > > returnSolution();
	void updateSolutions();
	int get_count();
	double get_cost();

private:
	Reqmap requestMap;
	vector<Vehdet> vehicleVector;
	Times timeMap;
	double journeyTimeMultiplier;
	list<Solution> bestSolutions;
	vector<int> mustServe;
	vector<int> otherRequests;
	int count;
};

#endif