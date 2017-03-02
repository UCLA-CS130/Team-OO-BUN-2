#ifndef PROXY_HANDLER_H
#define PROXY_HANDLER_H

#include "request_handler.h"
#include "response_parser.h"
#include <iostream>

namespace http {
	namespace server {
		class ProxyHandler : public RequestHandler {
		public:
			Status Init(const std::string& uri_prefix, const NginxConfig& config) override;

  			Status HandleRequest(const Request& request, Response* response) override;
		private:
			ResponseParser resp_parser;

			// domain name of host to proxy
			std::string host;

			// port num as a string
			std::string port;

			// makes request to given host and returns raw response
			// handles 302 error
			std::string make_request(std::string url);
		};

		REGISTER_REQUEST_HANDLER(ProxyHandler);
	} // namespace server
} // namespace http

#endif  // PROXY_HANDLER_H