GTEST_DIR=ngnix/googletest/googletest
GMOCK_DIR=ngnix/googletest/googlemock

LDFLAGS=-static-libgcc -static-libstdc++ -pthread -Wl,-Bstatic -lboost_log_setup -lboost_regex -lboost_log -lboost_thread -lboost_system
CXXFLAGS=-std=c++0x -Wall -Werror

all: echo_server

echo_server: server.o connection.o server_monitor.o response.o request.o \
	markdown.o markdown-tokens.o request_handler.o response_parser.o \
	request_handler_echo.o request_handler_static.o request_handler_default.o request_handler_status.o request_handler_proxy.o \
	main.o config_parser.o
	g++ -o webserver config_parser.o server.o connection.o main.o \
	server_monitor.o response.o request.o response_parser.o markdown.o markdown-tokens.o \
	request_handler.o request_handler_echo.o request_handler_static.o request_handler_default.o request_handler_status.o request_handler_proxy.o \
	$(CXXFLAGS) $(LDFLAGS)

server.o: server.cpp server.h
	g++ -c server.cpp $(CXXFLAGS) $(LDFLAGS)

connection.o: connection.cpp connection.h
	g++ -c connection.cpp $(CXXFLAGS) $(LDFLAGS)

server_monitor.o: server_monitor.cpp server_monitor.h
	g++ -c server_monitor.cpp $(CXXFLAGS) $(LDFLAGS)

response.o: response.cpp request_handler.h
	g++ -c response.cpp $(CXXFLAGS) $(LDFLAGS)

request.o: request.cpp request_handler.h
	g++ -c request.cpp $(CXXFLAGS) $(LDFLAGS)

markdown.o: markdown.cpp markdown.h
	g++ -c markdown.cpp $(CXXFLAGS) $(LDFLAGS)

markdown-tokens.o: markdown-tokens.cpp markdown-tokens.h
	g++ -c markdown-tokens.cpp $(CXXFLAGS) $(LDFLAGS)

request_handler.o: request_handler.cpp request_handler.h
	g++ -c request_handler.cpp $(CXXFLAGS) $(LDFLAGS)

request_handler_echo.o: request_handler_echo.cpp request_handler.h
	g++ -c request_handler_echo.cpp $(CXXFLAGS) $(LDFLAGS)

request_handler_static.o: request_handler_static.cpp request_handler.h
	g++ -c request_handler_static.cpp $(CXXFLAGS) $(LDFLAGS)

request_handler_default.o: request_handler_default.cpp request_handler.h
	g++ -c request_handler_default.cpp $(CXXFLAGS) $(LDFLAGS)

request_handler_status.o: request_handler_status.cpp request_handler.h
	g++ -c request_handler_status.cpp $(CXXFLAGS) $(LDFLAGS)

request_handler_proxy.o: request_handler_proxy.cpp request_handler.h
	g++ -c request_handler_proxy.cpp $(CXXFLAGS) $(LDFLAGS)

response_parser.o: response_parser.cpp response_parser.h
	g++ -c response_parser.cpp $(CXXFLAGS) $(LDFLAGS)

main.o: main.cpp ngnix/config_parser.cc
	g++ -c main.cpp $(CXXFLAGS) $(LDFLAGS)

config_parser.o: ngnix/config_parser.cc
	g++ -c ngnix/config_parser.cc $(CXXFLAGS) $(LDFLAGS)


test: ngnix/config_parser.cc server_monitor.cpp \
	response.cpp response_test.cpp \
	request.cpp request_test.cpp \
	request_handler.cpp \
	request_handler_echo.cpp request_handler_echo_test.cpp \
	request_handler_static.cpp request_handler_static_test.cpp \
	request_handler_default.cpp request_handler_default_test.cpp \
	request_handler_status.cpp request_handler_status_test.cpp \
	server.cpp server_test.cpp \
	connection.cpp connection_test.cpp \
	${GTEST_DIR}/src/gtest-all.cc ${GTEST_DIR}/src/gtest_main.cc ${GMOCK_DIR}/src/gmock-all.cc
	g++ ngnix/config_parser.cc server_monitor.cpp \
	response.cpp response_test.cpp \
	request.cpp request_test.cpp \
	request_handler.cpp \
	request_handler_echo.cpp request_handler_echo_test.cpp \
	request_handler_static.cpp request_handler_static_test.cpp \
	request_handler_default.cpp request_handler_default_test.cpp \
	request_handler_status.cpp request_handler_status_test.cpp \
	request_handler_proxy.cpp request_handler_proxy_test.cpp response_parser.cpp \
	server.cpp server_test.cpp connection.cpp connection_test.cpp \
	-isystem ${GTEST_DIR}/include -I${GTEST_DIR} ${GTEST_DIR}/src/gtest-all.cc ${GTEST_DIR}/src/gtest_main.cc \
	-isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} ${GMOCK_DIR}/src/gmock-all.cc \
	-std=c++0x -g -Wall -lboost_regex -lboost_system -fprofile-arcs -ftest-coverage -lboost_thread -lpthread -o mytest
	./mytest
	gcov -r server.cpp connection.cpp request.cpp response.cpp request_handler_status.cpp request_handler_default.cpp request_handler_static.cpp server_monitor.cpp
	rm *.gcov *.gcda *.gcno

# gcov: server_test.cc connection_test.cc server.cpp ${GTEST_DIR}/src/gtest-all.cc connection.cpp ${GTEST_DIR}/src/gtest_main.cc ${GMOCK_DIR}/src/gmock-all.cc
# 	g++ server_test.cc connection_test.cc server.cpp -isystem ${GTEST_DIR}/include \
# 	-I${GTEST_DIR} ${GTEST_DIR}/src/gtest-all.cc connection.cpp ${GTEST_DIR}/src/gtest_main.cc \
# 	-isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} ${GMOCK_DIR}/src/gmock-all.cc \
# 	-std=c++0x -g -Wall -lboost_regex -lboost_system -fprofile-arcs -ftest-coverage -lpthread -o gcov_test

integration: integration_test.sh thread_test.py reverse_proxy_integration_302.py reverse_proxy_integration.py
	make
	./integration_test.sh
	python thread_test.py
	python reverse_proxy_integration.py
	python reverse_proxy_integration_302.py

deploy:
	rm -rf deploy
	docker build -t httpserver.build .
	docker run httpserver.build > binary.tar
	mkdir deploy
	cp config deploy/
	cp -r static/ deploy/
	cp binary.tar deploy/
	cp Dockerfile.run deploy/Dockerfile
	cd deploy && tar -xvf binary.tar
	rm binary.tar
	cd ..
	docker build -t httpserver deploy

aws:
	./aws.sh

clean:
	rm -rf *.o webserver deploy/
