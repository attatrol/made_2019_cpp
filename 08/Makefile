CC=g++
EXTRAFLAGS = -std=gnu++14

test: vector.o test.o
	$(CC) $(EXTRAFLAGS) -o test vector.o test.o

vector.o: vector.cpp
	$(CC) $(EXTRAFLAGS) -c vector.cpp

clean:
	rm -rf *.o parse
