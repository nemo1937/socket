
CXX = g++

CC_OPTS = -Wall -ggdb3 -std=c++11 -I./inc

LIB = -pthread

TARGET = select_client select_server

all : $(TARGET)

clean:
	rm -rf *.o $(TARGET)

select_client : src/select_client.cpp
	$(CXX) $(CC_OPTS) $^ -o $@ $(LIB)
	
select_server : src/select_server.cpp
	$(CXX) $(CC_OPTS) $^ -o $@