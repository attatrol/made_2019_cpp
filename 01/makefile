CC=g++
EXTRAFLAGS = -std=gnu++14

run: lexer.o parser.o run.o 
	$(CC) $(EXTRAFLAGS) -o run lexer.o parser.o run.o

test: lexer.o parser.o test.o
	$(CC) $(EXTRAFLAGS) -o test lexer.o parser.o test.o

run.o: run.cpp
	$(CC) $(EXTRAFLAGS) -c run.cpp

test.o: test.cpp
	$(CC) $(EXTRAFLAGS) -c test.cpp

lexer.o: lexer.cpp lexer.h error_codes.h
	$(CC) $(EXTRAFLAGS) -c lexer.cpp

parser.o: parser.cpp parser.h error_codes.h
	$(CC) $(EXTRAFLAGS) -c parser.cpp

clean:
	rm -rf *.o parse
