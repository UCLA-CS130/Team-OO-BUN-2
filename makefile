GTEST_DIR=ngnix/googletest/googletest
GMOCK_DIR=ngnix/googletest/googlemock

all: echo_server

echo_server: main.cpp ngnix/config_parser.cc server_monitor.cpp response.cpp request.cpp request_handler.cpp request_handler_echo.cpp request_handler_static.cpp \
	request_handler_default.cpp request_handler_status.cpp server.cpp connection.cpp 
	g++ main.cpp ngnix/config_parser.cc server_monitor.cpp response.cpp request.cpp request_handler.cpp request_handler_echo.cpp request_handler_static.cpp \
	request_handler_default.cpp request_handler_status.cpp server.cpp connection.cpp \
	-std=c++0x -g -Wall -lboost_regex -lboost_system -lboost_thread -lpthread -o webserver


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
	server.cpp server_test.cpp connection.cpp connection_test.cpp \
	-isystem ${GTEST_DIR}/include -I${GTEST_DIR} ${GTEST_DIR}/src/gtest-all.cc ${GTEST_DIR}/src/gtest_main.cc \
	-isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} ${GMOCK_DIR}/src/gmock-all.cc \
	-std=c++0x -g -Wall -lboost_regex -lboost_system -fprofile-arcs -ftest-coverage -lpthread -o mytest
	./mytest
	gcov -r server.cpp connection.cpp request.cpp response.cpp request_handler_status.cpp request_handler_default.cpp request_handler_static.cpp server_monitor.cpp
	rm *.gcov *.gcda *.gcno

# gcov: server_test.cc connection_test.cc server.cpp ${GTEST_DIR}/src/gtest-all.cc connection.cpp ${GTEST_DIR}/src/gtest_main.cc ${GMOCK_DIR}/src/gmock-all.cc
# 	g++ server_test.cc connection_test.cc server.cpp -isystem ${GTEST_DIR}/include \
# 	-I${GTEST_DIR} ${GTEST_DIR}/src/gtest-all.cc connection.cpp ${GTEST_DIR}/src/gtest_main.cc \
# 	-isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} ${GMOCK_DIR}/src/gmock-all.cc \
# 	-std=c++0x -g -Wall -lboost_regex -lboost_system -fprofile-arcs -ftest-coverage -lpthread -o gcov_test

integration: integration_test.sh thread_test.py
	make
	./integration_test.sh
	python thread_test.py

clean:
	rm -rf *.o webserver