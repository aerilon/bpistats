#pragma once

#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace bpi
{

class date_parser
{
public:
	date_parser(const std::string& format)
	{
		// XXX al - pointer freed by std::~locale()
		this->ss.imbue(std::locale(std::locale(),
		    new boost::posix_time::time_input_facet(format.c_str())));
	}

	std::pair<bool, boost::posix_time::ptime>
	operator()(const std::string& text)
	{
		boost::posix_time::ptime pt;

		this->ss.clear();
		this->ss << text;

		ss >> pt;

		return std::make_pair(!pt.is_not_a_date_time(), pt);
	}

private:
	std::stringstream ss;
};

}
