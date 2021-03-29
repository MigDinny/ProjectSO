formula1: main.c config.c sharedmem.c
	gcc -Wall -pthread -D_REENTRANT -lrt -g -o formula1 include.h main.c config.c sharedmem.c log.c raceman.c breakdownman.c teamman.c car.c
