#include "header_files/HT/HT.h"

#define FILENAME "hash_file"

int main(void)
{
    HT_info *info;
    Record record;

    BF_Init();

    if ((HT_CreateIndex(FILENAME, 'i', "id", sizeof(int), 3)) < 0)
    {
        fprintf(stderr, "[!] Error in creating the HT_file in main()\n");
        return -1;
    }

    if ((info = HT_OpenIndex(FILENAME)) == NULL)
    {
        fprintf(stderr, "[!] Error in opening the file in main()\n");
        HT_CloseIndex(info);
        return -1;
    }

  return 0; // program terminated successfully.
}
