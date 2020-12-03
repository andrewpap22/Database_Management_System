#
# In order to execute this "Makefile" just type "make"
#	Dimitra Kousta, Andreas Pappas, :copyright:
# fall 2020-2021, All rights reserved.

OBJS	= ../../src/main_HP.o ../../src/dotC_files/HP/HP.o
SOURCE	= ../../src/main_HP.c ../../src/dotC_files/HP/HP.c
HEADER	= ../../src/header_files/HP/HP.h
OUT	= heapfile
CC	 = gcc
FLAGS	 = -g3 -c -Wall 
LFLAGS	 = ../../BF_lib/BF_64.a -no-pie
# -g option enables debugging mode 
# -c flag generates object code for separate files


all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)


# create/compile the individual files >>separately<<
Project1/src/main_HP.o: ../../src/main_HP.c
	$(CC) $(FLAGS) ../../src/main_HP.c 

Project1/src/dotC_files/HP/HP.o: ../../src/dotC_files/HP/HP.c
	$(CC) $(FLAGS) ../../src/dotC_files/HP/HP.c 


# clean house
clean:
	rm -f $(OBJS) $(OUT)

# run the program
run: $(OUT)
	./$(OUT) < ../../record_examples/records_test.txt

# < ../../record_examples/records1K.txt	

# compile program with debugging information
debug: $(OUT)
	valgrind $(OUT)

# run program with valgrind for errors
valgrind: $(OUT)
	valgrind $(OUT)

# run program with valgrind for leak checks
valgrind_leakcheck: $(OUT)
	valgrind --leak-check=full $(OUT)

# run program with valgrind for leak checks (extreme)
valgrind_extreme: $(OUT)
	valgrind --leak-check=full --show-leak-kinds=all --leak-resolution=high --track-origins=yes --vgdb=yes $(OUT)