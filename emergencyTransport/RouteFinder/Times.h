//class to wrap map of travel times between two points where the times are equal in either direction
#ifndef TIMES_H
#define TIMES_H

#include <map>
#include <unordered_map>
#include <list>
#include <algorithm>
using std::map;
using std::unordered_map;
using std::less;
using std::list;
using std::swap;

struct timekey
{
	timekey()
		: from(), to()
	{
	}

	timekey(const int& f, const int& t)
		: from(f), to(t)
	{
	}

	void arrange()
	{	if (from > to)
		{
		swap(to, from);
		}
	}

	bool operator<(const timekey& k) const
	{
		if (this->from == k.from)
		{
			return this->to < k.to;
		}
		return this->from < k.from;
	}

	int from;
	int to;
};

class timekey_hash
{
public:
	long operator()(const timekey &x) const
	{
		return x.from * 1000 + x.to;
	}
};

class timekey_equal_to
{
public:
	bool operator()(const timekey &x, const timekey &y) const
	{
		return x.from == y.from && x.to == y.to;
	}
};

typedef unordered_map < timekey, double, timekey_hash, timekey_equal_to > Timemap;

class Times
{
public:
	Times();
	Times(const Timemap&);

	void insert_time(const int&, const int&, const double&);

	double get_time(const int&, const int&) const;
private:
	Timemap times;
};


#endif