#pragma once

#include <mutex>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ssl/context.hpp>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>

#include <date_parser.hpp>
#include <records.hpp>

namespace bpi
{

class main
{
public:
	main(int, const char**);
	void operator()();

private:
	void parse_options(int, const char** argv);

	void range_option_notifier(const std::vector<std::string>&);
	void file_option_notifier(const std::vector<std::string>&);

	template<typename T> boost::property_tree::ptree parse_json(T t);
	void populate_records(const boost::property_tree::ptree&, bpi::records::map_t&);

	void print(const boost::property_tree::ptree&);

	boost::asio::io_service io_service;
	boost::asio::ssl::context ssl_ctx;

	boost::program_options::options_description desc;
	boost::program_options::variables_map vm;

	bpi::date_parser parser;

	std::vector<bpi::records::file> file_records;
	std::vector<bpi::records::online> online_records;

	bool minify_output;

	std::mutex print_lock;
};

}
