#pragma once

#include <boost/asio/ssl/context.hpp>

namespace bpi::platform
{

namespace ssl
{

boost::asio::ssl::context get_default_context();

}

}
