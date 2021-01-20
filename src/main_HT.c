#include "header_files/HT/HT.h"

#define FILENAME "hash_file.txt"

int main(void)
{
    HT_info *info;

    BF_Init();

    if ((HT_CreateIndex(FILENAME, 'i', "id", sizeof(int), 3)) < 0)
    {
        fprintf(stderr, "[!] Error in creating hash_file in main()\n");
        return -1;
    }

    if ((info = HT_OpenIndex(FILENAME)) == NULL)
    {
        fprintf(stderr, "[!] Error in opening hash_file in main()\n");
        HT_CloseIndex(info);
        return -1;
    }

    InsertEntries(info);

    //int value = NULL;
    int id = 50;
    int blocks_read;
    if ((blocks_read = HT_GetAllEntries(*info, &id)) < 0)
    {
        fprintf(stderr, "[!] Error in getting all the entries in main()\n");
        return -1;
    }

    printf("Blocks read = %d\n", blocks_read);


    if (HT_CloseIndex(info) < 0)
    {
        fprintf(stderr, "[!] Error in closing hashfile in main()\n");
        return -1;
    }

    HashStatistics(FILENAME);

    return 0; // program terminated successfully.
}
