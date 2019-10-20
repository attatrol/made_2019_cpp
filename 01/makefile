CC=g++

run: calc run.o

test: calc test.o

parse: lexer.o parser.o 
    $(CC) -o parse lexer.o parser.o

lexer.o: lexer.cpp lexer.h
    $(CC) -c lexer.cpp

parser.o: lexer.cpp lexer.h parser.h
    $(CC) -c parser.cpp

clean:
    rm -rf *.o parse