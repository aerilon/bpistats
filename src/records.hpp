#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/property_tree/ptree.hpp>

namespace bpi::records
{

using map_t = std::map<boost::posix_time::ptime, double>;

class map
{
public:
	map() = default;
	virtual ~map() = default;

	operator map_t&();
	operator const map_t&() const;
	operator boost::property_tree::ptree() const;

	virtual boost::property_tree::ptree describe() const;

private:
	map_t internal_map;
};

class file : public map
{
public:
	file(const std::string& filename);

	boost::property_tree::ptree describe() const;
private:
	std::string filename;
};

class online : public map
{
public:
	online(const std::string& host,
	    const std::string& target,
	    const boost::posix_time::ptime& from,
	    const boost::posix_time::ptime& to);

	boost::property_tree::ptree describe() const;

	std::string get_host();
	std::string get_full_target() const;

private:
	const std::string host;
	const std::string target;
	const boost::posix_time::ptime from;
	const boost::posix_time::ptime to;
};

}

