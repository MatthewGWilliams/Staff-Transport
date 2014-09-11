//class to hold details of a single request
#ifndef REQUESTSETUP_H
#define REQUESTSETUP_H

#include <vector>
using std::vector;

struct LocElement
{
	LocElement()
		: locationNo(0), walking(0)
	{
	}

	LocElement(int locNo, int walk)
		: locationNo(locNo), walking(walk)
	{
	}

	int locationNo;
	int walking;
};

typedef vector< LocElement> Loc;

class Requestsetup
{
public:
	Requestsetup();
	Requestsetup(bool, double, double, int, double, const Loc&, const Loc&);
	
	bool get_inOrOut()const;
	double get_absEarl() const;
	double get_absLate() const;
	int get_priority() const;
	double get_idealTime() const;
	const Loc& get_pickup() const;
	const Loc& get_dropoff() const;

private:
	bool inOrOut; //if in then true.
	double absEarl;
	double absLate;
	int priority;
	double idealTime;
	Loc pickup; //1st int represents location, 2nd represnets walking time for that location.
	Loc dropoff;
};

#endif