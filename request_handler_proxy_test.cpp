#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "request_handler.h"
#include <sstream>

namespace http {
namespace server {

	TEST(HandlerProxyTest, InitProxyTest)
	{
		RequestHandler* handler = RequestHandler::CreateByName("ProxyHandler");
		NginxConfig config;
		RequestHandler::Status status = handler->Init("", config);

		EXPECT_EQ(status, 0);
	}

	TEST(HandlerProxyTest, HandleMakeRequest)
	{
		RequestHandler* handler = RequestHandler::CreateByName("ProxyHandler");

		// set up the config
		NginxConfigParser parser;
		NginxConfig config;
		std::stringstream config_stream("host google.com;\nport 8080;\n");
		parser.Parse(&config_stream, &config);

		Request req;
		Response res;

		handler->Init("", config);
		RequestHandler::Status status = handler->HandleRequest(req, &res);

		EXPECT_EQ(status, 0);
	}
} // namespace server
} // namespace http