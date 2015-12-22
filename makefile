
all: p2

CC = /usr/local/amiga/bin/m68k-amigaos-gcc.exe
CPP = /usr/local/amiga/bin/m68k-amigaos-g++.exe
NAZWA = jsonMaster
#CC = gcc
#CPP = g++

# z -noixemul nie dziala "%lld"

p1: $(NAZWA).o jsonutil.o json.o utfjson.o ttbitmap.o
	$(CPP) $(NAZWA).o json.o jsonutil.o utfjson.o ttbitmap.o -o $(NAZWA).exe
	
p2: p1
	cp $(NAZWA).exe /cygdrive/p/$(NAZWA)2
#	cp $(NAZWA).exe /cygdrive/d/WinUAE/AmigaSYS/Work/mapnik/a1.out		
	
jsonutil.o: jsonutil.cpp jsonutil.h	
	$(CPP) jsonutil.cpp -c -o jsonutil.o    

json.o: json.c json.h	
	$(CPP) json.c -c -o json.o    

utfjson.o: utfjson.c utfjson.h
	$(CPP) utfjson.c -c -o utfjson.o   

ttbitmap.o: ttbitmap.c ttbitmap.h
	$(CC) ttbitmap.c -c -o ttbitmap.o

$(NAZWA).o: $(NAZWA).c
	$(CC) $(NAZWA).c -noixemul -c -o $(NAZWA).o


	