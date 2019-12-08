CC=g++
EXTRAFLAGS = -std=gnu++14

test: formatter.o test.o
	$(CC) $(EXTRAFLAGS) -o test formatter.o test.o

formatter.o: formatter.cpp
	$(CC) $(EXTRAFLAGS) -c formatter.cpp

clean:
	rm -rf *.o parse
