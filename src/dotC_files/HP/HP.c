#include "../../header_files/HP/HP.h"

int HP_CreateFile(char *fileName, char attrType, char *attrName, int attrLength)
{
  HP_info info;
  void *block;

  info.attrLength = attrLength;
  info.attrName = attrName;
  info.attrType = attrType;

  if (BF_CreateFile(fileName) < 0)
  {
    BF_PrintError("[!] Error in creating the file in HP_CreateFile!");
    return -1;
  }

  if ((info.fileDesc = BF_OpenFile(fileName)) < 0)
  {
    BF_PrintError("[!] Error in opening the file in HP_CreateFile");
    return -1;
  }

  /*
   * Store the HP_info struct for use of the type of file recognition (fileDesc)
  */
  if (BF_AllocateBlock(info.fileDesc) < 0)
  {
    BF_PrintError("[!] Error in allocating the block in HP_CreateFile");
    BF_CloseFile(info.fileDesc);
    return -1;
  }

  if (BF_ReadBlock(info.fileDesc, 0, &block) < 0)
  {
    BF_PrintError("[!] Error in reading the block in HP_CreateFile");
    BF_CloseFile(info.fileDesc);
    return -1;
  }

  memcpy(block, &info, sizeof(HP_info));

  if (BF_WriteBlock(info.fileDesc, 0) < 0)
  {
    BF_PrintError("[!] Error in writing to block in HP_CreateFile");
    BF_CloseFile(info.fileDesc);
    return -1;
  }

  if (BF_CloseFile(info.fileDesc) < 0)
  {
    BF_PrintError("[!] Error in closing the file in HP_CreateFile");
    return -1;
  }

  return (EXIT_SUCCESS); // Everything went smoothly!
}

HP_info *HP_OpenFile(char *fileName)
{
  HP_info *info;
  void *block;

  info = malloc(sizeof(HP_info));

  if ((info->fileDesc = BF_OpenFile(fileName)) < 0)
  {
    BF_PrintError("[!] Error in opening the heap file in HP_OpenFile");
    exit(EXIT_FAILURE);
  }

  if (BF_ReadBlock(BF_OpenFile(fileName), 0, &block) < 0)
  {
    BF_PrintError("[!] Error in reading the block in HP_OpenFile");
    BF_CloseFile(info->fileDesc);
    exit(EXIT_FAILURE);
  }

  memcpy(info, block, sizeof(HP_info));

  return info;
}

int HP_CloseFile(HP_info *header_info)
{
  if (BF_CloseFile(header_info->fileDesc) < 0)
  {
    BF_PrintError("[!] Error in closing the file in HP_CloseFile");
    return -1;
  }
  free(header_info);

  return (EXIT_SUCCESS);
}

int HP_InsertEntry(HP_info *header_info, Record record)
{
  void *block;
  int total_records;
  int records;
  int counter;

  if (BF_ReadBlock(header_info->fileDesc, 0, &block) < 0)
  {
    BF_PrintError("[!] Error in reading block in HP_InsertEntry (1).");
    return -1;
  }

  memcpy(&total_records, block + sizeof(int), sizeof(int));

  if (total_records == 0)
  {
    if (BF_AllocateBlock(header_info->fileDesc) < 0)
    {
      BF_PrintError("[!] Error in allocating block inside HP_InsertEntry (1).");
      return -1;
    }

    if (BF_ReadBlock(header_info->fileDesc, 1, &block) < 0)
    {
      BF_PrintError("[!] Error in reading block inside HP_InsertEntry (2).");
      return -1;
    }
  }
  else
  {
    records = total_records % ((BLOCK_SIZE - sizeof(int)) / (sizeof(Record)));

    if (records == 0)
    {
      if (BF_AllocateBlock(header_info->fileDesc) < 0)
      {
        BF_PrintError("[!] Error in allocating block inside HP_InsertEntry (2).");
        return -1;
      }

      if (BF_ReadBlock(header_info->fileDesc, (BF_GetBlockCounter(header_info->fileDesc) - 1), &block) < 0)
      {
        BF_PrintError("[!] Error in getting block counter inside HP_InsertEntry!");
        return -1;
      }

      counter = 1;

      memcpy(block, &counter, sizeof(int));
      memcpy((block + sizeof(int)), &record, sizeof(Record));
    }
    else
    {
      if (BF_ReadBlock(header_info->fileDesc, (BF_GetBlockCounter(header_info->fileDesc) - 1), &block) < 0)
      {
        BF_PrintError("[!] Error in getting block counter inside HP_InsertEntry! (2)");
        return -1;
      }

      memcpy((block + sizeof(int) + (records * sizeof(Record))), &record, sizeof(Record));
      records++;
      counter = records;
      memcpy(block, &counter, sizeof(int));
    }

    if (BF_ReadBlock(header_info->fileDesc, 0, &block) < 0)
    {
      BF_PrintError("[!] Error in reading block inside HP_InsertEntry (3).");
      return -1;
    }

    total_records++;

    memcpy(block + sizeof(int), &total_records, sizeof(int));

    if (BF_WriteBlock(header_info->fileDesc, 0) < 0)
    {
      BF_PrintError("[!] Error in writing to block inside HP_InsertEntry (1).");
      return -1;
    }

    if (BF_WriteBlock(header_info->fileDesc, (BF_GetBlockCounter(header_info->fileDesc) - 1)) < 0)
    {
      BF_PrintError("[!] Error in writing to block inside HP_InsertEntry (2).");
      return -1;
    }
  }

  return (EXIT_SUCCESS);
}

