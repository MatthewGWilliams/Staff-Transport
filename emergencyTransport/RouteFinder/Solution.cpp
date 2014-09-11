#include "Solution.h"
#include "Route.h"
#include <random>
#include <chrono>
#include <algorithm>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

typedef std::uniform_int_distribution<> Distribution;
typedef std::minstd_rand Generator;
using namespace std::chrono;

bool compareVehicles(const vector<double> &first, const vector<double> &second)
{
	
	if (first.at(1) < second.at(1))
		return true;
	else
		return false;
}

Solution::Solution()
	:cost(0), feasible(true)
{

}

Solution::Solution(const Reqmap *theRequests, const vector<Vehdet> *theVehicles, const Times *theTimes, const vector<int> &mustServeFromProb, const vector<int> &otherRequestsFromProb)
	: requests(theRequests), times(theTimes), cost(0), mustServe(mustServeFromProb), otherRequests(otherRequestsFromProb), feasible(true)
{
	//Initialize containers for vehicles and their cost
	double j = 0;
	for (vector<Vehdet>::size_type i = 0; i != theVehicles->size(); ++i)
	{
		vehicles.push_back(Route(&(theVehicles->at(i)), times));
		sortedVehicles.push_back(vector < double > {{j, 0}});
		++j;
	}

	//Shuffle vectors of requests
	high_resolution_clock::time_point tp = high_resolution_clock::now();
	high_resolution_clock::duration dtn = tp.time_since_epoch();
	int seed = dtn.count() % 10000000;
	g = Generator(seed);
	shuffle(mustServe.begin(), mustServe.end(), g);

	shuffle(otherRequests.begin(), otherRequests.end(), g);
}

bool Solution::solve()
{
	//int counterLimit = 10;
	for (vector<int>::size_type i = 0; i != mustServe.size(); ++i)
	{
		if (attemptInsert(mustServe[i]))
		{
			sortedVehicles.sort(compareVehicles);
		}
		else
		{
			feasible = false;
			return false;
		}
	}

	for (vector<int>::size_type i = 0; i != otherRequests.size(); ++i)
	{
		if (!attemptInsert(otherRequests[i]))
		{
			unservedRequests.push_back(otherRequests[i]);
			cost += requests->at(otherRequests[i]).get_priority() * 100000000;
		}
		sortedVehicles.sort(compareVehicles);
	}
	/*bool cont = true;
	int i = 0;
	while (cont)
	{
		++i;
		if (i < counterLimit)
		{
			cont = (improvingShift(true) || imporvingExchange());
		}
		else
		{
			cont = false;
		}
	}*/
	return true;
}

