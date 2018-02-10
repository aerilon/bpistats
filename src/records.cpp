#include <date_printer.hpp>

#include <records.hpp>

namespace
{
	bpi::date_printer printer("%Y-%m-%d");
}

namespace bpi::records
{

/*
 * Base map
 */
map::operator map_t&()
{
	return this->internal_map;
}

map::operator const map_t&() const
{
	return this->internal_map;
}

map::operator boost::property_tree::ptree() const
{
	return this->describe();
}

boost::property_tree::ptree
map::describe() const
{
	boost::property_tree::ptree pt;

	if (this->internal_map.size() != 0)
	{
		auto& from = *(this->internal_map.begin());
		auto& to = *(this->internal_map.rbegin());

		pt.put("from", printer(from.first));;
		pt.put("to", printer(to.first));
	}

	return pt;
}

/*
 * file
 */
file::file(const std::string& filename) :
	filename(filename)
{
}

boost::property_tree::ptree
file::describe() const
{
	auto pt = map::describe();

	pt.put("type", "file");
	pt.put("filename", filename);

	return pt;
}

/*
 * online
 */
online::online(const std::string& host,
    const std::string& target,
    const boost::posix_time::ptime& from,
    const boost::posix_time::ptime& to) :
	host(host),
	target(target),
	from(from),
	to(to)
{
}

boost::property_tree::ptree
online::describe() const
{
	auto pt = map::describe();

	pt.put("type", "online");
	pt.put("host", this->host);
	pt.put("target", this->get_full_target());

	return pt;
}

std::string
online::get_host()
{
	return this->host;
}

std::string
online::get_full_target() const
{
	std::stringstream ss;

	ss.imbue(std::locale(std::locale::classic(),
	    new boost::posix_time::time_facet("%Y-%m-%d")));

	ss << this->target;
	ss << "?";
	ss << "start=";
	ss << this->from;
	ss << "&";
	ss << "end=";
	ss << this->to;

	return ss.str();
}

}

