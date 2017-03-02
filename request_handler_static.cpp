#include "request_handler_static.h"
#include <sstream> 
#include <fstream>

namespace http {
  namespace server {

    RequestHandler::Status StaticHandler::Init(const std::string& uri_prefix, const NginxConfig& config) {

        mUri_prefix = uri_prefix;

        std::vector<std::shared_ptr<NginxConfigStatement>> statements = 
                                config.statements_;
        for (unsigned int j = 0; j < statements.size(); ++j) {
          for (unsigned int k = 0; k < statements[j]->tokens_.size(); ++k) {
            if (statements[j]->tokens_[k]=="root") {
              mRoot = statements[j]->tokens_[k+1];
            }
          }
        }

        //TODO: error handling for empty prefix
        return Status::OK;
    }

    RequestHandler::Status StaticHandler::HandleRequest(const Request& request, Response* response) {
        std::cout << "StaticFileHandler::HandleRequest called" << std::endl;

        std::string request_path;

        // Try to decode the uri that was passed in
        if (!url_decode(request.uri(), request_path))
        {
          response->SetStatus(Response::ResponseCode::BAD_REQUEST);
          response->SetBody("Bad Request: Bad url decoding");  
          return RequestHandler::Status::BAD_REQUEST;
        }

        // Request path must be absolute and not contain "..".
        if (request_path.empty() || request_path[0] != '/'
            || request_path.find("..") != std::string::npos)
        {
          response->SetStatus(Response::ResponseCode::BAD_REQUEST);
          response->SetBody("Bad Request: Bad pathing"); 
          return RequestHandler::Status::BAD_REQUEST;
        }

        // Static requests should be formattted /static/, with the second "/"
        std::size_t second_slash = request_path.find("/", 1);
        if (second_slash != std::string::npos)
          request_path = request_path.substr(second_slash);

        // Direct to default page
        if (request_path.size() == 1 && request_path[request_path.size() - 1] == '/')
        {
          request_path += "index.html";
        }

        // Find the extension of the file request
        std::size_t last_slash_pos = request_path.find_last_of("/");
        std::size_t last_dot_pos = request_path.find_last_of(".");
        std::string extension;
        if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
        {
          extension = request_path.substr(last_dot_pos + 1);
        }

        // Types of supported file extensions from mime_types
        std::string extension_type; 
        if(extension == "jpg"){
          extension_type = "image/jpeg"; 
        }
        else if (extension == "gif"){
          extension_type = "image/gif";
        }
        else if (extension == "htm") {
          extension_type = "text/html"; 
        }
        else if (extension == "html") {
          extension_type = "text/html"; 
        }
        else if (extension == "png") {
          extension_type == "image/png"; 
        }

        // Open the file to send back.
        std::string full_path = mRoot + request_path;

        std::cout << "full path = " << full_path << std::endl; 

        std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
        if (!is)
        {
          response->SetStatus(Response::ResponseCode::NOT_FOUND);
          response->SetBody("NOT FOUND"); 
          return RequestHandler::Status::NOT_FOUND;
        }

        std::string contents = ""; 
        std::stringstream sstr;
        sstr << is.rdbuf();
        contents = sstr.str();

        //Good response, fill out the response 
        response->SetStatus(Response::ResponseCode::OK);
        response->SetBody(contents);
        response->AddHeader("Content-Length", std::to_string(contents.length())); 
        response->AddHeader("Content-Type", extension_type); 

        return RequestHandler::Status::OK;
      }

    bool StaticHandler::url_decode(const std::string& in, std::string& out)
    {
      out.clear();
      out.reserve(in.size());
      for (std::size_t i = 0; i < in.size(); ++i)
      {
        if (in[i] == '%')
        {
          if (i + 3 <= in.size())
          {
            int value = 0;
            std::istringstream is(in.substr(i + 1, 2));
            if (is >> std::hex >> value)
            {
              out += static_cast<char>(value);
              i += 2;
            }
            else
            {
              return false;
            }
          }
          else
          {
            return false;
          }
        }
        else if (in[i] == '+')
        {
          out += ' ';
        }
        else
        {
          out += in[i];
        }
      }
      return true;
    }

  } // namespace server
} // namespace http