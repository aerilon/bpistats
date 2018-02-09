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

#include <boost/program_options/parsers.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <main.hpp>
#include <statistics.hpp>

namespace
{
	const std::string COINDESK_HISTORICAL_CLOSE_JSON_API("https://api.coindesk.com/v1/bpi/historical/close.json");
	const std::string COINDESK_FIRST_RECORD("2010-07-17");
}

/*
 * There is no elegant way in C++ to pass a streamstream as a method parameter, short of having the
 * method accept a reference and have a cumbersome temorary to manipulate, ie. have the following
 * syntax:
 *
 * fn("key: " << val << "key: << 1);
 *
 * thus we have to resort to a good old C macro.
 */
#define LOG_AND_THROW(msg, exception)			\
{							\
	std::stringstream ss;				\
	ss << msg;					\
	std::cerr << ss.str() << std::endl;		\
	throw (exception(ss.str()));			\
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

	// XXX slightly different than the above, mainly due to the different exception prototype
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
		// Integrity check.
		if (!std::filesystem::exists(file) && !std::filesystem::is_regular_file(file))
		{
			throw boost::program_options::validation_error(
			      boost::program_options::validation_error::invalid_option_value,
			      "--file", file);
		}

		// Load the JSON file, ...
		boost::property_tree::ptree root;
		boost::property_tree::read_json(file, root);

		// create the record, ...
		auto& record = this->file_records.emplace_back(file);

		// and populate our internal representation.
		this->populate_records(root, record.get());
	}
}

void
main::populate_records(const boost::property_tree::ptree& root,
    bpi::records::map_t& map)
{
	auto data = root.get_child("bpi");

	for (const auto& node : data)
	{
		auto [ok, pt] = this->parser(node.first);
		if (!ok)
		{
			LOG_AND_THROW("Invalid date argument:" << node.first, std::invalid_argument);
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

void
main::parse_options(int argc, const char** argv)
{
	this->desc.add_options()
		("help", "this help message")
		("range",
			boost::program_options::value<std::vector<std::string>>()->multitoken()->notifier([this](const auto& arg){ this->range_option_notifier(arg); }),
			"date range to check. Multiple ranges can be given.\nformat expected: YYYY-MM-DD,YYYY-MM-DD")
		("file",
			boost::program_options::value<std::vector<std::string>>()->multitoken()->notifier([this](const auto& arg){ this->file_option_notifier(arg); }),
			"JSON file to parse, must match coindesk historical close API. Multiple file can be given.");

	auto parsed = boost::program_options::command_line_parser(argc, argv)
		.options(this->desc)
		.run();

	boost::program_options::store(parsed, this->vm);

	boost::program_options::notify(this->vm);
}

void
main::print(const boost::property_tree::ptree& tree)
{
	std::stringstream ss;
	boost::property_tree::write_json(ss, tree);

	std::lock_guard<std::mutex> lk(this->print_lock);
	std::cout << ss.str() << std::endl;
}

void
main::operator()()
{
	if (this->vm.count("help"))
	{
		std::cout << this->desc << std::endl;
		return;
	}

	for (auto& records : this->file_records)
	{
		auto worker = [&]()
			{
			bpi::statistics::engine engine(records.get());

			auto statistics = engine.run();

			auto results = records.describe();

			results.put_child("statistics", statistics);

			this->print(results);
			};

		this->io_service.post(worker);
	}

	if (this->vm.count("range"))
	{
		for (auto& range : this->vm["range"].as<std::vector<std::string>>())
		{
			(void)range;
		}
	}

	this->io_service.run();
}

}
