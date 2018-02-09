#pragma once

#include <sstream>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace bpi
{

class date_parser
{
public:
	date_parser(const std::string&);

	std::pair<bool, boost::posix_time::ptime> operator()(const std::string&);

	void format(const std::string&);
private:
	std::stringstream ss;
};

}
