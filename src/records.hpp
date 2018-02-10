#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/property_tree/ptree.hpp>

#include <date_printer.hpp>

namespace bpi::records
{

using map_t = std::map<boost::posix_time::ptime, double>;

class map
{
public:
	map() :
		printer("%Y-%m-%d")
	{
	}

	operator map_t&()
	{
		return this->internal_map;
	}

	operator const map_t&() const
	{
		return this->internal_map;
	}

	operator boost::property_tree::ptree() const
	{
		return this->describe();
	}

	boost::property_tree::ptree
	describe() const
	{
		boost::property_tree::ptree pt;

		auto& from = *(this->internal_map.begin());
		auto& to = *(this->internal_map.rbegin());

		pt.put("from", this->printer(from.first));;
		pt.put("to", this->printer(to.first));

		return pt;
	};
private:
	map_t internal_map;
	bpi::date_printer printer;
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
	describe() const
	{
		auto pt = map::describe();

		pt.put("type", "online");
		pt.put("host", this->host);
		pt.put("target", this->target);

		return pt;
	}

	std::string
	get_host()
	{
		return this->host;
	}

	std::string
	get_target()
	{
		return this->target;
	}

	std::pair<boost::posix_time::ptime, boost::posix_time::ptime>
	get_range()
	{
		return std::make_pair(this->from, this->to);
	}

private:
	const std::string host;
	const std::string target;
	const boost::posix_time::ptime from;
	const boost::posix_time::ptime to;
};

}

