#include <boost/test/unit_test.hpp>

#include <date_parser.hpp>

BOOST_AUTO_TEST_CASE( date_parser )
{
	bpi::date_parser parser;

	{
		// Valid
		auto [ok, pt] = parser("2018-01-31");
		BOOST_TEST(ok);

		std::stringstream ss;
		ss << pt;

		BOOST_TEST(ss.str() == "2018-Jan-31 00:00:00");
	}

	{
		// Valid
		auto [ok, pt] = parser("1900-01-01");
		BOOST_TEST(ok);

		std::stringstream ss;
		ss << pt;

		BOOST_TEST(ss.str() == "1900-Jan-01 00:00:00");
	}

	{
		// "Feb 31st" is invalid
		auto [ok, pt] = parser("2018-02-31");
		(void)pt;
		BOOST_TEST(!ok);
	}

	{
		// 2018 is not a leap year
		auto [ok, pt] = parser("2018-02-29");
		(void)pt;
		BOOST_TEST(!ok);
	}

	{
		// 2020 is a leap year
		auto [ok, pt] = parser("2020-02-29");
		(void)pt;
		BOOST_TEST(ok);
	}
}
