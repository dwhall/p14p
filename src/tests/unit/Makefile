SHELL = /bin/sh

#CDEFS = -DHEAP_SIZE=0x0D00 
CDEFS = -g -Os -Wall -gstabs -Wstrict-prototypes -Werror -DHEAP_SIZE=0x1000 -DTARGET_DESKTOP -I../../vm -ansi
CFLAGS = -I../../vm $(CDEFS)
CU_SOURCES = CuTest.c AllTests.c
#CU_SOURCES = $(wildcard Cu*.c) AllTests.c
UT_SOURCES = ut_dict.c 
#UT_SOURCES = $(wildcard ut*.c)
ALL_SOURCES = $(CU_SOURCES) $(UT_SOURCES)
OBJS = $(ALL_SOURCES:.c=.o)
PRODUCT = AllTests.out

.PHONY: all check

# Default action is to build and run tests
all : check

check : AllTests.out

$(PRODUCT) : vm $(OBJS)
	$(CC) -o AllTests.out $(OBJS) -L../../vm/ -lpmvm

CuTest.o : CuTest.c
	$(CC) -c -o $@ $<

AllTests.o : AllTests.c
	$(CC) -c -o $@ $<


vm :
	make -C ../../vm

# Removes files made by default make
clean :
	$(RM) $(OBJS)
