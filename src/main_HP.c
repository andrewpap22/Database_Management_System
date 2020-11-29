#include "header_files/HP/HP.h"

#define NAME_SIZE 15
#define SURNAME_SIZE 25
#define ADDRESS_SIZE 50

#define FILENAME "heapfile"

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

  InsertEntries(info);

  return (EXIT_SUCCESS); // program terminated successfully.
}