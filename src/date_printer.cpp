#include <utility>

#include <date_printer.hpp>

namespace bpi
{

date_printer::date_printer(const std::string& format) :
	locale(std::locale(), new boost::posix_time::time_facet(format.c_str()))
{
}

std::string
date_printer::operator()(const boost::posix_time::ptime& pt) const
{
	std::stringstream ss;

	ss.imbue(this->locale);

	ss << pt;

	return ss.str();
}

}

