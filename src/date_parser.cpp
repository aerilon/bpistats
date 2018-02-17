#include <utility>
#include <regex>

#include <date_parser.hpp>

namespace bpi
{

date_parser::date_parser():
	date_parser("%Y-%m-%d")
{
}

date_parser::date_parser(const std::string& format)
{
	// XXX al - pointer freed by std::~locale()
	this->ss.imbue(std::locale(std::locale(),
	    new boost::posix_time::time_input_facet(format.c_str())));
}

std::pair<bool, boost::posix_time::ptime>
date_parser::operator()(const std::string& text)
{
	boost::posix_time::ptime pt;

	std::regex regex("^([0-9]{4})-[0-9]{2}-[0-9]{2}$");
	std::smatch match;

	if (!std::regex_match(text, match, regex))
	{
		return { false, pt };
	}

	this->ss.clear();
	this->ss.str("");
	this->ss << text;

	ss >> pt;

	return { !pt.is_not_a_date_time(), pt };
}

}
