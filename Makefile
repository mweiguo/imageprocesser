INCLUDE = -I../tinylog/src
CPP = g++
CPPFLAG = -g

a.exe : test.o bmp.o log.o
	g++ test.o bmp.o log.o -o a.exe

log.o : ../tinylog/src/tinylog.cpp ../tinylog/src/tinylog.h Makefile
	g++ -c $(CPPFLAG) ../tinylog/src/tinylog.cpp -o log.o

test.o : test.cpp Makefile
	g++ -c $(CPPFLAG) test.cpp $(INCLUDE)

bmp.o : bmp.cpp bmp.h Makefile
	g++ -c $(CPPFLAG) bmp.cpp $(INCLUDE)