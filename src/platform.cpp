#include <platform.hpp>

namespace bpi::platform
{

namespace ssl
{

boost::asio::ssl::context
get_default_context()
{
	boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12);

	ctx.set_default_verify_paths();

	return ctx;
}

}

}

