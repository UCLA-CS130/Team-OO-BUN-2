#include "request_handler_proxy.h"
#include <sstream>

namespace http {
namespace server {

	RequestHandler::Status ProxyHandler::Init(const std::string& uri_prefix, const NginxConfig& config) {
		port = "80"; //add default port
		for (auto it = config.statements_.begin(); it != config.statements_.end(); it++) {
			// if current statement has two tokens
			if ((*it)->tokens_.size() == 2) {
				if ((*it)->tokens_[0] == "port") {
					port = (*it)->tokens_[1];
				} else if ((*it)->tokens_[0] == "host") {
					host = (*it)->tokens_[1];
				}
			}
		}
		this->uri_prefix = uri_prefix;
		return RequestHandler::Status::OK;
	}

	RequestHandler::Status ProxyHandler::HandleRequest(const Request& request, Response* response) {
	  	std::cout << "ProxyHandler::HandleRequest called" << std::endl;
	  	//get path that the user is requesting from the remote server
	  	path = request.uri().substr(uri_prefix.length());
	  	if (path.length() == 0)
	  		path = "/";
	  	std::cout << path << std::endl;
	  	bool is_success = make_request(host, port, path, false, response);

	  	if (!is_success) {
	  		std::string fail_body = "Request to remote server failed";
	  		response->SetStatus(Response::ResponseCode::NOT_FOUND);
		  	response->AddHeader("Content-Type", "text/plain");
		  	response->AddHeader("Content-Length", std::to_string(fail_body.length()));
		  	response->SetBody(fail_body);
	  	}

		return RequestHandler::Status::OK;
	}

	bool ProxyHandler::make_request(std::string host, std::string port, std::string path, bool redirect, Response* res) {
		std::cout << "Making request to: www." << host << ":" << port << path << std::endl;

		using boost::asio::ip::tcp;
		boost::asio::io_service io_service;


	    // Get a list of endpoints corresponding to the server name.
	    tcp::resolver resolver(io_service);
	    tcp::resolver::query query(host, port);
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
	    if (!redirect)
	    	request_stream << "Host: www." << host << ":" << port << "\r\n";
	    else
	    	request_stream << "Host: " << host << "\r\n";
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
	    boost::asio::streambuf response_buffer;
	    boost::asio::read_until(socket, response_buffer, "\r\n");

	    // Check that response is OK.
	    std::istream response_stream(&response_buffer);

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
	    	// return make_request(host, port, path, res)
	    	// where host, port, and path are set from the redirect
	    	// Note: there is a response parser obj as member variable of ProxyHandler to get location header
	    	std::cout << "REDIRECT" << std::endl;
	    	boost::asio::read_until(socket, response_buffer, "\r\n\r\n");
	    	std::string response_string( (std::istreambuf_iterator<char>(&response_buffer)), std::istreambuf_iterator<char>() );
	    	resp_parser.parse_response(response_string);
	    	std::string new_loc = resp_parser.get_redirect_url();
	    	parse_url(new_loc, host, port, path);
	    	return make_request(host, port, path, true, res);
	    }
	    else if (status_code != 200)
	    {
	      std::cout << "Response returned with status code " << status_code << "\n";
	      return false;
	    }

	    // Read the response headers, which are terminated by a blank line.
	    boost::asio::read_until(socket, response_buffer, "\r\n\r\n");
	    std::string response_string( (std::istreambuf_iterator<char>(&response_buffer)), std::istreambuf_iterator<char>() );

	    //get the content type to set the correct field in the response
	    resp_parser.parse_response(response_string);
	    std::string content_type = resp_parser.get_content_type();
	    if (content_type == "")
	    	content_type = "text/plain";

	    // Read until EOF, writing data to output as we go.
	    boost::system::error_code error;
	    // temp storage of body
	    std::stringstream str_stream;
	    while (boost::asio::read(socket, response_buffer,boost::asio::transfer_at_least(1), error)) {
	      str_stream << &response_buffer;
	    }
	    //create a total response of the headers and message body, and isolate the message body
	    std::string total_response = response_string + str_stream.str();
	    std::string response_body = total_response.substr(total_response.find("\r\n\r\n")+4);
	    // set up proxy response
	    res->SetStatus(Response::ResponseCode::OK);
	    res->AddHeader("Content-Type", content_type);
	  	res->AddHeader("Content-Length", std::to_string(response_body.length()));
	    res->SetBody(response_body);

	    if (error != boost::asio::error::eof) {
	      throw boost::system::system_error(error);
	    }

		return true;
	}

	//parse the redirect url so that it can be used for the next request
	void ProxyHandler::parse_url(std::string url, std::string& host, std::string& port, std::string& path)
	{
		if (url.length() == 0)
			return;
		
		if (url.find("https") != std::string::npos){
			port = "443";
		}
		else if(url.find("http") != std::string::npos){
			port = "80";
		}
		else{
			//in the case that the redirect returns a relative url, instead of an absolute one
			path = url;
			return;
		}
		//turn http://www.google.com/blah blah blah -> www.google.com
		host = url.substr(url.find("//")+2);
		if (host.find("/") != std::string::npos){
			path = host.substr(host.find("/"));
			host = host.substr(0, host.find("/"));
		}
		else{
			//no path specified so redirected to just google.com which would be google.com/ for example
			path = "/";
		}

		
	}

} // namespace server
} // namespace http
