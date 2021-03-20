formula1: main.c config.c sharedmem.c
	gcc -Wall -g -o formula1 main.c config.c sharedmem.c -I.

clean: 
	rm *.o formula1