CC = g++
CFLAGS  = -Wall -std=gnu++11 -g

all: midieditor

# ----------------------------------------------------------------------------
# Application

midieditor: midieditor.o
	$(CC) $(CFLAGS) -o midieditor midieditor.o libmidifile.a

midieditor.o: midieditor.cpp midieditor.hpp
	$(CC) $(CFLAGS) -c midieditor.cpp

# ----------------------------------------------------------------------------
# Clean up

clean:
	rm *.o

# ----------------------------------------------------------------------------
