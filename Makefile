all : test main

test : test.o structures.o memory.o ipcTools.o
	gcc test.o structures.o memory.o ipcTools.o -o test 

main : main.o structures.o memory.o ipcTools.o
	gcc main.o structures.o memory.o ipcTools.o -o main 

main.o : main.c ./utils/memory.h
	gcc -c main.c

test.o: test.c ./utils/memory.h
	gcc -c test.c

memory.o: ./utils/memory.c ./utils/memory.h ./utils/structures.h ./utils/ipcTools.h
	gcc -c ./utils/memory.c 

ipcTools.o: ./utils/ipcTools.c ./utils/ipcTools.h
	gcc -c ./utils/ipcTools.c

structures.o: ./utils/structures.c ./utils/structures.h
	gcc -c ./utils/structures.c

doc :
	doxygen Doxyfile

clear :
	rm -f *.o test main