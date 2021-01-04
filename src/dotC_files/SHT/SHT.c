#include "../../header_files/SHT/SHT.h"

int SHT_CreateSecondaryIndex(char *sfileName, char *attrName, int attrLength, int buckets, char *fileName)
{
    SHT_info info;
    void *first_block;   //first block holds info

    info.attrLength = attrLength;
    info.attrName = attrName;
    info.numBuckets = buckets;

    if (BF_CreateFile(sfileName) < 0)
    {
        BF_PrintError("[!] Error in creating the file in HT_CreateFile");
        return -1;
    }

    if ((info.fileDesc = BF_OpenFile(sfileName)) < 0)
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

    void *new_page;
    SecondaryBlock new_block;
    new_block.maxRecords = 512 / (  sizeof(Record) - 2*sizeof(char) - sizeof(int) );
    new_block.currRecords = 0;
    new_block.nextBlock = -1;

    for(int i = 1; i <= buckets; i++)
    {

        if ((info.fileDesc = BF_OpenFile(sfileName)) < 0)
        {
            BF_PrintError("[!] Error in opening the file in SHT_CreateFile");
            return -1;
        }

        if (BF_AllocateBlock(info.fileDesc) < 0)
        {
            BF_PrintError("[!] Error in allocating the block in SHT_CreateFile");
            BF_CloseFile(info.fileDesc);
            return -1;
        }

        if (BF_ReadBlock(info.fileDesc, i, &new_page) < 0)
        {
            BF_PrintError("[!] Error in reading the block in SHT_CreateFile");
            BF_CloseFile(info.fileDesc);
            return -1;
        }

        memcpy(new_page, &new_block, sizeof(SHT_info));

        if (BF_WriteBlock(info.fileDesc, i) < 0)
        {
            BF_PrintError("[!] Error in writing to block in SHT_CreateFile");
            BF_CloseFile(info.fileDesc);
            return -1;
        }

        if (BF_CloseFile(info.fileDesc) < 0)
        {
            BF_PrintError("[!] Error in closing the file in SHT_CreateFile");
            return -1;
        }
    }

    return 0; //success

}

SHT_info* SHT_OpenSecondaryIndex(char *sfileName)
{
    SHT_info *info;
    SecondaryBlock *block;

    info = malloc(sizeof(HT_info));

    if ((info->fileDesc = BF_OpenFile(sfileName)) < 0)
    {
        BF_PrintError("[!] Error in opening the hash file in SHT_OpenFile");
        return -1;
    }

    if (BF_ReadBlock(BF_OpenFile(sfileName), 0, &block) < 0)
    {
        BF_PrintError("[!] Error in reading the block in SHT_OpenFile");
        BF_CloseFile(info->fileDesc);
        return -1;
    }

    memcpy(info, block, sizeof(SHT_info));

    return info;
}

int SHT_CloseSecondaryIndex(SHT_info *header_info)
{
    if (BF_CloseFile(header_info->fileDesc) < 0)
    {
        BF_PrintError("[!] Error in closing the file in HT_CloseFile");
        return -1;
    }
    free(header_info);

    return 0;
}

int hashfunction(int numBuckets, char* value)
{
    int a = 678, p = 100019;

    int hashedId = 0;
    char* str = (char*)value;
    while (str[0] != '\0')
    {
        hashedId = (hashedId * a + str[0]) % p;
        str++;
    }
    return (hashedId % numBuckets) + 1;   //+1 to avoid having 0 as bucket number (as it is the info bucket)
}

int SHT_SecondaryInsertEntry(SHT_info header_info, SecondaryRecord record)
{

}

int SHT_SecondaryGetAllEntries(SHT_info header_info_sht, HT_info header_info_ht, void *value)
{
    int key = hashfunction(info.buckets, value);
    void* page;
    SecondaryBlock block;

    int blocks_read = 0;

    if (BF_ReadBlock(info.fileDesc, key, &page) < 0)   //first page for this key
    {
        BF_PrintError("[!] Error in reading the block in HT_CreateFile");
        BF_CloseFile(info.fileDesc);
        return -1;
    }

    blocks_read++;

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

            blocks_read++;

            memcpy(&block, page, sizeof(SecondaryBlock));

            for(int j = 0; j < block.currRecords; j++)
            {
                Record currRecord = block.records[j];
                printf("%d %s %s %s \n", currRecord.id, currRecord.name, currRecord.surname, currRecord.address);
            }
        }
    }

    return blocks_read;
}