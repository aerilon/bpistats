#include <utility>

#include <date_printer.hpp>

namespace bpi
{

date_printer::date_printer(const std::string& format)
{
	// XXX al - pointer freed by std::~locale()
	this->ss.imbue(std::locale(std::locale(),
	    new boost::posix_time::time_facet(format.c_str())));
}

std::string
date_printer::operator()(const boost::posix_time::ptime& pt) const
{
	this->ss.clear();
	this->ss.str("");

	this->ss << pt;

	return ss.str();
}

}

