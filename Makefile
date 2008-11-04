CPP = g++
CPPFLAG = -g

all : im.exe

im.exe : test.o bmp.o log.o clip.o
	g++ test.o bmp.o log.o clip.o -o im.exe

log.o : tinyLog.cpp stdafx.h Makefile
	g++ -c $(CPPFLAG) tinyLog.cpp -o log.o

test.o : test.cpp stdafx.h BMP.h clip.h Makefile 
	g++ -c $(CPPFLAG) test.cpp

bmp.o : BMP.cpp BMP.h stdafx.h Makefile
	g++ -c $(CPPFLAG) BMP.cpp -o bmp.o

clip.o : clip.c clip.h funcs.c stdafx.h Makefile
	g++ -c $(CPPFLAG) clip.c funcs.c

clean : 
	- rm *.o *.exe *~
