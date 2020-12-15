#include "header_files/HT/HT.h"

#define FILENAME "hash_file"

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

    int value = 3;
    int blocks_read;
    if ((blocks_read = HT_GetAllEntries(*info, &value)) < 0)
    {
        fprintf(stderr, "[!] Error in getting all the entries in main()\n");
        return -1;
    }

    printf("Blocks read = %d\n", blocks_read);

//    FILE* readFile;
//    readFile = fopen("records1K.txt", "r");
//
//    char strToInt[100];
//
//    for (int i = 0; i < 1000; i++)
//    {
//        int j = 0;
//        char c = fgetc(readFile);
//        c = fgetc(readFile);
//        while (c != ',')
//        {
//
//            strToInt[j] = c;
//            c = fgetc(readFile);
//            j++;
//        }
//
//        strToInt[j] = '\0';
//        record.id = atoi(strToInt);
//
//        j = 0;
//        c = fgetc(readFile);
//        c = fgetc(readFile);
//
//        while (c != '"')
//        {
//
//            record.name[j] = c;
//            c = fgetc(readFile);
//            j++;
//        }
//        record.name[j] = '\0';
//
//        j = 0;
//        c = fgetc(readFile);
//        c = fgetc(readFile);
//        c = fgetc(readFile);
//
//        while (c != '"')
//        {
//
//            record.surname[j] = c;
//            c = fgetc(readFile);
//            j++;
//        }
//        record.surname[j] = '\0';
//        j = 0;
//        c = fgetc(readFile);
//        c = fgetc(readFile);
//        c = fgetc(readFile);
//        while (c != '"')
//        {
//            record.address[j] = c;
//            c = fgetc(readFile);
//            j++;
//        }
//        record.address[j] = '\0';
//
//        while (c != '\n')
//        {
//            c = fgetc(readFile);
//        }
//
//        HT_InsertEntry(info, record);
//
//        printf("Id: %d Name: %s Surname: %s Address: %s \n", record.id, record.name, record.surname, record.address);
//    }



//    int value = 3;
//    HT_GetAllEntries(*info, &value);

    if (HT_CloseIndex(info) < 0)
    {
        fprintf(stderr, "[!] Error in closing hashfile in main()\n");
        return -1;
    }

    return 0; // program terminated successfully.
}
