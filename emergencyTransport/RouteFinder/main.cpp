#include "Requestsetup.h"
#include "Problem.h"
#include "Route.h"
#include <vector>
#include <iostream>
#include <map>
#include <list>
#include <chrono>
#include <string>
#include <fstream>
#include <sstream>

using std::cout;
using std::endl;
using std::cin;
using namespace std::chrono;

using std::vector;
using std::map;
using std::list;
typedef std::map< int, Requestsetup, less< int > > Reqmap;

using std::string;
using std::stringstream;


int main()
{
	double multiplier;
	cin >> multiplier;

	vector<int> iterations;
	vector<double> time;
	vector<double> cost;

	for (int i = 0; i < 100; ++i)
	{
		Problem SatAm(multiplier);

		string line;
		std::ifstream inTimesFile("C:/Users/Matthew/Dropbox/Cardiff MSc/Creating test sets/SaturdayAMTimes.csv");

		while (std::getline(inTimesFile, line))
		{
			stringstream linestream(line);
			string value;
			vector<double> numbers;
			while (getline(linestream, value, ','))
			{
				double d = std::stod(value);
				numbers.push_back(d);
			}
			SatAm.insert_time(int(numbers[0]), int(numbers[1]), numbers[2]);
		}

		std::ifstream inRequestsFile("C:/Users/Matthew/Dropbox/Cardiff MSc/Creating test sets/SaturdayAMRequestsFormatted.csv");

		int reqNo = 1;
		while (std::getline(inRequestsFile, line))
		{
			stringstream linestream(line);
			string value;
			vector<double> numbers;
			while (getline(linestream, value, ','))
			{
				double d = std::stod(value);
				numbers.push_back(d);
			}
			SatAm.insert_Request(reqNo, bool(numbers[6]), numbers[2], numbers[3], int(numbers[5]), numbers[4], Loc{ { int(numbers[0]), 0 } }, Loc{ { int(numbers[1]), 0 } });
			++reqNo;
		}

		SatAm.insert_Vehicle(1, 1, 1, 4, 60, 780);
		SatAm.insert_Vehicle(2, 1, 1, 4, 60, 780);
		SatAm.insert_Vehicle(3, 1, 1, 4, 60, 780);
		SatAm.insert_Vehicle(4, 1, 1, 7, 60, 780);

		SatAm.insert_Vehicle(5, 1, 1, 4, 60, 780);
		//SatAm.insert_Vehicle(6, 1, 1, 4, 0, 720);

		//Problem now set up. Solve it.
		time_point<system_clock> start = system_clock::now();
		SatAm.setup_solution();
		time_point<system_clock> end = system_clock::now();
		duration<double> elaspedSeconds = end - start;
		//cout << elaspedSeconds.count() << endl;
		iterations.push_back(SatAm.get_count());
		time.push_back(elaspedSeconds.count());
		cost.push_back(SatAm.get_cost());
	}
	
	std::ofstream results("C:/Users/Matthew/Dropbox/Cardiff MSc/Creating test sets/results.csv", std::ios::out);
	results << "iterations" << "," << "time" <<  "," << "cost" << "," << endl;
	for (int i = 0; i < 100; ++i)
	{
		results << iterations[i] << "," << time[i] << "," << cost[i] << "," << endl;
	}

	//Output solution to csv files.
	/*vector< vector < vector < double > > > answer = SatAm.returnSolution();
	for (signed int car = 0; car != answer.size(); ++car)
	{
		string s = std::to_string(car);
		std::ofstream outCarFile("C:/Users/Matthew/Dropbox/Cardiff MSc/Creating test sets/car" + s + ".csv", std::ios::out);
		outCarFile << "reqNo" << "," << "pickup?" << "," << "servedTime" << "," << "locNo" << "," << "passengersAfter" << "," << endl;
		for (int location = 1; location != answer.at(car).size(); ++location)
		{
			outCarFile << answer.at(car).at(location).at(0) << "," << answer.at(car).at(location).at(1) << "," << answer.at(car).at(location).at(2) << "," << answer.at(car).at(location).at(3) << "," << answer.at(car).at(location).at(4) << "," << endl;
		}
	}

	cout << "done" << endl;
	char waiting[100];
	std::cin >> waiting;
	return 0;*/


	/*Problem myProb;

	myProb.insert_Request(1, true, 0, 100, 8, 40, Loc{ { 1, 0 } }, Loc{ { 2, 0 } });
	myProb.insert_Request(2, false, 77, 130, 8, 130, Loc{ { 3, 0 } }, Loc{ { 4, 0 } });
	myProb.insert_Request(3, true, 0, 49, 8, 20, Loc{ { 5, 0 } }, Loc{ { 6, 0 } });

	myProb.insert_time(1, 2, 40);
	myProb.insert_time(1, 3, 20);
	myProb.insert_time(1, 4, 50);
	myProb.insert_time(1, 5, 5);
	myProb.insert_time(1, 6, 40);
	myProb.insert_time(2, 3, 30);
	myProb.insert_time(2, 4, 60);
	myProb.insert_time(2, 5, 40);
	myProb.insert_time(2, 6, 0);
	myProb.insert_time(3, 4, 50);
	myProb.insert_time(3, 5, 25);
	myProb.insert_time(3, 6, 30);
	myProb.insert_time(4, 5, 55);
	myProb.insert_time(4, 6, 60);
	myProb.insert_time(5, 6, 40);

	myProb.insert_Vehicle(1, 1, 4);*/
	//myProb.insert_Vehicle(1, 1, 4);

	/*time_point<system_clock> start2 = system_clock::now();
	myProb.setup_solution();
	time_point<system_clock> end2 = system_clock::now();
	duration<double> elaspedSeconds2 = end2 - start2;
	cout << elaspedSeconds2.count() << endl;*/

	////Output solution to csv files.
	//vector< vector < vector < double > > > answer2 = myProb.returnSolution();
	//for (signed int car = 0; car != answer2.size(); ++car)
	//{
	//	string s = std::to_string(car);
	//	std::ofstream outCarFile("C:/Users/Matthew/Dropbox/Cardiff MSc/Creating test sets/car" + s + ".csv", std::ios::out);
	//	outCarFile << "reqNo" << "," << "pickup?" << "," << "servedTime" << "," << endl;
	//	for (int location = 0; location != answer2.at(car).size(); ++location)
	//	{
	//		outCarFile << answer2.at(car).at(location).at(0) << "," << answer2.at(car).at(location).at(1) << "," << answer2.at(car).at(location).at(2) << "," << endl;
	//	}
	//}



	//cout << (myRoute.get_locations()->begin())->get_servedTime() << endl;

	//cout << (--anotherRoute.get_locations()->end())->get_requestNo() << endl;
	//cout << (--myRoute.get_locations()->end())->get_requestNo() << endl;
	
}