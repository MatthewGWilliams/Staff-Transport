#include "Requestsetup.h"
#include <vector>

using std::vector;

Requestsetup::Requestsetup()
	: inOrOut(true), absEarl(0), absLate(0), priority(0), idealTime(0)
{

}


Requestsetup::Requestsetup(bool inOut, double Earl, double Late, int pri, double ideal, const Loc& pick, const Loc& drop)
	:inOrOut(inOut), absEarl(Earl), absLate(Late), priority(pri), idealTime(ideal), pickup(pick), dropoff(drop)
{

}

bool Requestsetup::get_inOrOut() const
{
	return inOrOut;
}

double Requestsetup::get_absEarl() const
{
	return absEarl;
}

double Requestsetup::get_absLate() const
{
	return absLate;
}

int Requestsetup::get_priority() const
{
	return priority;
}

double Requestsetup::get_idealTime() const
{
	return idealTime;
}

const Loc& Requestsetup::get_pickup() const
{
	return pickup;
}

const Loc& Requestsetup::get_dropoff() const
{
	return dropoff;
}