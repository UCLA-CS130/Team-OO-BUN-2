#ifndef PROXY_HANDLER_H
#define PROXY_HANDLER_H

#include "request_handler.h"
#include <iostream>

namespace http {
	namespace server {
		class ProxyHandler : public RequestHandler {
		public:
			Status Init(const std::string& uri_prefix, const NginxConfig& config) override;

  			Status HandleRequest(const Request& request, Response* response) override;
		};


		REGISTER_REQUEST_HANDLER(ProxyHandler);
	} // namespace server
} // namespace http

#endif  // PROXY_HANDLER_H