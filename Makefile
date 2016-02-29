ifeq ($(DEBUG), 1)
CFLAGS = -v -g -Wall
	else
CFLAGS = -O3
	endif

all:
	@echo "Debug mode : $(DEBUG)"
	gcc $(CFLAGS) -std=c99 sum.c -o sum -lmpfr

