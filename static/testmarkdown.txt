# TEAM-OO-BUN-2

Implementation of the echo server was done using the Boost Documentation Examples. 
[Boost documentation](http://www.boost.org/doc/libs/1_62_0/doc/html/boost_asio/examples/cpp11_examples.html#boost_asio.examples.cpp11_examples.spawn)

## Usage

Clone the repository:
```
git clone --recursive https://github.com/UCLA-CS130/Team-OO-BUN-2.git
```
Change directory into the webserver directory
```
cd Team-OO-BUN-2/echo_server/webserver/
```

Make
```
make
```

Run the server
```
./webserver <config>
```
Our default `<config>` file is called `config`

Open browser (Chrome, Firefox, etc.) and go to `localhost:<port>`.

## Run Unit Tests
```
make test
```
At the end of `make test`, the coverage of the files are presented.

## Integration Test
```
./integration_test.sh
```

## Test Coverage
`make test` will provide the test coverage information


# Code Layout
* main.cpp: Takes in a configuration file and creates and passes the file to the server
* server.cpp: Maps the config file paths to handlers. 
 * The request handlers are initialized here. 
 * The handlers are passed into a connection that is created (by calling connection::start()). 
* connection.cpp: This is where most of the work is done by the server.
 * The request handlers are taken in from the server and saved into a private member.
 * Request data is read into a buffer and turned into a string in connection::do_read().
 * connection::call_parser is called and parses the request using the Request::Parse(...) function.
 * After parsing the request, the uri of the request is used to find the longest matching prefix using the connection::find_handler method which returns the corresponding request handler in the map.
 * connection::call_handler(...) sends the request to the handler and obtains the response.
 * connection::write_response() writes the response back to the user
* request_handler.h: Holds the implementation of the RequestHandler, Request, and Response classes.
 * request.cpp and response.cpp contains the implementation of the two classes.
 * request_handler.cpp contains the implementation of the RequestHandler class.
* request_handler_xxxxx.cpp: The implementation of our different request handlers:
 * Echo
 * Default (Not found)
 * Static
 * Status

## Folders
* `static`, `static1`, and `static2` contain the files that can be obtained by issuing a static request

# Docker
## Deploy our server
```
make deloy
```
## Push onto AWS
```
make aws
```
You can connect to the server on AWS using the following two URLs in a browser:
```
ec2-54-241-154-77.us-west-1.compute.amazonaws.com:8080/
```
or 
```
54.241.154.77:8080/
```