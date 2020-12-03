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
  int no_of_blocks;
  int no_of_records = 0;

  if ((no_of_blocks = BF_GetBlockCounter(header_info->fileDesc)) < 0)
  {
    BF_PrintError("[!] Error in getting the block counter inside HP_InsertEntry");
    return -1;
  }
  else if (no_of_blocks == 1)
  {
    if (BF_AllocateBlock(header_info->fileDesc) < 0)
    {
      BF_PrintError("[!] Error in allocating the block inside HP_InsertEntry (1)");
      BF_CloseFile(header_info->fileDesc);
      return -1;
    }

    no_of_blocks = 2;
  }

  if (BF_ReadBlock(header_info->fileDesc, no_of_blocks - 1, &block) < 0)
  {
    BF_PrintError("[!] Error in getting the block inside HP_InsertEntry");
    return -1;
  }
  memcpy(&no_of_records, block, sizeof(int));

  if (++no_of_records > (int)((BLOCK_SIZE - sizeof(int)) / sizeof(Record)))
  {
    if (BF_AllocateBlock(header_info->fileDesc) < 0)
    {
      BF_PrintError("[!] Error in allocating the block inside HP_InsertEntry (2)");
      BF_CloseFile(header_info->fileDesc);
      return -1;
    }

    ++no_of_blocks;
  }

  if (BF_ReadBlock(header_info->fileDesc, no_of_blocks - 1, &block) < 0)
  {
    BF_PrintError("[!] Error in reading the block inside HP_Insertentry");
    return -1;
  }

  memcpy(block, &no_of_records, sizeof(int));
  memcpy(block + sizeof(int) + (no_of_records * sizeof(Record)), &record, sizeof(Record));

  if (BF_WriteBlock(header_info->fileDesc, no_of_blocks - 1) < 0)
  {
    BF_PrintError("[!] Error in writing to block inside HP_InsertEntry");
    BF_CloseFile(header_info->fileDesc);
    return -1;
  }

  return (EXIT_SUCCESS);
}

int HP_DeleteEntry(HP_info *header_info, void *value)
{
}

int HP_GetAllEntries(HP_info *header_info, void *value)
{
  void *block;
  Record record;
  int no_of_blocks, no_of_records;
  int found = FALSE;
  clock_t start, end;

  if ((no_of_blocks = BF_GetBlockCounter(header_info->fileDesc)) < 0)
  {
    BF_PrintError("[!] error in getting the block counter inside HP_GetAllEntries");
    return -1;
  }

  if (value == NULL)
  {
    start = clock();
    for (int i = 1; i < no_of_blocks; i++)
    {
      if (BF_ReadBlock(header_info->fileDesc, i, &block) < 0)
      {
        BF_PrintError("[!] Error in reading block inside HP_GetAllEntries.");
        return -1;
      }
      // reading the number of records per block.
      memcpy(&no_of_records, block, sizeof(int));
      for (int j = 1; j <= no_of_records; j++)
      {
        // reading the records one by one
        memcpy(&record, block + sizeof(int) + (j * sizeof(Record)), sizeof(Record));

        printf("%d,\n%s,\n%s,\n%s\n\n", record.id, record.name, record.surname, record.address);
        printf("%d blocks were read...\n",i);

        found = TRUE;
      }
      if (found == TRUE)
      {
        break;
      }
    }
    end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Total time spent: %lf\n", time_spent);
  }
  // else
  // {

  // }
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
    tmp[strlen(tmp) - 1] = 0;
    strncpy(record.name, tmp, sizeof(record.name));

    tmp = strtok(NULL, ",");
    tmp++;
    tmp[strlen(tmp) - 1] = 0;
    strncpy(record.surname, tmp, sizeof(record.surname));

    tmp = strtok(NULL, ",");
    tmp++;
    tmp[strlen(tmp) - 1] = 0;
    strncpy(record.address, tmp, sizeof(record.address));

    assert(!HP_InsertEntry(info, record));
  }
  free(line);
}