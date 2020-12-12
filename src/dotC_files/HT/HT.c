#include "../../header_files/HT/HT.h"
#include "../../../BF_lib/BF.h"

int HT_CreateIndex(char *fileName, char attrType, char *attrName, int attrLength, int buckets)
{
    HT_info info;
    Block *first_block;   //first block holds info

    info.attrLength = attrLength;
    info.attrName = attrName;
    info.attrType = attrType;
    info.buckets = buckets;

    first_block->maxRecords = 512 / (  sizeof(Record) - 2*sizeof(char) - sizeof(int) );
    first_block->currRecords = 0;
    first_block->nextBlock = 1;

    if (BF_CreateFile(fileName) < 0)
    {
        BF_PrintError("[!] Error in creating the file in HT_CreateFile");
        return -1;
    }

    if ((info.fileName = BF_OpenFile(fileName)) < 0)
    {
        BF_PrintError("[!] Error in opening the file in HT_CreateFile");
        return -1;
    }

    if (BF_AllocateBlock(info.fileName) < 0)
    {
        BF_PrintError("[!] Error in allocating the block in HT_CreateFile");
        BF_CloseFile(info.fileName);
        return -1;
    }

    if (BF_ReadBlock(info.fileName, 0, &first_block) < 0)
    {
        BF_PrintError("[!] Error in reading the block in HT_CreateFile");
        BF_CloseFile(info.fileName);
        return -1;
    }

    memcpy(first_block, &info, sizeof(HT_info));

    if (BF_WriteBlock(info.fileName, 0) < 0)
    {
        BF_PrintError("[!] Error in writing to block in HT_CreateFile");
        BF_CloseFile(info.fileName);
        return -1;
    }

    if (BF_CloseFile(info.fileName) < 0)
    {
        BF_PrintError("[!] Error in closing the file in HT_CreateFile");
        return -1;
    }

    //same procedure but for each bucket:

    for(int i = 0; i < buckets; i++)
    {
        Block *new_block;

        if ((info.fileName = BF_OpenFile(fileName)) < 0)
        {
            BF_PrintError("[!] Error in opening the file in HT_CreateFile");
            return -1;
        }

        if (BF_AllocateBlock(info.fileName) < 0)
        {
            BF_PrintError("[!] Error in allocating the block in HT_CreateFile");
            BF_CloseFile(info.fileName);
            return -1;
        }

        if (BF_ReadBlock(info.fileName, 0, &new_block) < 0)
        {
            BF_PrintError("[!] Error in reading the block in HT_CreateFile");
            BF_CloseFile(info.fileName);
            return -1;
        }

        if (BF_WriteBlock(info.fileName, 0) < 0)
        {
            BF_PrintError("[!] Error in writing to block in HT_CreateFile");
            BF_CloseFile(info.fileName);
            return -1;
        }

        if (BF_CloseFile(info.fileName) < 0)
        {
            BF_PrintError("[!] Error in closing the file in HT_CreateFile");
            return -1;
        }
    }

    return 0; //success
}

HT_info *HT_OpenIndex(char *fileName)
{
    HT_info *info;
    Block *block;

    info = malloc(sizeof(HT_info));

    if ((info->fileName = BF_OpenFile(fileName)) < 0)
    {
        BF_PrintError("[!] Error in opening the hash file in HT_OpenFile");
        return -1;
    }

    if (BF_ReadBlock(BF_OpenFile(fileName), 0, &block) < 0)
    {
        BF_PrintError("[!] Error in reading the block in HT_OpenFile");
        BF_CloseFile(info->fileName);
        return -1;
    }

    memcpy(info, block, sizeof(HT_info));

    return info;

}

int HT_CloseIndex(HT_info *header_info)
{
    if (BF_CloseFile(header_info->fileName) < 0)
    {
        BF_PrintError("[!] Error in closing the file in HT_CloseFile");
        return -1;
    }
    free(header_info);

    return 0;
}

int HT_InsertEntry(HT_info *header_info, Record record)
{



    return 0;
}

int hashfunction(char attrType, int numBuckets, void* value)
{
    int a = 678, b = 345, p = 100019;

    if (attrType == 'c')
    {
        int hashedId = 0;
        char* str = (char*)value;
        while (str[0] != '\0')
        {

            hashedId = (hashedId * a + str[0]) % p;
            str++;
        }
        return (hashedId % numBuckets) + 1;   //+1 to exclude having 0 as bucket number (as it is the info bucket)
    }
    else if (attrType == 'i')
    {
        int* id = (int*)value;
        return (((a * b + (*id)) % p) % numBuckets) + 1;
    }
    else
    {
        return -1;
    }
}



