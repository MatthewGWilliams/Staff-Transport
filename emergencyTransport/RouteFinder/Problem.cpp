#include "Problem.h"

#include <map>
#include <iostream>
#include <thread>
//using std::thread;
using std::map;
using std::less;


typedef map< int, Requestsetup > Reqmap;
typedef map< int, Vehdet > Vehmap;

bool compareSolutions(const Solution &first, const Solution &second)
{
	if (first.get_cost() < second.get_cost())
		return true;
	else
		return false;
}

Problem::Problem()
	:journeyTimeMultiplier(6)
{

}

Problem::Problem(double multiplier)
	: journeyTimeMultiplier(multiplier), count(0)
{
}

void Problem::insert_Request(int requestNo, bool inOut, double Earl, double Late, int pri, double ideal, const Loc& pick, const Loc& drop)
{
	if (Earl == -1)
		requestMap[requestNo] = Requestsetup(inOut, ideal - journeyTimeMultiplier * timeMap.get_time(pick[0].locationNo, drop[0].locationNo) , Late, pri, ideal, pick, drop);
	else if (Late == -1)
		requestMap[requestNo] = Requestsetup(inOut, Earl, Earl + journeyTimeMultiplier * timeMap.get_time(pick[0].locationNo, drop[0].locationNo), pri, ideal, pick, drop);
	else
	requestMap[requestNo] = Requestsetup (inOut, Earl, Late, pri, ideal, pick, drop);
}

void Problem::insert_Vehicle(int vehicleNo, int sink, int source, int capacity, double earliest, double latest)
{
	vehicleVector.push_back(Vehdet(vehicleNo, sink, source, capacity, earliest, latest));
}

void Problem::insert_time(const int& a, const int& b, const double& time)
{
	timeMap.insert_time(a, b, time);
}

void Problem::partitionRequests()
{
	for (Reqmap::iterator it = requestMap.begin(); it != requestMap.end(); ++it)
	{
		if (it->second.get_priority() > 2)
			mustServe.push_back(it->first);
		else
			otherRequests.push_back(it->first);
	}
}

void Problem::setup_solution()
{	
	partitionRequests();

	//Lambda used in threading version
	/*auto trySolving = [](Solution *sol){
		sol->solve();
	};*/

	while (bestSolutions.size() < 40 && count < 10000)
	{
		Solution tempSol(&requestMap, &vehicleVector, &timeMap, mustServe, otherRequests);
		tempSol.solve();
		++count;
		if (tempSol.get_feasible())
		{
			bestSolutions.push_back(tempSol);
			bestSolutions.sort(compareSolutions);
		}

		//Threading to increase speed of search. Currently unused due to compatability issues with Python.
		/*count += 4;
		Solution tempSol1(&requestMap, &vehicleVector, &timeMap, mustServe, otherRequests);
		Solution tempSol2(&requestMap, &vehicleVector, &timeMap, mustServe, otherRequests);
		Solution tempSol3(&requestMap, &vehicleVector, &timeMap, mustServe, otherRequests);
		Solution tempSol4(&requestMap, &vehicleVector, &timeMap, mustServe, otherRequests);

		thread t1 = thread(trySolving, &tempSol1);
		thread t2 = thread(trySolving, &tempSol2);
		thread t3 = thread(trySolving, &tempSol3);
		thread t4 = thread(trySolving, &tempSol4);

		t1.join();
		if (tempSol1.get_feasible())
		{
			bestSolutions.push_back(tempSol1);
			bestSolutions.sort(compareSolutions);
		}

		t2.join();
		if (tempSol2.get_feasible())
		{
			bestSolutions.push_back(tempSol2);
			bestSolutions.sort(compareSolutions);
		}

		t3.join();
		if (tempSol3.get_feasible())
		{
			bestSolutions.push_back(tempSol3);
			bestSolutions.sort(compareSolutions);
		}

		t4.join();
		if (tempSol4.get_feasible())
		{
			bestSolutions.push_back(tempSol3);
			bestSolutions.sort(compareSolutions);
		}*/

	}
	std::cout << count << " solutions tried." << std::endl;
	if (!bestSolutions.empty())
	{
		std::cout << "Best solution cost is " << bestSolutions.begin()->get_cost() << std::endl;
	}
}

Requestsetup Problem::get_request(int requestNo)
{
	return requestMap.at(requestNo);
}

Vehdet Problem::get_vehicle(int vehicleNo)
{
	return vehicleVector.at(vehicleNo);
}

double Problem::get_time(const int& a, const int& b)
{
	return timeMap.get_time(a, b);
}

vector< vector< vector< double > > > Problem::returnSolution()
{
	vector< vector< vector< double > > > answer;
	vector<Location> routeRecord;
	vector< vector< double > > vehicleRecord;
	vector < double > locationRecord;
	if (!bestSolutions.empty())
	{
		for (vector<Vehdet>::size_type routeNo = 0; routeNo != vehicleVector.size(); ++routeNo)
		{
			vehicleRecord.assign(1, vector < double > { { double(vehicleVector[routeNo].get_vehicleNo()) } });
			routeRecord = bestSolutions.begin()->get_route(routeNo);
			for (vector<Location>::size_type i = 0; i != routeRecord.size(); ++i)
			{
				vehicleRecord.push_back(vector < double > { {double(routeRecord.at(i).get_requestNo()), double(routeRecord.at(i).get_pickup()), routeRecord.at(i).get_servedTime(), double(routeRecord.at(i).get_locationNo()), double(routeRecord.at(i).get_passengersAfter())} });
			}
			answer.push_back(vehicleRecord);
		}
	}
	return answer;
}

void Problem::updateSolutions()
{
	list<Solution>::iterator it = bestSolutions.begin();
	while (it != bestSolutions.end())
	{
		if (!it->updateTimes())
			bestSolutions.erase(it);
		else
			++it;
	}
}

int Problem::get_count()
{
	return count;
}

double Problem::get_cost()
{
	if (!bestSolutions.empty())
	{
		return bestSolutions.begin()->get_cost();
	}
	else
	{
		return -1;
	}
}