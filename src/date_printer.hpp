#pragma once

#include <sstream>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace bpi
{

class date_printer
{
public:
	date_printer(const std::string&);

	std::string operator()(const boost::posix_time::ptime&) const;
private:
	mutable std::stringstream ss;
};

}
