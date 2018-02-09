#pragma once

#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include <boost/asio/io_service.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

namespace bpi::network
{

using handler_t = std::function<void(boost::system::error_code, const std::string& message)>;

namespace ssl
{

inline boost::asio::ssl::context
get_context()
{
	boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12);

	ctx.set_default_verify_paths();

	return std::move(ctx);
}

}

class session : public std::enable_shared_from_this<session>
{
public:
	session(boost::asio::io_service&, boost::asio::ssl::context&, handler_t handler);

	// Start the asynchronous operation
	void run(const std::string& host, const std::string& port, const std::string& target);

private:
	void on_resolve(boost::system::error_code ec, boost::asio::ip::tcp::resolver::results_type results);
	void on_connect(boost::system::error_code ec);
	void on_handshake(boost::system::error_code ec);
	void on_write(boost::system::error_code ec, std::size_t bytes_transferred);
	void on_read(boost::system::error_code ec, std::size_t bytes_transferred);
	void on_shutdown(boost::system::error_code ec);

	handler_t handler;
	std::string host;

	boost::asio::ip::tcp::resolver resolver;
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream;
	boost::beast::flat_buffer buffer; // (Must persist between reads)
	boost::beast::http::request<boost::beast::http::empty_body> request;
	boost::beast::http::response<boost::beast::http::string_body> response;
};

}
