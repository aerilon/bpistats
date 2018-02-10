//
// Copyright (c) 2016-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#include <boost/asio/ssl/rfc2818_verification.hpp>

#include <network_session.hpp>

namespace bpi::network
{

session::session(boost::asio::io_context& io_service, boost::asio::ssl::context& context, handler_t handler) :
	handler(handler),
	resolver(io_service),
	stream(io_service, context)
{
}

// Start the asynchronous operation
void
session::run(const std::string& host, const std::string& port, const std::string& target)
{
	// Set SNI Hostname (many hosts need this to handshake successfully)
	if (! SSL_set_tlsext_host_name(this->stream.native_handle(), host.c_str()))
	{
		boost::system::error_code ec{static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()};
		this->handler(ec, "");
		return;
	}

	this->host = host; // for SSL setup

	// Set up an HTTP GET request message
	this->request.version(11);
	this->request.method(boost::beast::http::verb::get);
	this->request.target(target);
	this->request.set(boost::beast::http::field::host, host);
	this->request.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

	// Look up the domain name
	this->resolver.async_resolve(host, port,
		[self = this->shared_from_this()](auto ec, auto results)
		{
			self->on_resolve(ec, results);
		});
}

void
session::on_resolve(boost::system::error_code ec, boost::asio::ip::tcp::resolver::results_type results)
{
	if (ec)
	{
		this->handler(ec, "");
		return;
	}

	// Make the connection on the IP address we get from a lookup
	boost::asio::async_connect(this->stream.next_layer(),
		results.begin(), results.end(),
		[self = this->shared_from_this()](auto error_code, auto)
		{
			self->on_connect(error_code);
		});
}

void
session::on_connect(boost::system::error_code ec)
{
	if (ec)
	{
		this->handler(ec, "");
		return;
	}

	// Setup peer verification method
	this->stream.set_verify_mode(boost::asio::ssl::verify_peer);
	this->stream.set_verify_callback(boost::asio::ssl::rfc2818_verification(this->host));

	// Perform the SSL handshake
	this->stream.async_handshake(
		boost::asio::ssl::stream_base::client,
		[self = this->shared_from_this()](auto error_code)
		{
			self->on_handshake(error_code);
		});
}

void
session::on_handshake(boost::system::error_code ec)
{
	if (ec)
	{
		this->handler(ec, "");
		return;
	}

	// Send the HTTP request to the remote host
	boost::beast::http::async_write(this->stream, this->request,
		[self = this->shared_from_this()](auto error_code, auto size_transfered)
		{
			self->on_write(error_code, size_transfered);
		});
}

void
session::on_write(boost::system::error_code ec, std::size_t)
{
	if (ec)
	{
		this->handler(ec, "");
		return;
	}

	this->response_parser.emplace();

	this->on_chunk_body_trampoline.emplace(
		[self = this->shared_from_this()](auto remain, auto body, auto ec)
		{
			return self->on_chunk_body(remain, body, ec);
		});

	this->response_parser->on_chunk_body(*this->on_chunk_body_trampoline);

	// Receive the HTTP response
	boost::beast::http::async_read(this->stream, this->buffer, *this->response_parser,
		[self = this->shared_from_this()](auto error_code, auto size_transfered)
		{
			self->on_read(error_code, size_transfered);
		});
}

size_t
session::on_chunk_body(uint64_t, boost::string_view body, boost::system::error_code& ec)
{
	if (ec)
	{
		this->handler(ec, "");
		return 0;
	}

	this->ss << body;

	return body.size();
}

void
session::on_read(boost::system::error_code ec, std::size_t)
{
	if (ec)
	{
		this->handler(ec, "");
		return;
	}

	this->response_parser.reset();
	this->on_chunk_body_trampoline.reset();

	this->handler(ec, this->ss.str());

	// Gracefully close the stream
	this->stream.async_shutdown(
		[self = this->shared_from_this()](auto error_code)
		{
			self->on_shutdown(error_code);
		});
}

void
session::on_shutdown(boost::system::error_code ec)
{
	if (ec == boost::asio::error::eof ||
	    ec == boost::asio::ssl::error::stream_truncated)
	{
		// Rationale:
		// http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
		ec.assign(0, ec.category());
	}

	if (ec)
	{
		this->handler(ec, "");
		return;
	}

	// Done !
}

}
