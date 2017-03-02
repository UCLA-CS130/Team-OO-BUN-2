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
				}
			}
		}

		return RequestHandler::Status::OK;
	}


	RequestHandler::Status ProxyHandler::HandleRequest(const Request& request, Response* response) {
	  	std::cout << "ProxyHandler::HandleRequest called" << std::endl;
	  	std::cout << "Making request to: http://www." << host << ":" << port << std::endl;

	  	response->SetStatus(Response::ResponseCode::OK);
	  	response->AddHeader("Content-Type", "text/plain");
	  	response->AddHeader("Content-Length", std::to_string(request.raw_request().length()));
	  	response->SetBody(request.raw_request());

		return RequestHandler::Status::OK;
	}

} // namespace server
} // namespace http
