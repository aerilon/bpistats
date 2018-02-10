#include <boost/test/unit_test.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <statistics.hpp>
#include <date_parser.hpp>

/*
 * XXX al -
 *
 * Unfortunately, Boost.Test does not have a direct support for parametrized test
 * fixture, as does GoogleTests. This would have allowed us to
 * write a generic set of test for the possible multiple implementation of the
 * statistics engine.
 */
struct fixture {
	fixture() :
		records(this->generic_map)
	{
		this->records.emplace(this->parser("2018-01-01").second, 1.0);
		this->records.emplace(this->parser("2018-01-02").second, 2.0);
		this->records.emplace(this->parser("2018-01-03").second, 3.0);
		this->records.emplace(this->parser("2018-01-04").second, 4.0);
		this->records.emplace(this->parser("2018-01-05").second, 5.0);
	}

	bpi::date_parser parser;

	bpi::records::map generic_map;
	bpi::records::map_t& records;

};

BOOST_FIXTURE_TEST_SUITE( statistics, fixture )

BOOST_AUTO_TEST_CASE( statistics )
{
#if 0
{
    "lowest": {
        "price": "1",
        "date": "2018-01-01"
    },
    "highest": {
        "price": "5",
        "date": "2018-01-05"
    },
    "stddev": "1.58113883008418966598",
    "average": "3",
    "median": "3",
    "sample_size": "5"
}
#endif

	bpi::statistics::engine engine(this->records);

	auto results = engine.run();

	BOOST_CHECK(results.get<int>("sample_size") == 5);

	BOOST_CHECK_CLOSE(results.get<double>("stddev"), 1.581, 0.01);
	BOOST_CHECK_CLOSE(results.get<double>("average"), 3.0, 0.01);
	BOOST_CHECK_CLOSE(results.get<double>("median"), 3.0, 0.01);

	auto lowest = results.get_child("lowest");
	BOOST_CHECK_CLOSE(lowest.get<double>("price"), 1.0, 0.01);
	BOOST_CHECK(lowest.get<std::string>("date") == "2018-01-01");

	auto highest = results.get_child("highest");
	BOOST_CHECK_CLOSE(highest.get<double>("price"), 5.0, 0.01);
	BOOST_CHECK(highest.get<std::string>("date") == "2018-01-05");
}

BOOST_AUTO_TEST_CASE( median )
{
	// Even number of records
	this->records.emplace(this->parser("2018-01-06").second, 6.0);

	bpi::statistics::engine engine(this->records);

	auto results = engine.run();

	BOOST_CHECK_CLOSE(results.get<double>("median"), 3.5, 0.01);
}

BOOST_AUTO_TEST_SUITE_END()
