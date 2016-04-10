#
# Students' Makefile for the Malloc Lab
#
VERSION = 1

CC = gcc
CFLAGS = -Wall -O3 -Werror -m32 -pthread -std=gnu11
# for debugging
#CFLAGS = -Wall -g -Werror -m32 -pthread -std=gnu11

SHARED_OBJS = mdriver.o memlib.o fsecs.o fcyc.o clock.o ftimer.o list.o
OBJS = $(SHARED_OBJS) mm.o
MTOBJS = $(SHARED_OBJS) mmts.o
BOOK_IMPL_OBJS = $(SHARED_OBJS) mm-book-implicit.o
GBACK_IMPL_OBJS = $(SHARED_OBJS) mm-gback-implicit.o

all: mdriver mdriver-ts

mdriver: $(OBJS)
	$(CC) $(CFLAGS) -o mdriver $(OBJS)

mdriver-ts: $(MTOBJS)
	$(CC) $(CFLAGS) -o mdriver-ts $(MTOBJS)

mdriver-book: $(BOOK_IMPL_OBJS)
	$(CC) $(CFLAGS) -o $@ $(BOOK_IMPL_OBJS)

mdriver-gback: $(GBACK_IMPL_OBJS)
	$(CC) $(CFLAGS) -o $@ $(GBACK_IMPL_OBJS)

mdriver.o: mdriver.c fsecs.h fcyc.h clock.h memlib.h config.h mm.h
memlib.o: memlib.c memlib.h config.h
mm.o: mm.c mm.h memlib.h
mmts.o: mm.c mm.h memlib.h
	$(CC) $(CFLAGS) -DTHREAD_SAFE=1 -c mm.c -o mmts.o

fsecs.o: fsecs.c fsecs.h config.h
fcyc.o: fcyc.c fcyc.h
ftimer.o: ftimer.c ftimer.h config.h
clock.o: clock.c clock.h
list.o: list.c list.h

handin:
	/home/courses/cs3214/bin/submit.pl p3 mm.c

clean:
	rm -f *~ *.o mdriver


