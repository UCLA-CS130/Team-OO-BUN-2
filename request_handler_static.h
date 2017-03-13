#ifndef STATIC_FILE_HANDLER_H
#define STATIC_FILE_HANDLER_H

#include "request_handler.h"
#include <iostream>

namespace http {
	namespace server {
		class StaticHandler : public RequestHandler {
		public: 
			Status Init(const std::string& uri_prefix, const NginxConfig& config) override;

  			Status HandleRequest(const Request& request, Response* response) override;
  			
  		private:
  			bool url_decode(const std::string& in, std::string& out);
  			std::string compress(std::string& data);
		};

		REGISTER_REQUEST_HANDLER(StaticHandler);
	} // namespace server
} // namespace http

#endif