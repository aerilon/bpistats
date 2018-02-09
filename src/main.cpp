#include <map>
#include <string>
#include <iostream>

#if __has_include("experimental/filesystem")
#include <experimental/filesystem>
namespace std
{
	namespace filesystem = experimental::filesystem;
}
#elif __has_include("filesystem")
#include <filesystem>
#elif __has_include("boost/filesystem.hpp")
#include <boost/filesystem.hpp>
namespace std
{
	namespace filesystem = ::boost::filesystem;
}
#endif

#include <boost/asio/io_service.hpp>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <main.hpp>

namespace
{
	const std::string COINDESK_HISTORICAL_CLOSE_JSON_API("https://api.coindesk.com/v1/bpi/historical/close.json");
	const std::string COINDESK_FIRST_RECORD("2010-07-17");
}

namespace bpi
{

main::main(int argc, const char** argv) :
	desc("Allowed options"),
	parser("%Y-%m-%d")
{
	this->parse_options(argc, argv);
}

void
main::range_option_notifier(const std::vector<std::string>& ranges)
{

	[[maybe_unused]] auto [ok, coindesk_fist_record] = this->parser(COINDESK_FIRST_RECORD);
	(void)ok; // XXX al - previous attributes only works in gcc >7.2

	auto log_and_throw = [](const std::string& message, const std::string& range)
	{
		std::stringstream ss;
		ss << message << ": " << range;
		std::cerr << ss.str() << std::endl;
		throw boost::program_options::validation_error(
		      boost::program_options::validation_error::invalid_option_value,
		      "--range", ss.str());
	};

	for (auto& range : ranges)
	{
		auto comma = range.find(',');
		if (comma == std::string::npos)
		{
			log_and_throw("Missing comma separator", range);
		}

		auto [from_ok, from] = this->parser(range.substr(0, comma));
		auto [to_ok, to] = this->parser(range.substr(comma + 1, std::string::npos));
		if (!from_ok || !to_ok)
		{
			log_and_throw("Error raised during date conversion", range);
		}

		if (to < from)
		{
			log_and_throw("Range ends before it begins", range);
		}

		if (from < coindesk_fist_record)
		{
			log_and_throw("First record asked predate Coindesk first record", range);
		}

		this->online_records.emplace_back(COINDESK_HISTORICAL_CLOSE_JSON_API,
		    from, to);
	}
}

void
main::file_option_notifier(const std::vector<std::string>& files)
{

	for (auto& file : files)
	{
		if (!std::filesystem::exists(file) && !std::filesystem::is_regular_file(file))
		{
			throw boost::program_options::validation_error(
			      boost::program_options::validation_error::invalid_option_value,
			      "--file", file);
		}

		boost::property_tree::ptree root;

		boost::property_tree::read_json(file, root);

		auto& collection = this->file_records.emplace_back(file);
		auto& map = collection.get();

		auto data = root.get_child("bpi");

		for(const auto& node : data)
		{
			auto [ok, pt] = this->parser(node.first);
			if (!ok)
			{
				// FIXME
				throw std::exception();
			}

			[[maybe_unused]] auto [elem, inserted] = map.emplace(pt, node.second.get_value<double>());
			(void)elem;
			if (!inserted)
			{
				// FIXME
				throw std::exception();
			}
		}
	}
}

void
main::parse_options(int argc, const char** argv)
{
	this->desc.add_options()
		("help", "this help message")
		("range",
			boost::program_options::value<std::vector<std::string>>()->notifier([this](const auto& arg){ this->range_option_notifier(arg); }),
			"date range to check. Multiple ranges can be given.\nformat expected: YYYY-MM-DD,YYYY-MM-DD")
		("file",
			boost::program_options::value<std::vector<std::string>>()->notifier([this](const auto& arg){ this->file_option_notifier(arg); }),
			"JSON file to parse, must match coindesk historical close API. Multiple file can be given.");

	auto parsed = boost::program_options::command_line_parser(argc, argv)
		.options(this->desc)
		.run();

	boost::program_options::store(parsed, this->vm);

	boost::program_options::notify(this->vm);
}

void
main::operator()()
{
	if (this->vm.count("help"))
	{
		std::cout << this->desc << std::endl;
		return;
	}

	boost::asio::io_service io_service;

	for (auto& records : this->file_records)
	{
		//io_service.post()
	}

	if (this->vm.count("range"))
	{
		for (auto& range : this->vm["range"].as<std::vector<std::string>>())
		{
			(void)range;
		}
	}

	io_service.run();
}

}

int
main(int argc, const char** argv)
{
	try
	{
		bpi::main(argc, argv)();
	}
	catch (std::exception &e)
	{
		std::cout << "uncaught exception: " << e.what() << std::endl;
	}

	return 0;
}
