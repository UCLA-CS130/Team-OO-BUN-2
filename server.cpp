#include "server.h"
#include <signal.h>
#include <utility>
#include <string>
#include <iostream>
#include <cstddef>

#include "request_handler_default.h"
#include "request_handler_echo.h"
#include "request_handler_static.h"
#include "request_handler_status.h"


namespace http {
namespace server {
    //Constructor
    server::server(const std::string& address, NginxConfig config)
      : io_service_(), signals_(io_service_), acceptor_(io_service_), socket_(io_service_)
    {


      // map from path -> (handler_name, root)
      std::map <std::string, std::pair<std::string,NginxConfig>> path_info;

      for (const auto& statement : config.statements_) {
        for (unsigned int i = 0; i < statement->tokens_.size(); ++i) {

          if (statement->tokens_.size() == 2 && statement->tokens_[i] == "port")
            portNum_ = statement->tokens_[i+1];

          else if (statement->tokens_.size() == 3 && statement->tokens_[i]=="path" && statement->tokens_[i+2]=="EchoHandler"){
            std::string echo_handler = statement->tokens_[i+2];
            NginxConfig echo_config;
            path_info[statement->tokens_[i+1]] = std::make_pair(echo_handler, echo_config);
            ServerMonitor::getInstance()->addHandler(statement->tokens_[i+2], statement->tokens_[i+1]); 
          }

          else if (statement->tokens_.size() == 3 && statement->tokens_[i]=="path" && statement->tokens_[i+2]=="StatusHandler"){
            std::string status_handler = statement->tokens_[i+2];
            NginxConfig status_config;
            path_info[statement->tokens_[i+1]] = std::make_pair(status_handler, status_config);
            ServerMonitor::getInstance()->addHandler(statement->tokens_[i+2], statement->tokens_[i+1]); 

          }

          else if (statement->tokens_.size() == 3 && statement->tokens_[i]=="path" && statement->tokens_[i+2]=="StaticHandler"){

            std::string path = statement->tokens_[i+1];
            std::string static_handler = statement->tokens_[i+2];

            NginxConfig child = *(statement->child_block_);
            path_info[path] = std::make_pair(static_handler, child);
            ServerMonitor::getInstance()->addHandler(statement->tokens_[i+2], statement->tokens_[i+1]); 
          }

          else if (statement->tokens_.size() == 2 && statement->tokens_[i]=="default"){ 
              NginxConfig default_config;
              path_info[statement->tokens_[i]]= std::make_pair(statement->tokens_[i+1], default_config);
              ServerMonitor::getInstance()->addHandler(statement->tokens_[i+1], ""); 
          }

        }
      }

      std::cout << std::endl << "PORT: " << portNum_ << std::endl;

      std::cout << std::endl << "HANDLERS: " << std::endl;
      // for(auto iterator = path_info.begin(); iterator != path_info.end(); iterator++) {
      //   // iterator->first = key
      //   // iterator->second = value
      //   std::cout << iterator->first << "\t" <<
      //           iterator->second.first << "\t" <<
      //           iterator->second.second << std::endl;
      // }
      // std::cout << std::endl;


      for(auto iterator = path_info.begin(); iterator != path_info.end(); iterator++) {
          std::string path = iterator->first;
          handlers[path] = RequestHandler::CreateByName(iterator->second.first.c_str());
          handlers[path]->Init(path, iterator->second.second);
      }


      if (stoi(portNum_) < 0 || stoi(portNum_) > 65535) {
        std::cerr << "Error: Port number not in range [0:65535].\n";
      }



      boost::asio::ip::tcp::resolver resolver(io_service_);
      boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({address, portNum_});
      acceptor_.open(endpoint.protocol());
      acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
      acceptor_.bind(endpoint);
      acceptor_.listen();

      address_ = address;

      signals_.add(SIGINT);
      signals_.add(SIGTERM);
      #if defined(SIGQUIT)
      signals_.add(SIGQUIT);
      #endif // defined(SIGQUIT)

      do_await_stop();

      do_accept();
    }

    void server::run()
    {
      isRunning = true; 
      io_service_.run();
    }

    bool server::getStatus() 
    {
      return isRunning; 
    }

    std::string server::getPortNum(){
      return portNum_; 
    }

    std::string server::getAddress(){
      return address_;
    }

    void server::do_await_stop()
    {
      signals_.async_wait(
      [this](boost::system::error_code /*ec*/, int /*signo*/)
      {
        // The server is stopped by cancelling all outstanding asynchronous
        // operations. Once all operations have finished the io_service::run()
        // call will exit.
        std::cout << " \n";
        acceptor_.close();
        io_service_.stop();
      });
    }

    void server::do_accept()
    {
      acceptor_.async_accept(socket_, 
        [this](boost::system::error_code ec){
          // error check, if the website name given is not valid then this will return
          if (!acceptor_.is_open()) {
            return;
          }

          // if accept handle did not detect any problems with creating then start the connection
          if (!ec) {
            // Creates a shared connection ptr and calls start on it
            // std::move gets rid of the copy constructor delete error
            auto my_connection = std::make_shared<connection> (std::move(socket_), handlers);
            my_connection->start(); 
          }

          do_accept();
        });
    }

    bool server::create_connection(boost::system::error_code ec){
      isRunning = true;
      if (ec == 0)
        return true;
      else
        return false;
    }
  }
}