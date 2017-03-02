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
	  	bool is_success = make_request("http://www." + host + ":" + port, response)

	  	if (!is_success) {
	  		std::string fail_body = "Request to remote server failed"
	  		response->SetStatus(Response::ResponseCode::NOT_FOUND);
		  	response->AddHeader("Content-Type", "text/plain");
		  	response->AddHeader("Content-Length", std::to_string(fail_body.length()));
		  	response->SetBody(fail_body);
	  	}

		return RequestHandler::Status::OK;
	}

	bool make_request(std::string url, Response* response) {
		std::cout << "Making request to: " << url << std::endl;

		// TODO
	}

} // namespace server
} // namespace http
