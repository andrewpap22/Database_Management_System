#ifndef HT_H
#define HT_H

/* includes needed... */
#include <stdio.h>

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

int hashfunction(char, int, void *);

int HT_CreateIndex(char *, char, char *, int, int);
HT_info *HT_OpenIndex(char *);
int HT_CloseIndex(HT_info *);
int HT_InsertEntry(HT_info *, Record);
int HT_DeleteEntry(HT_info , void *);
int HT_GetAllEntries(HT_info , void *);
int HashStatistics(char *);
void InsertEntries(HT_info *);

#endif /* HT_H */