bool Solution::attemptInsert(const int &requestNo)
{
	int counter = 0;
	int counterLimit = 10;
	bool cont = true;
	while (cont && !ChooseBestVehicle(requestNo))
	{
		++counter;
		if (!whereHaveTheRequestsGone.empty() && counter < counterLimit)
		{
			cont = (improvingShift(false) || imporvingExchange());
		}
		else
		{
			cont = false;
		}
	}
	if (counter == counterLimit || !cont)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool Solution::updateTimes()
{
	for (vector<Route>::size_type routeNo = 0; routeNo != vehicles.size(); ++routeNo)
	{
		if (!vehicles.at(routeNo).updateTimes())
			return false;
	}
	return true;
}

vector<Location> Solution::get_route(int routeNo) 
{
	vector<Location> answer = vehicles.at(routeNo).selectedRoute();
	return answer;
}

double Solution::get_cost() const
{
	double current_cost = cost;
	for (vector<Route>::size_type i = 0; i != vehicles.size(); ++i)
		current_cost += vehicles.at(i).get_cost();
	return current_cost;
}

bool Solution::ChooseBestVehicle(const int &requestNo, int oldveh)
{
	Route temporaryRoute;
	double temporaryCost;
	Route bestRoute;
	int bestVehicle = -1;
	double bestCost = 1.0e30;
	int vehicleNum;
	list< vector < double > >::iterator bestIt;
	for (list<vector < double > >::iterator it = sortedVehicles.begin(); it != sortedVehicles.end(); ++it)
	{
		vehicleNum = int(it->at(0));
		if (bestCost > 0)
		{
			temporaryRoute = vehicles.at(vehicleNum);
			temporaryCost = temporaryRoute.get_cost();
			if (temporaryRoute.BestInsert_Request(requestNo, &(requests->at(requestNo)), (vehicleNum == oldveh)))// tabuList, tabuCount, !(vehicleNum == oldveh), (vehicleNum == oldveh)))
			{
				if (temporaryRoute.get_cost() - temporaryCost < bestCost)
				{
					bestCost = temporaryRoute.get_cost() - temporaryCost;
					bestRoute = temporaryRoute;
					bestVehicle = vehicleNum;
					bestIt = it;
				}
			}
		}
	}
	if (bestVehicle != -1)
	{
		vehicles[bestVehicle] = bestRoute;
		bestIt->at(1) = bestCost;
		whereHaveTheRequestsGone.push_back({ {requestNo, bestVehicle} });
		return true;
	}
	else
		return false;
}

bool Solution::improvingShift(bool checkIfWothMoving)
{
	//Only for use if some requests have been assigned!
	shuffle(whereHaveTheRequestsGone.begin(), whereHaveTheRequestsGone.end(), g);

	double currentCost = get_cost();
	size_t i = 0;
	do {
		int requestNo = whereHaveTheRequestsGone.at(i).at(0);
		int vehicleNo = whereHaveTheRequestsGone.at(i).at(1);
		Route ahhh(vehicles.at(vehicleNo));
		if (vehicles.at(vehicleNo).remove_Request(requestNo, checkIfWothMoving))//, tabuList, tabuCount, false)) //If request is worth moving then it is removed from the route. Otherwise route unchanged.
		{
			whereHaveTheRequestsGone.erase(whereHaveTheRequestsGone.begin() + i);
			if (!ChooseBestVehicle(requestNo, vehicleNo))
			{
				return false;
				cost += 1e30; //Shouldn't happen, if it does makes solution useless so set cost to huge
				/*std::cout << "Cheat! " << requestNo << std::endl;
				std::ofstream outCarFile("C:/Users/Matthew/Dropbox/Cardiff MSc/Creating test sets/Badcar.csv", std::ios::out);
				outCarFile << "reqNo" << "," << "pickup?" << "," << "servedTime" << "," << "passAft" << "," << std::endl;
				for (size_t location = 0; location != ahhh.selectedRoute().size(); ++location)
				{
					outCarFile << ahhh.selectedRoute().at(location).get_requestNo() << "," << ahhh.selectedRoute().at(location).get_pickup() << "," << ahhh.selectedRoute().at(location).get_servedTime() << "," << ahhh.selectedRoute().at(location).get_passengersAfter() << "," << std::endl;
				}*/
			}
			if (currentCost > get_cost())
				return true;
			else
				return false;
		}
		else
		{
			++i;
		}
	} while (i != whereHaveTheRequestsGone.size());

	return false; //No shift worth making
}

bool Solution::imporvingExchange()
{
	//Only for use if some requests have been assigned!
	shuffle(whereHaveTheRequestsGone.begin(), whereHaveTheRequestsGone.end(), g);

	double currentCost = get_cost();
	size_t i = 0;
	do {
		int request1No = whereHaveTheRequestsGone.at(i).at(0);
		int vehicle1No = whereHaveTheRequestsGone.at(i).at(1);
		if (vehicles.at(vehicle1No).remove_Request(request1No, false))//, tabuList, tabuCount, false)) //If request is worth moving then it is removed from the route. Otherwise route unchanged.
		{
			for (size_t j = i+1; j != whereHaveTheRequestsGone.size(); ++j)
			{
				int request2No = whereHaveTheRequestsGone.at(j).at(0);
				int vehicle2No = whereHaveTheRequestsGone.at(j).at(1);
				if (vehicles.at(vehicle2No).remove_Request(request2No, false))//, tabuList, tabuCount, false))
				{
					if (vehicles.at(vehicle1No).BestInsert_Request(request2No, &(requests->at(request2No)), false))// tabuList, tabuCount, true, false))
					{
						if (vehicles.at(vehicle2No).BestInsert_Request(request1No, &(requests->at(request1No)), false))// tabuList, tabuCount, true, false))
						{
							if (currentCost > get_cost())
							{
								whereHaveTheRequestsGone.at(i).at(1) = vehicle2No;
								whereHaveTheRequestsGone.at(j).at(1) = vehicle1No;
								return true;
							}
							else
							{
								vehicles.at(vehicle1No).remove_Request(request2No, false);//, tabuList, tabuCount, false);
								vehicles.at(vehicle2No).remove_Request(request1No, false);//, tabuList, tabuCount, false);
							}
						}
						else
						{
							vehicles.at(vehicle1No).remove_Request(request2No, false);// , tabuList, tabuCount, false);
						}
					}
					vehicles.at(vehicle2No).BestInsert_Request(request2No, &(requests->at(request2No)), false);// tabuList, tabuCount, false, false);
					vehicles.at(vehicle1No).BestInsert_Request(request1No, &(requests->at(request1No)), false);// tabuList, tabuCount, false, false);
					return false;
				}
			}
			vehicles.at(vehicle1No).BestInsert_Request(request1No, &(requests->at(request1No)), false);// tabuList, tabuCount, false, false);
			++i;
		}
		else
		{
			++i;
		}
	} while (i != whereHaveTheRequestsGone.size() - 2);
	return false;
}

bool Solution::get_feasible() const
{
	return feasible;
}