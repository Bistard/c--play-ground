CC = g++
CXXFLAGS = -Wall -Werror -Wextra -pedantic -std=c++11 -g -fsanitize=address

OBJ = $(SRC:.cc=.o)
EXEC = test1

all: test1 clean

test1: test1.o
	$(CC) $(CXXFLAGS) test1.o -ltbb -lpthread -o test1 

test1.o: test1.cpp
	$(CC) $(CXXFLAGS) -c test1.cpp
clean:
	rm *.o
