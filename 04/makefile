CC=g++
EXTRAFLAGS = -std=gnu++14

test: matrix.o test.o
	$(CC) $(EXTRAFLAGS) -o test matrix.o test.o

test.o: test.cpp
	$(CC) $(EXTRAFLAGS) -c test.cpp

matrix.o: matrix.cpp matrix.h
	$(CC) $(EXTRAFLAGS) -c matrix.cpp

clean:
	rm -rf *.o parse
