
all: p2

CC = /usr/local/amiga/bin/m68k-amigaos-gcc.exe
CPP = /usr/local/amiga/bin/m68k-amigaos-g++.exe
NAZWA = jsonMaster
#CC = gcc
#CPP = g++

# z -noixemul nie dziala "%lld"

p1: $(NAZWA).o jsonutil.o json.o utfjson.o ttbitmap.o wingui.o
	$(CPP) -s $(NAZWA).o json.o jsonutil.o utfjson.o ttbitmap.o wingui.o -o $(NAZWA).exe
	
p2: p1
	rm /cygdrive/p/$(NAZWA)2	
	cp $(NAZWA).exe /cygdrive/p/$(NAZWA)2
	cp $(NAZWA).exe /cygdrive/d/WinUAE/AmigaSYS/Work/mapnik/$(NAZWA)2
	#cp \\N111-0101\$(ES)\V\_A R C H I W U M\WinUAE\AmigaSYS\Work\$(NAZWA)2	
	
jsonutil.o: jsonutil.cpp jsonutil.h	
	$(CPP) jsonutil.cpp -c -o jsonutil.o    

json.o: json.c json.h	
	$(CPP) json.c -c -o json.o    

utfjson.o: utfjson.c utfjson.h
	$(CC) utfjson.c -c -o utfjson.o   

ttbitmap.o: ttbitmap.c ttbitmap.h
	$(CC) ttbitmap.c -c -o ttbitmap.o
	
wingui.o:   wingui.c wingui.h jsonMaster.h
	$(CC) wingui.c -c -o wingui.o

$(NAZWA).o: $(NAZWA).c $(NAZWA).h jsonMaster.h
	$(CC) $(NAZWA).c -noixemul -c -o $(NAZWA).o


	