CC=g++
EXTRAFLAGS = -std=gnu++14

test: saxparser.o test.o
	$(CC) $(EXTRAFLAGS) -o test test.o saxparser.o

test.o: test.cpp
	$(CC) $(EXTRAFLAGS) -c test.cpp

saxparser.o: saxparser.cpp saxparser.h observable.h
	$(CC) $(EXTRAFLAGS) -c saxparser.cpp

clean:
	rm -rf *.o parse
