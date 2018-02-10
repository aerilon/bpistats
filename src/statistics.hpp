#pragma once

#include <boost/property_tree/ptree.hpp>

#include <records.hpp>
#include <date_printer.hpp>

namespace bpi::statistics
{

class engine
{
public:
	engine(const bpi::records::map_t& map);

	boost::property_tree::ptree run();

private:
	const bpi::records::map_t& map;
	bpi::date_printer printer;
};

}
