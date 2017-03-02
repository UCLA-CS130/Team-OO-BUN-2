#include "connection.h"
#include <utility>
#include <vector>
#include <iostream> 
#include "request_handler.h"
#include <boost/asio.hpp>
#include "server_monitor.h"

namespace http {
namespace server {

connection::connection(boost::asio::ip::tcp::socket socket, std::map <std::string, RequestHandler*> handlers)
  : socket_(std::move(socket)), handlers_(handlers)
{
}

void connection::start()
{
  do_read();
}

void connection::stop()
{
  socket_.close();
  io_service_.stop();
}

Response connection::get_response()
{
  return res;
}

void connection::do_read()
{
  auto self(shared_from_this());
  socket_.async_read_some(boost::asio::buffer(buffer_),
      [this, self](boost::system::error_code ec, std::size_t bytes_transferred)
      {
        if (!ec)
        {
          // Turn the request buffer into a string
          std::string s = "";
          s.append(buffer_.data(), buffer_.data() + bytes_transferred);

          // Create a request object of the parsed object
          call_parser(s);

          // Determine the file handler needed
          RequestHandler* server_mode = find_handler(req);

          // Call the request handler that matches the request
          call_handler(server_mode);
        }
      });
}

// Parse the input buffer into a pointer to a request object
Request connection::call_parser(std::string s)
{
  // Parse the request
  req = *(Request::Parse(s));
  return req;
} 

// Determines which handler the input uri should use
RequestHandler* connection::find_handler(Request& req)
{
	std::string mode = req.uri();

  // Go backward from the end of the uri
  for (int index = mode.length(); index > 0; index--) {
    // Update the size of the uri string 
    mode.resize(index);

    // Find longest matching path in the handlers
    if (handlers_.find(mode) != handlers_.end()) {
      // Found: return pointer to the handler
      RequestHandler* handler = handlers_[mode];
      return handler;
    }
  }
  // No matching handler found, return nullptr
  return nullptr;
}

// Call the handler that is associated with the current request
RequestHandler::Status connection::call_handler(RequestHandler* handler_mode)
{
  RequestHandler::Status status;

  // Check if the parser returned null
  if (handler_mode == nullptr) {
    handlers_["default"]->HandleRequest(req, &res);

  }
  else {
    // Check if the request handler returned not found
    status = handler_mode->HandleRequest(req, &res);
    if (status == RequestHandler::Status::NOT_FOUND){
      // Call the default/not found handler
      handlers_["default"]->HandleRequest(req, &res);
    }
  } 
    ServerMonitor::getInstance()->addRequest(req.uri(), res.GetStatus());
    // Write back to the client
    write_response();

  // Return the request
  return status;
}

// Write response to the client
void connection::write_response()
{
  auto self(shared_from_this());
  boost::asio::async_write(socket_, boost::asio::buffer(res.ToString()),
      [this, self](boost::system::error_code ec, std::size_t)
      {
        if (!ec)
        {
          // Initiate graceful connection closure.
          boost::system::error_code ignored_ec;
          socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
            ignored_ec);
        }

      });

}

} // namespace server
} // namespace http