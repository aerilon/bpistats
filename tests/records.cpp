#include <boost/test/unit_test.hpp>

#include <date_parser.hpp>
#include <records.hpp>

namespace
{
	struct fixture
	{
		bpi::date_parser parser;
	};
}

BOOST_FIXTURE_TEST_SUITE( records, fixture )

BOOST_AUTO_TEST_CASE( base_map )
{
	bpi::records::map records;

	bpi::records::map_t& map = records;

	map.emplace(this->parser("2018-01-01").second, 42.0);
	map.emplace(this->parser("2018-01-02").second, 42.0);

	boost::property_tree::ptree description = records;

	BOOST_CHECK(description.get<std::string>("from") == "2018-01-01");
	BOOST_CHECK(description.get<std::string>("to") == "2018-01-02");
}

BOOST_AUTO_TEST_CASE( file_records )
{
	const std::string filename("filename");
	bpi::records::file file_records(filename);

	boost::property_tree::ptree description = file_records;

	BOOST_CHECK(description.get<std::string>("type") == "file");
	BOOST_CHECK(description.get<std::string>("filename") == filename);
}

BOOST_AUTO_TEST_CASE( online_records )
{
	const std::string host("HOST");
	const std::string target("TARGET");

	auto from = this->parser("2018-01-01").second;
	auto to = this->parser("2018-01-02").second;

	bpi::records::online online_records(host, target, from, to);

	boost::property_tree::ptree description = online_records;

	BOOST_CHECK(description.get<std::string>("type") == "online");
	BOOST_CHECK(description.get<std::string>("host") == host);
	BOOST_CHECK(description.get<std::string>("target") == "TARGET?start=2018-01-01&end=2018-01-02");
}

BOOST_AUTO_TEST_SUITE_END()
