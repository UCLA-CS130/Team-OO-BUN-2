#include "request_handler_proxy.h"


namespace http {
namespace server {

	RequestHandler::Status ProxyHandler::Init(const std::string& uri_prefix, const NginxConfig& config) {
		for (auto it = config.statements_.begin(); it != config.statements_.end(); it++) {

			// if current statement has two tokens
			if ((*it)->tokens_.size() == 2) {
				if ((*it)->tokens_[0] == "port") {
					port = (*it)->tokens_[1];
				} else if ((*it)->tokens_[0] == "host") {
					host = (*it)->tokens_[1];
				} else if ((*it)->tokens_[0] == "path") {
					path = (*it)->tokens_[1];
				}
			}
		}

		return RequestHandler::Status::OK;
	}

	RequestHandler::Status ProxyHandler::HandleRequest(const Request& request, Response* response) {
	  	std::cout << "ProxyHandler::HandleRequest called" << std::endl;
	  	bool is_success = make_request(host, port, path, response);

	  	if (!is_success) {
	  		std::string fail_body = "Request to remote server failed";
	  		response->SetStatus(Response::ResponseCode::NOT_FOUND);
		  	response->AddHeader("Content-Type", "text/plain");
		  	response->AddHeader("Content-Length", std::to_string(fail_body.length()));
		  	response->SetBody(fail_body);
	  	}

		return RequestHandler::Status::OK;
	}

	bool ProxyHandler::make_request(std::string host, std::string port, std::string path, Response* res) {
		std::cout << "Making request to: www." << host << ":" << port << path << std::endl;

		using boost::asio::ip::tcp;
		boost::asio::io_service io_service;

	    // Get a list of endpoints corresponding to the server name.
	    tcp::resolver resolver(io_service);
	    tcp::resolver::query query(host, "http");
	    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

	    // Try each endpoint until we successfully establish a connection.
	    tcp::socket socket(io_service);
	    boost::asio::connect(socket, endpoint_iterator);

	    // Form the request. We specify the "Connection: close" header so that the
	    // server will close the socket after transmitting the response. This will
	    // allow us to treat all data up until the EOF as the content.
	    boost::asio::streambuf request;
	    std::ostream request_stream(&request);
	    request_stream << "GET " << path << " HTTP/1.0\r\n";
	    request_stream << "Host: www." << host << ":" << port << "\r\n";
	    request_stream << "Accept: */*\r\n";
	    request_stream << "Connection: close\r\n\r\n";

	    // For debugging: print out the request
	    boost::asio::streambuf::const_buffers_type bufs = request.data();
		std::string str(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + request.size());
		std::cout << str << std::endl;

	    // Send the request.
	    boost::asio::write(socket, request);

	    // Read the response status line. The response streambuf will automatically
	    // grow to accommodate the entire line. The growth may be limited by passing
	    // a maximum size to the streambuf constructor.
	    boost::asio::streambuf response;
	    boost::asio::read_until(socket, response, "\r\n");

	    // Check that response is OK.
	    std::istream response_stream(&response);

	    std::string http_version;
	    response_stream >> http_version;

	    unsigned int status_code;
	    response_stream >> status_code;

	    std::string status_message;
	    std::getline(response_stream, status_message);
	    if (!response_stream || http_version.substr(0, 5) != "HTTP/")
	    {
	      std::cout << "Invalid response\n";
	      return false;
	    }

	    // Handle redirect
	    if (status_code == 302) {
	    	// TODO:
	    	// return make_request(host, port, path, res)
	    	// where host, port, and path are set from the redirect
	    	// Note: there is a response parser obj as member variable of ProxyHandler to get location header
	    	std::cout << "TODO Redirect:" << std::endl;
	    }
	    if (status_code != 200)
	    {
	      std::cout << "Response returned with status code " << status_code << "\n";
	      return false;
	    }

	    // Read the response headers, which are terminated by a blank line.
	    boost::asio::read_until(socket, response, "\r\n\r\n");

	    // Process the response headers.
	    std::string header;
	    while (std::getline(response_stream, header) && header != "\r")
	      std::cout << header << "\n";
	    std::cout << "\n";

	    // Write whatever content we already have to output.
	    if (response.size() > 0)
	      std::cout << &response;

	    // Read until EOF, writing data to output as we go.
	    boost::system::error_code error;
	    while (boost::asio::read(socket, response,
	          boost::asio::transfer_at_least(1), error))
	      std::cout << &response;
	    if (error != boost::asio::error::eof)
	      throw boost::system::system_error(error);

		return true;
	}

} // namespace server
} // namespace http
