#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <boost/asio.hpp>
#include "request_handler_static.h"

namespace http {
namespace server {

	TEST(StaticHandlerTest, InitTest)
	{
		RequestHandler* staticHandler = RequestHandler::CreateByName("StaticHandler");
		NginxConfigParser config_parser;
		NginxConfig config;
		config_parser.Parse("config", &config);

		NginxConfig child;
	      for (const auto& statement : config.statements_) {
	        for (unsigned int i = 0; i < statement->tokens_.size(); ++i) {
	          if (statement->tokens_[i] == "port")
	            ;
	          else if (statement->tokens_[i]=="path" && statement->tokens_[i+1]=="/echo")
	            ;
	          else if (statement->tokens_[i]=="path" && statement->tokens_[i+1]=="/status")
	            ;
	          else if (statement->tokens_[i]=="path")
	            child = *(statement->child_block_);
	        }
	      }

		RequestHandler::Status status = staticHandler->Init("/", child);

		EXPECT_EQ(status, RequestHandler::Status::OK);
	}

	TEST(StaticHandlerTest, HandleBadRequestTest)
	{
		RequestHandler* staticHandler = RequestHandler::CreateByName("StaticHandler");
		NginxConfigParser config_parser;
		NginxConfig config;
		config_parser.Parse("config", &config);

		NginxConfig child;
	      for (const auto& statement : config.statements_) {
	        for (unsigned int i = 0; i < statement->tokens_.size(); ++i) {
	          if (statement->tokens_[i] == "port")
	            ;
	          else if (statement->tokens_[i]=="path" && statement->tokens_[i+1]=="/echo")
	            ;
	          else if (statement->tokens_[i]=="path" && statement->tokens_[i+1]=="/status")
	            ;
	          else if (statement->tokens_[i]=="path")
	            child = *(statement->child_block_);
	        }
	      }

		staticHandler->Init("/", child);

		Request request;
		Response response;
		RequestHandler::Status status = staticHandler->HandleRequest(request, &response);
		EXPECT_EQ(status, RequestHandler::Status::BAD_REQUEST);
	}

	TEST(StaticHandlerTest, HandleOkRequestTest)
	{
		RequestHandler* staticHandler = RequestHandler::CreateByName("StaticHandler");
		NginxConfigParser config_parser;
		NginxConfig config;
		config_parser.Parse("config", &config);

		NginxConfig child;
	      for (const auto& statement : config.statements_) {
	      	if (statement->tokens_.size() == 3 && statement->tokens_[0] == "path"
	      		&& statement->tokens_[2] == "StaticHandler") {
	      		child = *(statement->child_block_);
	      	}
	      }

		staticHandler->Init("/", child);

		std::string raw_request = "GET / HTTP/1.1\r\n"
	 	"Host: localhost:2020\r\n"
		"User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:44.0) Gecko/20100101 Firefox/44.0\r\n"
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
		"Accept-Language: en-US,en;q=0.5\r\n"
		"Accept-Encoding: gzip, deflate\r\n"
		"Connection: keep-alive\r\n\r\n";

		std::unique_ptr<Request> request = Request::Parse(raw_request);
		Request req = *request;
		Response response;
		RequestHandler::Status status = staticHandler->HandleRequest(req, &response);
		EXPECT_EQ(status, RequestHandler::Status::OK);
	}

	TEST(StaticHandlerTest, HandleNotFoundRequestTest)
	{
		RequestHandler* staticHandler = RequestHandler::CreateByName("StaticHandler");
		NginxConfigParser config_parser;
		NginxConfig config;
		config_parser.Parse("config", &config);

		NginxConfig child;
	      for (const auto& statement : config.statements_) {
	        for (unsigned int i = 0; i < statement->tokens_.size(); ++i) {
	          if (statement->tokens_[i] == "port")
	            ;
	          else if (statement->tokens_[i]=="path" && statement->tokens_[i+1]=="/echo")
	            ;
	          else if (statement->tokens_[i]=="path" && statement->tokens_[i+1]=="/status")
	            ;
	          else if (statement->tokens_[i]=="path")
	            child = *(statement->child_block_);
	        }
	      }

		staticHandler->Init("/", child);

		std::string raw_request = "GET /sddfsdf HTTP/1.1\r\n"
	 	"Host: localhost:2020\r\n"
		"User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:44.0) Gecko/20100101 Firefox/44.0\r\n"
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
		"Accept-Language: en-US,en;q=0.5\r\n"
		"Accept-Encoding: gzip, deflate\r\n"
		"Connection: keep-alive\r\n\r\n";

		std::unique_ptr<Request> request = Request::Parse(raw_request);
		Request req = *request;
		Response response;
		RequestHandler::Status status = staticHandler->HandleRequest(req, &response);
		EXPECT_EQ(status, RequestHandler::Status::NOT_FOUND);
	}
}
}