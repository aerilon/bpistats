#pragma once

#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#if __has_include("optional")
#include <optional>
//#elif __has_include("experimental/optional")
// XXX al -
//
// Do *not* include <experimental/optional> on purpose. Apple LLVM version 9.0.0
// (clang-900.0.39.2)) has a non-conforming version missing
// std::optional::reset().
#else
#include <boost/optional.hpp>
namespace std
{
	template<typename T>
	using optional = ::boost::optional<T>;
};
#endif

#include <boost/asio/io_service.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/http/parser.hpp>

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

	return ctx;
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
	size_t on_chunk_body(uint64_t, boost::string_view body, boost::system::error_code& ec);
	void on_read(boost::system::error_code ec, std::size_t bytes_transferred);
	void on_shutdown(boost::system::error_code ec);

	handler_t handler;
	std::string host;

	boost::asio::ip::tcp::resolver resolver;
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream;
	boost::beast::flat_buffer buffer; // (Must persist between reads)
	boost::beast::http::request<boost::beast::http::empty_body> request;

	std::optional<boost::beast::http::response_parser<boost::beast::http::dynamic_body>> response_parser;
	std::stringstream ss; // the chunk buffer

	// XXX al -
	// boost::beast::http::response_parser::on_chunk_cb takes a *reference*
	// to a callback, so we have to manage its lifetime manually. This is a
	// *major* pain :-/
	std::optional<std::function<size_t(uint64_t, boost::string_view, boost::system::error_code&)>> on_chunk_body_trampoline;
};

}
