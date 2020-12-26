#ifndef SHT_H
#define SHT_H

/* includes needed... */
#include "../../../BF_lib/BF.h"
#include "../HT/HT.h"

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct
{
  int fileDesc;
  char *attrName;
  int attrLength;
  long int numBuckets;
  char *fileName  
} SHT_info;

typedef struct 
{
  Record record;
  int blockId;
}SecondaryRecord;

int SHT_CreateSecondaryIndex(char *, char*, int, int, char *);

SHT_info* SHT_OpenSecondaryIndex(char *);

int SHT_CloseSecondaryIndex(SHT_info *);

int SHT_SecondaryInsertEntry(SHT_info, SecondaryRecord);

int SHT_SecondaryGetAllEntries(SHT_info, HT_info, void *);

#endif /* SHT_H */