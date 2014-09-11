#ifndef VEHDET_H
#define VEHDET_H
#include "Requestsetup.h"

class Vehdet
{
public:
	Vehdet();
	Vehdet(int, int, int, int, double, double);

	int get_vehicleNo() const;
	int get_sink() const;
	int get_source() const;
	int get_capacity() const;
	double get_earliest() const;
	double get_latest() const;
	Requestsetup vehReqa;
	Requestsetup vehReqb;

private:
	int vehicleNo;
	int sink;
	int source;
	int capacity;
	double earliest;
	double latest;

};

#endif