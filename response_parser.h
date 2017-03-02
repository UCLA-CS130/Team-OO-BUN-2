#ifndef RESPONSE_PARSER_H
#define RESPONSE_PARSER_H

#include <string>

//this class only parses the headers of a response
//once we get the content-length, the payload can be gathered after!

class ResponseParser{
public:

	ResponseParser(){
		redirect_url = "";
	}

	//parses the http response headers (does not include payload)
	int parse_response(std::string response);
	int get_response_code();
	int get_content_length();
	std::string get_redirect_url();

private:
	int response_code;
	int content_length;
	std::string redirect_url;
	bool is_a_number(const std::string s);
	//looks through the first line for the response code
	std::string find_response_code(std::string response);
	//finds the value for a header no matter the header, and returns that value as a string
	std::string find_header_value(std::string header, std::string response);
};

#endif