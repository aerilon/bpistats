#include <map>
#include <string>
#include <iostream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

class Main
{
public:
	Main(int, const char**);
	void operator()();

private:
	void parse_options(int, const char** argv);

	static void check_range_options(const std::vector<std::string>&);
	static void check_file_options(const std::vector<std::string>&);

	boost::program_options::options_description desc;
	boost::program_options::variables_map vm;
};

Main::Main(int argc, const char** argv) :
	desc("Allowed options")
{
	this->parse_options(argc, argv);
}

void
Main::check_range_options(const std::vector<std::string>& ranges)
{
	std::cout << ranges.size() << std::endl;
}

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

void
Main::check_file_options(const std::vector<std::string>& files)
{

	for (auto& file : files)
	{
		if (!std::filesystem::exists(file) && !std::filesystem::is_regular_file(file))
		{
			throw boost::program_options::validation_error(
			      boost::program_options::validation_error::invalid_option_value,
			      "--file", file);
		}
	}
}

void
Main::parse_options(int argc, const char** argv)
{
	this->desc.add_options()
		("help", "this help message")
		("range",
			boost::program_options::value<std::vector<std::string>>()->notifier(&Main::check_range_options),
			"date range to check. Multiple ranges can be given.\nformat expected: YYYY-MM-DD,YYYY-MM-DD")
		("file",
			boost::program_options::value<std::vector<std::string>>()->notifier(&Main::check_file_options),
			"JSON file to parse, must match coindesk historical close API. Multiple file can be given.");

	auto parsed = boost::program_options::command_line_parser(argc, argv)
		.options(this->desc)
		.run();

	boost::program_options::store(parsed, this->vm);

	boost::program_options::notify(this->vm);
}

void
Main::operator()()
{
	if (this->vm.count("help"))
	{
		std::cout << this->desc << std::endl;
		return;
	}

	if (this->vm.count("file"))
	{
		for (auto& file : this->vm["file"].as<std::vector<std::string>>())
		{
			(void)file;
		}
	}

	if (this->vm.count("range"))
	{
		for (auto& range : this->vm["range"].as<std::vector<std::string>>())
		{
			(void)range;
		}
	}
}

int
main(int argc, const char** argv)
{
	try
	{
		Main(argc, argv)();
	}
	catch (std::exception &e)
	{
		std::cout << "uncaught exception: " << e.what() << std::endl;
	}

	return 0;
}
