#include "Times.h"

#include <map>
#include <list>
using std::map;
using std::less;
using std::list;

Times::Times()
{}

Times::Times(const Timemap& timemap)
{
	times = timemap;
}

void Times::insert_time(const int& a, const int& b, const double& time)
{
	timekey key(a, b);
	key.arrange();
	times[key] = time;
}

double Times::get_time(const int& a, const int& b) const
{
	if (a == b)
		return 0;
	else
	{
		timekey key(a, b);
		key.arrange();
		return times.at(key);
	}
}