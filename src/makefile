#
# In order to execute this "Makefile" just type "make"
#	Dimitra Kousta, Andreas Pappas, :copyright:
# fall 2020-2021, All rights reserved.

OBJS	= main.o dotC_files/test.o
SOURCE	= main.c dotC_files/test.c
HEADER	= header_files/test.h
OUT	= project1
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 =
# -g option enables debugging mode
# -c flag generates object code for separate files


all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)


# create/compile the individual files >>separately<<
main.o: main.c
	$(CC) $(FLAGS) main.c

src/dotC_files/test.o: dotC_files/test.c
	$(CC) $(FLAGS) dotC_files/test.c

# simply copy paste the lines 25-26 to compile more files on dorC_files directory.

# clean house
clean:
	rm -f $(OBJS) $(OUT)
