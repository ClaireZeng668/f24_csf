CXX = g++
CXXFLAGS = -g -Wall -std=c++17

CC = gcc
CFLAGS = -g -Wall -std=gnu11

# Common C++ sources for clients/server/unit test program
CXX_COMMON_SRCS = message.cpp message_serialization.cpp table.cpp value_stack.cpp
CXX_COMMON_OBJS = $(CXX_COMMON_SRCS:%.cpp=%.o)

# Server-only C++ sources
CXX_SERVER_SRCS = server.cpp client_connection.cpp server_main.cpp
CXX_SERVER_OBJS = $(CXX_SERVER_SRCS:%.cpp=%.o)

# C++ client common sources (used by all clients)
CXX_CLIENT_SRCS =
CXX_CLIENT_OBJS = $(CXX_CLIENT_SRCS:%.cpp=%.o)

# C++ client main function sources
CXX_CLIENT_MAIN_SRCS = get_value.cpp set_value.cpp incr_value.cpp
CXX_CLIENT_MAIN_EXES = $(CXX_CLIENT_MAIN_SRCS:%.cpp=%)

# C++ sources for unit tests
CXX_TEST_SRCS = unit_tests.cpp
CXX_TEST_OBJS = $(CXX_TEST_SRCS:%.cpp=%.o)

# All C++ sources (for generating header dependencies)
CXX_ALL_SRCS = $(CXX_COMMON_SRCS) $(CXX_SERVER_SRCS) $(CXX_CLIENT_SRCS) $(CXX_CLIENT_MAIN_SRCS)

# Common C sources for both clients and server
C_COMMON_SRCS = csapp.c
C_COMMON_OBJS = $(C_COMMON_SRCS:%.c=%.o)

# C sources for unit tests
C_TEST_SRCS = tctest.c
C_TEST_OBJS = $(C_TEST_SRCS:%.c=%.o)

# All C sources (for generating header dependencies)
C_ALL_SRCS = $(C_COMMON_SRCS) $(C_TEST_SRCS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $*.cpp -o $*.o

%.o : %.c
	$(CC) $(CFLAGS) -c $*.c -o $*.o

all : unit_tests server $(CXX_CLIENT_MAIN_EXES)

server : $(CXX_SERVER_OBJS) $(CXX_COMMON_OBJS) $(C_COMMON_OBJS)
	$(CXX) -o $@ $(CXX_SERVER_OBJS) $(CXX_COMMON_OBJS) $(C_COMMON_OBJS) -lpthread

unit_tests : $(CXX_COMMON_OBJS) $(CXX_TEST_OBJS) $(C_TEST_OBJS)
	$(CXX) -o $@ $(CXX_COMMON_OBJS) $(CXX_TEST_OBJS) $(C_TEST_OBJS)

get_value : get_value.o $(CXX_COMMON_OBJS) $(CXX_CLIENT_OBJS) $(C_COMMON_OBJS)
	$(CXX) -o $@ get_value.o $(CXX_COMMON_OBJS) $(CXX_CLIENT_OBJS) $(C_COMMON_OBJS)

set_value : set_value.o $(CXX_COMMON_OBJS) $(CXX_CLIENT_OBJS) $(C_COMMON_OBJS)
	$(CXX) -o $@ set_value.o $(CXX_COMMON_OBJS) $(CXX_CLIENT_OBJS) $(C_COMMON_OBJS)

incr_value : incr_value.o $(CXX_COMMON_OBJS) $(CXX_CLIENT_OBJS) $(C_COMMON_OBJS)
	$(CXX) -o $@ incr_value.o $(CXX_COMMON_OBJS) $(CXX_CLIENT_OBJS) $(C_COMMON_OBJS)

.PHONY: solution.zip
solution.zip :
	rm -f $@
	zip -9r $@ *.h *.c *.cpp Makefile README.txt

clean :
	rm -f *.o unit_tests server $(CXX_CLIENT_MAIN_EXES) depend.mak

depend :
	$(CXX) $(CXXFLAGS) -M $(CXX_ALL_SRCS) > depend.mak
	$(CC) $(CFLAGS) -M $(C_ALL_SRCS) >> depend.mak

depend.mak :
	touch $@

include depend.mak
