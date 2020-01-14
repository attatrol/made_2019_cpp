CC=g++
EXTRAFLAGS = -std=gnu++14

test: serializer.o deserializer.o test.o
	$(CC) $(EXTRAFLAGS) -o test serializer.o deserializer.o test.o

test.o: test.cpp
	$(CC) $(EXTRAFLAGS) -c test.cpp

serializer.o: serializer.cpp serializer.h error.h
	$(CC) $(EXTRAFLAGS) -c serializer.cpp
	
deserializer.o: deserializer.cpp deserializer.h error.h
	$(CC) $(EXTRAFLAGS) -c deserializer.cpp

clean:
	rm -rf *.o parse
