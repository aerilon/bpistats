#pragma once

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

	boost::program_options::options_description desc;
	boost::program_options::variables_map vm;

	bpi::date_parser parser;

	std::vector<bpi::records::file> file_records;
	std::vector<bpi::records::online> online_records;
};

}
