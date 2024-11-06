###############################################################################
###               University of Hawaii, College of Engineering
###               Lab 4 - memscan - SRE - Spring 2023
###
### Build a memscan program
###
### @see     https://www.gnu.org/software/make/manual/make.html
###
### @file    Makefile
### @author  Vinton Sistoza <sistozav@hawaii.edu>
###############################################################################

TARGET = memscan
all: $(TARGET)

CC     = gcc
CFLAGS = -Wall -Wextra $(DEBUG_FLAGS)

clean:
	rm  -f $(TARGET) *.o

memscan:
		$(CC) $(CFLAGS) -o $(TARGET) memscan.c


test:
	./memscan
