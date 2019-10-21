CC=g++

test: test.o lexer.o parser.o
	$(CC) -o test lexer.o parser.o test.o

test.o: test.cpp
	$(CC) -c test.cpp

lexer.o: lexer.cpp lexer.h error_codes.h
	$(CC) -c lexer.cpp

parser.o: parser.cpp parser.h error_codes.h
	$(CC) -c parser.cpp

clean:
	rm -rf *.o parse
