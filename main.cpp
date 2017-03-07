#include <iostream> 
#include <stdio.h>
using namespace std;

#include <boost/regex.hpp>
#include <boost/asio.hpp>
#include "server.h"


int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("Usage: ./config_parser <path to config file>\n");
    return 1;
  }

  NginxConfigParser config_parser;
  NginxConfig config;
  config_parser.Parse(argv[1], &config);

  http::server::server s("0.0.0.0", config);
  s.run();
  return 0;
}