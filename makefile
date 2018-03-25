
CXX = g++

CC_OPTS = -Wall -ggdb3 -std=c++11 -I./inc 

LIB = -pthread

ALIB = sw_tcp_socket_utils

LPATH = ./lib

TARGET = bin/select_client bin/select_server lib/libsw_tcp_socket_utils.a bin/sw_tcp_socket_server bin/sw_tcp_socket_client bin/datetime_tcp_server bin/datetime_tcp_client bin/echo_tcp_server bin/echo_tcp_client

all : $(TARGET)

clean:
	rm -rf src/*.o $(TARGET)
	
	
lib/libsw_tcp_socket_utils.a : src/sw_tcp_socket_utils.o
	@echo "complie utils.a"
	ar  crv $@ $^
	
src/sw_tcp_socket_utils.o : src/sw_tcp_socket_utils.cpp
	$(CXX)  $(CC_OPTS) -c -nostartfiles $^ -o $@

bin/select_client : src/select_client.cpp
	$(CXX) $(CC_OPTS) $^ -o $@ $(LIB)
	
bin/select_server : src/select_server.cpp
	$(CXX) $(CC_OPTS) $^ -o $@
	
bin/sw_tcp_socket_server : src/sw_tcp_socket_server.o
	$(CXX) -o $@ $^ -L$(LPATH) -l$(ALIB)

src/sw_tcp_socket_server.o : src/sw_tcp_socket_server.cpp
	$(CXX) $(CC_OPTS) -c $^ -o $@

#src/sw_tcp_socket_utils.o : src/sw_tcp_socket_utils.cpp
#	$(CXX) $(CC_OPTS) -c $^ -o $@

bin/sw_tcp_socket_client : src/sw_tcp_socket_client.o
	$(CXX)  $^ -o $@

src/sw_tcp_socket_client.o : src/sw_tcp_socket_client.cpp
	$(CXX) $(CC_OPTS) -c $^ -o $@
	
bin/datetime_tcp_server : src/datetime_tcp_server.o
	$(CXX) $^ -o $@ -L$(LPATH) -l$(ALIB)

src/datetime_tcp_server.o : src/datetime_tcp_server.cpp
	$(CXX) $(CC_OPTS) -c $^ -o $@

bin/datetime_tcp_client : src/datetime_tcp_client.o
	$(CXX) $^ -o $@ -L$(LPATH) -l$(ALIB)

src/datetime_tcp_client.o : src/datetime_tcp_client.cpp
	$(CXX) $(CC_OPTS) -c $^ -o $@
	
bin/echo_tcp_server : src/echo_tcp_server.o
	$(CXX) $^ -o $@ -L$(LPATH) -l$(ALIB)

src/echo_tcp_server.o : src/echo_tcp_server.cpp	
	$(CXX) $(CC_OPTS) -c $^ -o $@
	
bin/echo_tcp_client : src/echo_tcp_client.o
	$(CXX) $^ -o $@ -L$(LPATH) -l$(ALIB)

src/echo_tcp_client.o : src/echo_tcp_client.cpp	
	$(CXX) $(CC_OPTS) -c $^ -o $@
	