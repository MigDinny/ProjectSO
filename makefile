formula1: main.c config.c sharedmem.c
	gcc -Wall -pthread -lrt -g -o formula1 main.c config.c sharedmem.c raceman.c breakdownman.c -I.