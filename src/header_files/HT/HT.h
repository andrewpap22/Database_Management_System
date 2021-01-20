#ifndef HT_H
#define HT_H

/* includes needed... */
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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
  char maxRecords; //char instead of int to save space
  int nextBlock;   //fileDescriptor
  char currRecords;
  Record records[512 / (sizeof(Record) - 2 * sizeof(char) - sizeof(int))];
} Block;

typedef struct
{
  int fileDesc;
  char *attrName;
  int attrLength;
  long int numBuckets;
  char *fileName;
} SHT_info;

typedef struct
{
  Record record;
  int blockId;
} SecondaryRecord;

typedef struct
{
  char data[40];
  int blockId;
} SRecord; //a structure that has the needed information to search a record from SHT to HT

typedef struct
{
  char maxRecords; //char instead of int to save space
  int nextBlock;   //fileDescriptor
  char currRecords;
  SRecord records[512 / (sizeof(SRecord) - 2 * sizeof(char) - sizeof(int))];
} SecondaryBlock;

int hashfunction(char, int, void *); //A global hash function for both ints and chars

int HT_CreateIndex(char *, char, char *, int, int); //Creates and initializes an empty hash file

HT_info *HT_OpenIndex(char *); //Opens hash file by using the corresponding BF function

int HT_CloseIndex(HT_info *); //Closes hash file by using the corresponding BF function

int HT_InsertEntry(HT_info *, Record); //calls hash function to calculate key, then finds the last block for this key.
                                       //If it has space left, the new records is stored there. Otherwise creates a new block

int HT_DeleteEntry(HT_info, void *); //eliminates record

int HT_GetAllEntries(HT_info, void *); //call hash function to calculate key and then print each record for this key

int HashStatistics(char *); //Calculates and prints statistics for HT

void SHT_InsertEntries(HT_info *, SHT_info *); //reads file and calls InsertEntry for each line to create new record

int SHT_CreateSecondaryIndex(char *, char *, int, long int, char *); //Creates and initializes an empty secondary hash file

SHT_info *SHT_OpenSecondaryIndex(char *); //Opens secondary hash file by using the corresponding BF function

int SHT_CloseSecondaryIndex(SHT_info *); //Closes secondary hash file by using the corresponding BF function

int SHT_SecondaryInsertEntry(SHT_info *, SecondaryRecord); //Gets a SecondaryRecord that holds a record and a block id. Creates an SRecord where it keeps the data from the record and the id
                                                           //Stores the SRecord at secondary hash table

int SHT_SecondaryGetAllEntries(SHT_info, HT_info, void *); //It searches and finds the SRecord with the same data as value and uses the blockId to find the block of the primary hash table
                                                           //that has the record and then it prints it

int hashfunction_char(int, void *);

int SHT_HashStatistics(char *); //Calculates and prints statistics fot SHT

#endif /* HT_H */