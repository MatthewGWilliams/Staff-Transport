#include "Vehdet.h"

Vehdet::Vehdet()
	:vehicleNo(0), sink(0), source(0), capacity(0)
{}

Vehdet::Vehdet(int vehNo, int sin, int sou, int cap, double earl, double late)
	: vehicleNo(vehNo), sink(sin), source(sou), capacity(cap), earliest(earl), latest(late)
{
	vehReqa = Requestsetup(true, earliest, latest, 1, 0, Loc{ { source, 0 } }, Loc{ { sink, 0 } });
	vehReqb = Requestsetup(false, earliest, latest, 1, 0, Loc{ { source, 0 } }, Loc{ { sink, 0 } });
}

int Vehdet::get_vehicleNo() const
{
	return vehicleNo;
}

int Vehdet::get_sink() const
{
	return sink;
}

int Vehdet::get_source() const
{
	return source;
}

int Vehdet::get_capacity() const
{
	return capacity;
}

double Vehdet::get_earliest() const
{
	return earliest;
}

double Vehdet::get_latest() const
{
	return latest;
}