from libcpp.vector cimport vector
from cpython cimport bool

cdef extern from "Requestsetup.h":
	cdef cppclass LocElement:
		LocElement(int locNo, int walk) except +
	
cdef extern from "Problem.h":
	cdef cppclass Problem:
		Problem(double) except +
		
		void insert_Request(int requestNo, bint inOut, double Earl, double Late, int priority, double ideal, vector[LocElement]& pickup, vector[LocElement]& dropoff)
		void insert_Vehicle(int vehicleNo, int sink, int source, int capacity, double earliest, double latest)
		void insert_time(const int&, const int&, const double&)
		
		void setup_solution()
		
		double get_time(const int&, const int&)
		vector[vector[vector[double]]] returnSolution()
		void updateSolutions()
		

cdef class PyProblem:
	cdef Problem *thisptr	# hold a c++ instance
	def __cinit__(self, float journeyTimeMultiplier):
		self.thisptr = new Problem(journeyTimeMultiplier)
	def __dealloc__(self):
		del self.thisptr
		
	def insert_Request(self, int requestNo, bool inOut, float earliestTime, float latestTime, int priority, float ideal, list pickup, list dropoff): 
		#setup pickup and dropoff vectors
		cdef vector[LocElement] pick
		for loc in pickup:
			pick.push_back(LocElement(loc[0], loc[1]))
		cdef vector[LocElement] drop
		for loc2 in dropoff:
			drop.push_back(LocElement(loc2[0], loc2[1]))
		self.thisptr.insert_Request(requestNo, inOut, earliestTime, latestTime, priority, ideal, pick, drop)
		return 0
	
	def insert_Vehicle(self, int vehicleNo, int sink, int source, int capacity, float earliest, float latest):
		self.thisptr.insert_Vehicle(vehicleNo, sink, source, capacity, earliest, latest)
		
	def insert_time(self, int locationNum1, int locationNum2, float time):
		self.thisptr.insert_time(locationNum1, locationNum2, time)
	
	def setup_solution(self):
		self.thisptr.setup_solution()
	
	def get_time(self, int locationNum1, int locationNum2):
		return self.thisptr.get_time(locationNum1, locationNum2)

	def returnSolution(self):
		return self.thisptr.returnSolution()
		
	def updateSolutions(self):
		self.thisptr.updateSolutions()