#pragma once

#include <boost/property_tree/ptree.hpp>

#include <records.hpp>

namespace bpi::statistics
{

class engine
{
public:
	engine(const bpi::records::map_t& map);

	boost::property_tree::ptree run();

private:
	const bpi::records::map_t& map;

	long double lowest;
	long double highest;
	long double median;
	long double mean;
	long double stddev;
};

}
