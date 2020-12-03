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

  if ((info = HP_OpenFile(FILENAME)) == NULL)
  {
    fprintf(stderr, "[!] Error in opening the file in main()\n");
    HP_CloseFile(info);
    exit(EXIT_FAILURE);
  }

  // InsertEntries(info);

  while (!feof(stdin))
  {
    scanf("%d %s %s %s", &record.id, record.name, record.surname, record.address);

    // insert records
    if (HP_InsertEntry(info, record) < 0)
    {
      fprintf(stderr, "Error in inserting entry in main.\n");
      HP_CloseFile(info);
      exit(EXIT_FAILURE);
    }
  }

  // close heapfile
  if (HP_CloseFile(info) < 0)
  {
    fprintf(stderr, "Error in closing heapfile in main");
    exit(EXIT_FAILURE);
  }

  // print here...

  return (EXIT_SUCCESS); // program terminated successfully.
}