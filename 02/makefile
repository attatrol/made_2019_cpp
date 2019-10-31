CC=g++
EXTRAFLAGS = -std=gnu++14

test: linearallocator.o test.o
	$(CC) $(EXTRAFLAGS) -o test linearallocator.o test.o

test.o: test.cpp
	$(CC) $(EXTRAFLAGS) -c test.cpp

linearallocator.o: linearallocator.cpp linearallocator.h
	$(CC) $(EXTRAFLAGS) -c linearallocator.cpp

clean:
	rm -rf *.o parse
