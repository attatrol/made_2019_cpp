CC=g++
EXTRAFLAGS = -std=gnu++14

test: bigint.o deserializer.o test.o
	$(CC) $(EXTRAFLAGS) -o test bigint.o test.o

test.o: test.cpp
	$(CC) $(EXTRAFLAGS) -c test.cpp

bigint.o: bigint.cpp bigint.h
	$(CC) $(EXTRAFLAGS) -c bigint.cpp

clean:
	rm -rf *.o parse