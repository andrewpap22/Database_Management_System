#include "../../header_files/HT/HT.h"
#include "../../../BF_lib/BF.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

int HT_CreateIndex(char *fileName, char attrType, char *attrName, int attrLength, int buckets)
{
    HT_info info;
    void *first_block;   //first block holds info

    info.attrLength = attrLength;
    info.attrName = attrName;
    info.attrType = attrType;
    info.buckets = buckets;

//    first_block->maxRecords = 512 / (  sizeof(Record) - 2*sizeof(char) - sizeof(int) );
//    first_block->currRecords = 0;
//    first_block->nextBlock = 1;

    if (BF_CreateFile(fileName) < 0)
    {
        BF_PrintError("[!] Error in creating the file in HT_CreateFile");
        return -1;
    }

    if ((info.fileDesc = BF_OpenFile(fileName)) < 0)
    {
        BF_PrintError("[!] Error in opening the file in HT_CreateFile");
        return -1;
    }

    if (BF_AllocateBlock(info.fileDesc) < 0)
    {
        BF_PrintError("[!] Error in allocating the block in HT_CreateFile");
        BF_CloseFile(info.fileDesc);
        return -1;
    }

    if (BF_ReadBlock(info.fileDesc, 0, &first_block) < 0)
    {
        BF_PrintError("[!] Error in reading the block in HT_CreateFile");
        BF_CloseFile(info.fileDesc);
        return -1;
    }

    memcpy(first_block, &info, sizeof(HT_info));

    if (BF_WriteBlock(info.fileDesc, 0) < 0)
    {
        BF_PrintError("[!] Error in writing to block in HT_CreateFile");
        BF_CloseFile(info.fileDesc);
        return -1;
    }

    if (BF_CloseFile(info.fileDesc) < 0)
    {
        BF_PrintError("[!] Error in closing the file in HT_CreateFile");
        return -1;
    }

    //same procedure but for each bucket:

    void *new_page;
    Block new_block;
    new_block.maxRecords = 512 / (  sizeof(Record) - 2*sizeof(char) - sizeof(int) );
    new_block.currRecords = 0;
    new_block.nextBlock = -1;

    for(int i = 1; i <= buckets; i++)
    {

        if ((info.fileDesc = BF_OpenFile(fileName)) < 0)
        {
            BF_PrintError("[!] Error in opening the file in HT_CreateFile");
            return -1;
        }

        if (BF_AllocateBlock(info.fileDesc) < 0)
        {
            BF_PrintError("[!] Error in allocating the block in HT_CreateFile");
            BF_CloseFile(info.fileDesc);
            return -1;
        }

        if (BF_ReadBlock(info.fileDesc, i, &new_page) < 0)
        {
            BF_PrintError("[!] Error in reading the block in HT_CreateFile");
            BF_CloseFile(info.fileDesc);
            return -1;
        }

        memcpy(new_page, &new_block, sizeof(HT_info));

        if (BF_WriteBlock(info.fileDesc, i) < 0)
        {
            BF_PrintError("[!] Error in writing to block in HT_CreateFile");
            BF_CloseFile(info.fileDesc);
            return -1;
        }

        if (BF_CloseFile(info.fileDesc) < 0)
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

    if ((info->fileDesc = BF_OpenFile(fileName)) < 0)
    {
        BF_PrintError("[!] Error in opening the hash file in HT_OpenFile");
        return -1;
    }

    if (BF_ReadBlock(BF_OpenFile(fileName), 0, &block) < 0)
    {
        BF_PrintError("[!] Error in reading the block in HT_OpenFile");
        BF_CloseFile(info->fileDesc);
        return -1;
    }

    memcpy(info, block, sizeof(HT_info));

    return info;

}

int HT_CloseIndex(HT_info *header_info)
{
    if (BF_CloseFile(header_info->fileDesc) < 0)
    {
        BF_PrintError("[!] Error in closing the file in HT_CloseFile");
        return -1;
    }
    free(header_info);

    return 0;
}

int HT_InsertEntry(HT_info *header_info, Record record)
{
    int key = hashfunction(header_info->attrType, header_info->buckets, &record.id);
    void *page;
    Block block;

    if (BF_ReadBlock(header_info->fileDesc, key, &page) < 0)  //first page for this key
    {
        BF_PrintError("[!] Error in reading the block in HT_CreateFile");
        BF_CloseFile(header_info->fileDesc);
        return -1;
    }

    memcpy(&block, page, sizeof(Block));

    while(block.nextBlock != -1)  //check if there is a chain of blocks and get to the last one
    {
        key = block.nextBlock;
        if (BF_ReadBlock(header_info->fileDesc, key, &page) < 0)
        {
            BF_PrintError("[!] Error in reading the block in HT_CreateFile");
            BF_CloseFile(header_info->fileDesc);
            return -1;
        }

        memcpy(&block, page, sizeof(Block));
    }

    if(block.maxRecords == block.currRecords)   //block is full
    {
        void *new_page;
        Block new_block;
        new_block.maxRecords = 512 / (  sizeof(Record) - 2*sizeof(char) - sizeof(int) );
        new_block.currRecords = 0;
        new_block.nextBlock = -1;
        new_block.records[0] = record;
        new_block.currRecords++;

        int num_blocks;

        if ((num_blocks = BF_GetBlockCounter(header_info->fileDesc)) < 0)
        {
            BF_PrintError("[!] Error in getting the block counter inside HP_InsertEntry");
            return -1;
        }

        block.nextBlock = num_blocks;

        memcpy(page, &block, sizeof(Block));

        if (BF_WriteBlock(header_info->fileDesc, key) < 0)
        {
            BF_PrintError("[!] Error in writing block in HT_CreateFile");
            BF_CloseFile(header_info->fileDesc);
            return -1;
        }


        if (BF_AllocateBlock(header_info->fileDesc) < 0)
        {
            BF_PrintError("[!] Error in allocating the block in HT_CreateFile");
            BF_CloseFile(header_info->fileDesc);
            return -1;
        }
        if (BF_ReadBlock(header_info->fileDesc, num_blocks, &new_page) < 0)
        {
            BF_PrintError("[!] Error in reading the block in HT_CreateFile");
            BF_CloseFile(header_info->fileDesc);
            return -1;
        }

        memcpy(new_page, &new_block, sizeof(Block));

        if (BF_WriteBlock(header_info->fileDesc, num_blocks) < 0)
        {
            BF_PrintError("[!] Error in writing block in HT_CreateFile");
            BF_CloseFile(header_info->fileDesc);
            return -1;
        }

    }
    else   //block has space left to save new record
    {
        block.records[block.currRecords] = record;
        block.currRecords++;

        memcpy(page, &block, sizeof(Block));

        if (BF_WriteBlock(header_info->fileDesc, key) < 0)
        {
            BF_PrintError("[!] Error in writing block in HT_CreateFile");
            BF_CloseFile(header_info->fileDesc);
            return -1;
        }
    }

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
        return (hashedId % numBuckets) + 1;   //+1 to avoid having 0 as bucket number (as it is the info bucket)
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

int HT_GetAllEntries(HT_info info, void *value)
{
    int key = hashfunction(info.attrType, info.buckets, value);
    void* page;
    Block block;

    if (BF_ReadBlock(info.fileDesc, key, &page) < 0)   //first page for this key
    {
        BF_PrintError("[!] Error in reading the block in HT_CreateFile");
        BF_CloseFile(info.fileDesc);
        return -1;
    }

    memcpy(&block, page, sizeof(Block));

    for(int j = 0; j < block.currRecords; j++)   //print records
    {
        Record currRecord = block.records[j];
        printf("%d %s %s %s \n", currRecord.id, currRecord.name, currRecord.surname, currRecord.address);
    }

    if(block.nextBlock != -1)   //there are more pages for this key
    {
        while(block.nextBlock != -1)  //until the last linked page
        {
            if (BF_ReadBlock(info.fileDesc, block.nextBlock, &page) < 0)
            {
                BF_PrintError("[!] Error in reading the block in HT_CreateFile");
                BF_CloseFile(info.fileDesc);
                return -1;
            }

            memcpy(&block, page, sizeof(Block));

            for(int j = 0; j < block.currRecords; j++)
            {
                Record currRecord = block.records[j];
                printf("%d %s %s %s \n", currRecord.id, currRecord.name, currRecord.surname, currRecord.address);
            }

        }

    }

    return 0;

}

void InsertEntries(HT_info *info)
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
        line++;
        char *tmp;

        tmp = strtok(line, ",");
        record.id = atoi(tmp);
        printf("id = %d \n", record.id);

        tmp = strtok(NULL, ",");
        tmp++;
        tmp[strlen(tmp) - 1] = '\0';
        strncpy(record.name, tmp, sizeof(record.name));
        printf("name = %s \n", record.name);

        tmp = strtok(NULL, ",");
        tmp++;
        tmp[strlen(tmp) - 1] = '\0';
        strncpy(record.surname, tmp, sizeof(record.surname));
        printf("sname = %s \n", record.surname);


        tmp = strtok(NULL, ",");
        tmp++;
        tmp[strlen(tmp) - 1] = '\0';
        strncpy(record.address, tmp, sizeof(record.address));
        printf("addr = %s \n", record.address);


        assert(!HT_InsertEntry(info, record));
    }
    free(line);
}



