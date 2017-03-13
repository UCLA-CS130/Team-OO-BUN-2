#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "server.h"
#include <boost/asio.hpp>
// #include "mock_connection.h"

boost::asio::io_service io_service_;
boost::asio::ip::tcp::socket socket_(io_service_);


class ServerTest: public ::testing::Test {
protected:
	int errc_success = 0;

    bool fake_connection(boost::system::error_code ec) 
    {
    	NginxConfigParser config_parser;
  		NginxConfig config;
  		config_parser.Parse("config", &config);
      	http::server::server s("localhost", config);
      	return s.create_connection(ec);
    }

    boost::system::error_code create_error(int error_code)
	{
		if (error_code == errc_success)
			return boost::system::errc::make_error_code(boost::system::errc::success);
		else 
			return boost::system::errc::make_error_code(boost::system::errc::invalid_argument);
	}
};

TEST_F(ServerTest, RunTest) {
	NginxConfigParser config_parser;
	NginxConfig config;
	config_parser.Parse("config", &config);
	http::server::server("localhost", config);
	EXPECT_TRUE(true);
}

TEST_F(ServerTest, ServerRunning) {
	NginxConfigParser config_parser;
	NginxConfig config;
	config_parser.Parse("config", &config);
	http::server::server s("localhost", config);
	s.create_connection(create_error(0));
	EXPECT_TRUE(s.getStatus());
}

// TEST_F(ServerTest, ServerRunning2) {
// 	NginxConfigParser config_parser;
// 	NginxConfig config;
// 	config_parser.Parse("config", &config);
// 	http::server::server s("localhost", config);
// 	// s.run();
// 	EXPECT_TRUE(s.getStatus());
// }


TEST_F(ServerTest, ErrorSuccess) {
	EXPECT_TRUE(fake_connection(create_error(0)));
}

TEST_F(ServerTest, ErrorFail) {
	EXPECT_FALSE(fake_connection(create_error(1)));
}

TEST_F(ServerTest, CorrectInfoTest) {
	NginxConfigParser config_parser;
	NginxConfig config;
	config_parser.Parse("config", &config);
	http::server::server s("localhost", config); 

	EXPECT_EQ("8001", s.getPortNum());
	EXPECT_EQ("localhost", s.getAddress());  
}

// TEST_F(ServerTest, MockTest) {
// 	//MockConnection mock_connection(std::move(socket_));
// 	EXPECT_TRUE(true);
// }