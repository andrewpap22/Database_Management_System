#ifndef HP_H
#define HP_H

/* includes needed... */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
  char attrType;
  char *attrName;
  int attrLength;
} HP_info;


int HP_CreateFile(char *, char, char *, int);

HP_info *HP_OpenFile(char *);

int HP_CloseFile(HP_info *);

int HP_InsertEntry(HP_info, Record);

int HP_DeleteEntry(HP_info, void *);

int HP_GetAllEntries(HP_info, void *);

#endif /* HP_H */
