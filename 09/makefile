CC=g++
EXTRAFLAGS = -std=gnu++17

test: lockfreetaskqueue.o threadpool.o test.o
	$(CC) $(EXTRAFLAGS) -o test lockfreetaskqueue.o threadpool.o test.o -pthread

test.o: test.cpp
	$(CC) $(EXTRAFLAGS) -c test.cpp

threadpool.o: threadpool.cpp threadpool.h lockfreetaskqueue.h
	$(CC) $(EXTRAFLAGS) -c threadpool.cpp

lockfreetaskqueue.o: lockfreetaskqueue.cpp lockfreetaskqueue.h
	$(CC) $(EXTRAFLAGS) -c lockfreetaskqueue.cpp

clean:
	rm -rf *.o parse
