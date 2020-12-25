#ifndef HP_H
#define HP_H

/* includes needed... */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <time.h>
#include "../../../BF_lib/BF.h"

#define TRUE 1
#define FALSE 0

typedef struct 
{
  int id; // Primary Key.
  char name[15];
  char surname[25];
  char address[50];
} Record;

typedef struct 
{
  int fileDesc;
  char attrType; // acts as field key
  char *attrName; // acts as field key
  int attrLength; // acts as field key
} HP_info;

typedef struct
{
  char maxRecords;   //char instead of int to save space
  int nextBlock;    //fileDescriptor
  char currRecords;
  Record records[ 512 / (  sizeof(Record) - 2*sizeof(char) - sizeof(int) ) ];
}Block;

/*
 * Creates and initializes an empty heapfile
 * On success it returns 0 else it returns 1.
*/
int HP_CreateFile(char *, char, char *, int);

/*
 * Opens the heapfile created and it reads from the 1st block of that file the information regarding the heapfile.
 * It stores all the information needed for the coresponding heapfile to a responsible struct. 
 * If successful it returns the struct else it terminates the program with error code EXIT_FAILURE
*/
HP_info *HP_OpenFile(char *);

/*
 * Closes the heapfile and frees the information that the struct holds for the specific heapfile. 
 * On success it returns 0 else it returns 1
*/
int HP_CloseFile(HP_info *);

/*
 * Inserts a new record at the end of the heapfile. 
 * First it opens the 1st block (information block) and reads the number of the total records. 
 * If no records are inserted at allocates proper space for the new record to be inserted and it inserts it right there. 
 * If records are already inserted it finds how many records the last block has and if the last block has free space it inserts the record right there. In the end we write at the disc all the changes of the last block and we update the total number of the 1st information block and we write that as well at the disc. 
 * On success it returns EXIT_SUCCESS code else it returns -1. 
 * Blocks that have 'installed' records, hold on their first bytes an integer that represents how many records that block contains. 
 * Based on the 68 bytes which is the size of each record and the 4 bytes of the integer, we know that each block can hold 7 records.
 * Why? ~> Block size = 512, (68 * 7) + 4 = 480 
 * But 68 * 8 = 544 > 512, so 7 records for each block.
*/
int HP_InsertEntry(HP_info *, Record);

/*
 * This one does a similar job with the GetAllentries function on the aspect of searching a specific record with a specific given value. (See below for more information on how that is done) 
 * After it finds the record to be deleted it simply makes use of the memset() function and it sets the memory of the record to be deleted to 0 instead of its own memory and that's how if we search for it with the GetAllEntries function we won't find that specific record. 
 * On success it returns 0 else it returns -1
*/
int HP_DeleteEntry(HP_info *, void *);

/*
 * If we pass NULL as the second parameter, all the records will be printed. If we pass a specific key (for example id = 22) it will search for that particular record and it will print it on the tty. 
 * How? 
 * It opens the 1st information block and reads the total number of records. It also finds the total number of blocks. 
 * If the heapfile has no records at all, a coresponding message appears for the user. 
 * How it searches for a specific record: 
 * From the block 1 until the last block: it opens the block, it reads the total number of records it contains, and it checks all of those records. If a record has its key name equal to the value given then that specific record is printed to the tty else we get a message that there are no records with that specific value. The searching is linear and we can see that it gets the work done pretty fast (in < 1secods) even at the 15K dataset. 
*/
int HP_GetAllEntries(HP_info *, void *);

/*
 * Helper function for main.
*/
void InsertEntries(HP_info *);

#endif /* HP_H */