CC = gcc
CFLAGS = -lm -lSDL2 

raytracing: raytracing.c
	$(CC) -o raytracing raytracing.c $(CFLAGS)
