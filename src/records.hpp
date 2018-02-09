#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/property_tree/ptree.hpp>

namespace bpi::records
{

using map_t = std::map<boost::posix_time::ptime, double>;

class map
{
public:
	map_t&
	get()
	{
		return this->map;
	}

	boost::property_tree::ptree
	describe() const
	{
		boost::property_tree::ptree pt;

		auto& from = *(this->map.begin());
		auto& to = *(this->map.rbegin());

		pt.put("from", from.first);
		pt.put("to", to.first);

		return pt;
	};
private:
	map_t map;
};

class file : public map
{
public:
	file(const std::string& filename) :
		filename(filename)
	{
	}

	boost::property_tree::ptree
	describe() const
	{
		auto pt = map::describe();

		pt.put("type", "file");
		pt.put("filename", filename);

		return pt;
	}

private:
	std::string filename;
};

class online : public map
{
public:
	online(const std::string& host,
	    const boost::posix_time::ptime& from,
	    const boost::posix_time::ptime& to) :
		host(host),
		from(from),
		to(to)
	{
	}

	boost::property_tree::ptree
	describe() const
	{
		auto pt = map::describe();

		pt.put("type", "online");
		pt.put("host", host);

		return pt;
	}

private:
	const std::string host;
	const boost::posix_time::ptime from;
	const boost::posix_time::ptime to;
};

}

