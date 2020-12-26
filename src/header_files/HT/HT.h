#ifndef HT_H
#define HT_H

/* includes needed... */
#include <stdio.h>
#include <sys/types.h>

typedef struct
{
  int id;
  char name[15];
  char surname[25];
  char address[50];
} Record;

typedef struct
{
  int fileDesc;
  char attrType;
  char *attrName;
  int attrLength;
  int buckets;
} HT_info;

typedef struct
{
  char maxRecords;   //char instead of int to save space
  int nextBlock;    //fileDescriptor
  char currRecords;
  Record records[ 512 / (  sizeof(Record) - 2*sizeof(char) - sizeof(int) ) ];
}Block;

int hashfunction(char, int, void *);   //A global hash function for both ints and chars


int HT_CreateIndex(char *, char, char *, int, int);  //Creates and initializes an empty hash file


HT_info *HT_OpenIndex(char *);  //Opens hash file by using the corresponding BF function


int HT_CloseIndex(HT_info *);  //Closes hash file by using the corresponding BF function


int HT_InsertEntry(HT_info *, Record); //calls hash function to calculate key, then finds the last block for this key. 
                                      //If it has space left, the new records is stored there. Otherwise creates a new block


int HT_DeleteEntry(HT_info , void *);  //eliminates record


int HT_GetAllEntries(HT_info , void *);  //call hash function to alculate key and then print each record for this key



int HashStatistics(char *);   //Calculates and prints statistics for hash



void InsertEntries(HT_info *);   //reads file and calls InsertEntry for each line to create new record

#endif /* HT_H */