int HP_DeleteEntry(HP_info *header_info, void *value)
{
}

int HP_GetAllEntries(HP_info *header_info, void *value)
{
  void *block;
  int total_records;
  int total_blocks;
  int rpb; // read per block
  int blocks_read = 0;
  int records_read = 0;
  int found = 0;
  clock_t begin, end;
  double time_spent;
  Record record;

  if (BF_ReadBlock(header_info->fileDesc, 0, &block) < 0)
  {
    BF_PrintError("[!] Error in reading block in HP_GetAllEntries.");
    return -1;
  }

  memcpy(&total_records, (block + sizeof(int)), sizeof(int));

  if ((total_blocks = BF_GetBlockCounter(header_info->fileDesc)) < 0)
  {
    BF_PrintError("[!] Error in getting block counter inside HP_GetAllEntries!");
    return -1;
  }

  printf("\nTotal records: %d - Total blocks: %d\n", total_records, (total_blocks - 1));

  if (total_records == 0)
  {
    printf("The file has not any records yet...\n Please insert some records and try again [1]\n");
    return -1;
  }

  printf("\n |  ID  |      NAME      |       SURNAME       |   ADDRESS            |\n");

  if (value == NULL)
  {
    begin = clock();

    for (int i = 1; i < total_blocks; i++)
    {
      if (BF_ReadBlock(header_info->fileDesc, i, &block) < 0)
      {
        BF_PrintError("[!] Error in reading block inside HP_GetAllEntries (2).");
        return -1;
      }

      memcpy(&rpb, block, sizeof(int));

      for (int j = 0; j < rpb; j++)
      {
        // read records one by one.
        memcpy(&record, (block + sizeof(int) + (j * sizeof(Record))), sizeof(Record));
        records_read++;

        printf(" |%6d|%16s|%21s|%22s|\n", record.id, record.name, record.surname, record.address);

        found++;
      }
      blocks_read++;
    }
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  }
  else
  {
    begin = clock();
    for (int i = 1; i < total_blocks; i++)
    {
      if (BF_ReadBlock(header_info->fileDesc, i, &block) < 0)
      {
        BF_PrintError("[!] Error in reading block inside HP_GetAllEntries (3).");
        return -1;
      }

      memcpy(&rpb, block, sizeof(int));

      for (int j = 0; j < rpb; j++)
      {
        memcpy(&record, (block + sizeof(int) + (j * sizeof(Record))), sizeof(Record));
        records_read++;

        // check if there exists a record based on the field keys.
        if ((record.id == *(int *)value) || (!strcmp(record.name, value)) || (!strcmp(record.surname, value)) || (!strcmp(record.address, value)))
        {
          printf(" |%6d|%16s|%21s|%22s|\n", record.id, record.name, record.surname, record.address);

          found++;
        }
      }
      blocks_read++;
    }
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    if (found == 0)
    {
      printf("There are no results with this value. \n Please try again.\n");
    }
  }
  printf("\n\n					***********HEAP/READ STATISTICS************\n");
  printf("					*                                         *\n");
  printf("					* | Records |  Blocks |   Time  | Found  |*\n");
  printf("					* |%9d|%9d|%9f|%8d|*\n", records_read, blocks_read, time_spent, found);
  printf("					*                                         *\n");
  printf("					*******************************************\n\n");
  return 0;
}

/*
 * Helper function for main.
*/
void InsertEntries(HP_info *info)
{
  FILE *FP;
  char *line = NULL;
  size_t length = 0;
  ssize_t read; // size type with an error value (-1)
  FP = stdin;
  Record record;

  while ((read = getline(&line, &length, FP)) != -1)
  {
    line[read - 2] = 0;
    char *tmp;

    tmp = strtok(line, ",");
    record.id = atoi(tmp);

    tmp = strtok(NULL, ",");
    tmp++;
    tmp[strlen(tmp) - 1] = '\0';
    strncpy(record.name, tmp, sizeof(record.name));

    tmp = strtok(NULL, ",");
    tmp++;
    tmp[strlen(tmp) - 1] = '\0';
    strncpy(record.surname, tmp, sizeof(record.surname));

    tmp = strtok(NULL, ",");
    tmp++;
    tmp[strlen(tmp) - 1] = '\0';
    strncpy(record.address, tmp, sizeof(record.address));
    
    assert(!HP_InsertEntry(info, record));
  }
  free(line);
}