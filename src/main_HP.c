#include "header_files/HP/HP.h"

#define FILENAME "heap_file"

int main(void)
{  

  HP_info *info;
  Record record;

  BF_Init();

  /*
   * Create Heap File 
  */
  if ((HP_CreateFile(FILENAME, 'i', "id", sizeof(int))) < 0)
  {
    fprintf(stderr, "[!] Error in creating the heapfile in main()\n");
    exit(EXIT_FAILURE);
  }

  /*
   * Open Heap File 
  */
  if ((info = HP_OpenFile(FILENAME)) == NULL)
  {
    fprintf(stderr, "[!] Error in opening the file in main()\n");
    HP_CloseFile(info);
    exit(EXIT_FAILURE);
  }

  /*
   * Insert entries.
  */
  InsertEntries(info);

  // while (!feof(stdin))
  // {
  //   scanf("%d %s %s %s", &record.id, record.name, record.surname, record.address);

  //   /*
  //   * Insert entries. 
  //   */
  //   if (HP_InsertEntry(info, record) < 0)
  //   {
  //     fprintf(stderr, "[!] Error in inserting entry in main.\n");
  //     HP_CloseFile(info);
  //     exit(EXIT_FAILURE);
  //   }
  // }

  /*
   * Print Entries.
  */
  if (HP_GetAllEntries(info, NULL) < 0)
  {
    fprintf(stderr, "[!] Error in getting all the entries in main()\n");
    exit(EXIT_FAILURE);
  }

  /*
   * Search and print specific entry
  */
  int id = 22;

  printf("\nSearching the record with id = 22 ...\n");
  if (HP_GetAllEntries(info, &id) < 0)
  {
    fprintf(stderr, "[!] Error in getting all the entries in main()\n");
    exit(EXIT_FAILURE);
  }

  /*
   * Close heapfile
  */
  if (HP_CloseFile(info) < 0)
  {
    fprintf(stderr, "[!] Error in closing heapfile in main()\n");
    exit(EXIT_FAILURE);
  }

  return (EXIT_SUCCESS); // program terminated successfully.
}