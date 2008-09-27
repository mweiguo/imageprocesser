CPP = g++
CPPFLAG = -g

all : im.exe

im.exe : test.o bmp.o log.o clip.o funcs.o
	g++ test.o bmp.o log.o clip.o funcs.o -o im.exe

log.o : tinylog.cpp tinylog.h Makefile
	g++ -c $(CPPFLAG) tinylog.cpp -o log.o

test.o : test.cpp Makefile
	g++ -c $(CPPFLAG) test.cpp

bmp.o : bmp.cpp bmp.h Makefile
	g++ -c $(CPPFLAG) bmp.cpp

clip.o : clip.c clip.h Makefile
	g++ -c $(CPPFLAG) clip.c

funcs.o : funcs.c
	g++ -c $(CPPFLAG) funcs.c

clean : 
	-\rm *.o *.exe *~