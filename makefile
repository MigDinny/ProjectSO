formula1: main.c config.c sharedmem.c
	gcc -Wall -pthread -lrt -g -o formula1 main.c config.c sharedmem.c log.c raceman.c breakdownman.c teamman.c car.c -I.
