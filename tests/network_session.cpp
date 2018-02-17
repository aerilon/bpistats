#include <boost/test/unit_test.hpp>

#include <network_session.hpp>
#include <platform.hpp>

BOOST_AUTO_TEST_CASE( network_session )
{
	// XXX al - Load from file ...
	const std::string EXPECTED("{\"bpi\":{\"2018-01-01\":13412.44,\"2018-01-02\":14740.7563},\"disclaimer\":\"This data was produced from the CoinDesk Bitcoin Price Index. BPI value data returned as USD.\",\"time\":{\"updated\":\"Jan 3, 2018 00:03:00 UTC\",\"updatedISO\":\"2018-01-03T00:03:00+00:00\"}}");

	boost::asio::io_service io_service;

	auto ctx = bpi::platform::ssl::get_default_context();

	auto session = std::make_shared<bpi::network::session>(io_service, ctx,
		[&](auto ec, auto message)
		{
			if (ec)
			{
				std::cout << "ERROR: " << ec.message() << std::endl;
				return;
			}

			BOOST_CHECK(!ec);
			BOOST_CHECK(message == EXPECTED);
		});

	session->run("api.coindesk.com", "https",
	    "/v1/bpi/historical/close.json?start=2018-01-01&end=2018-01-02");

	io_service.run();
}
