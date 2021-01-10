#include "../../header_files/SHT/SHT.h"

int SHT_CreateSecondaryIndex(char *sfileName, char *attrName, int attrLength, int buckets, char *fileName)
{
    SHT_info info;
    void *first_block; //first block holds info

    info.attrLength = attrLength;
    info.attrName = attrName;
    info.numBuckets = buckets;
    info.fileName = fileName;

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
    new_block.maxRecords = 512 / (sizeof(Record) - 2 * sizeof(char) - sizeof(int));
    new_block.currRecords = 0;
    new_block.nextBlock = -1;

    for (int i = 1; i <= buckets; i++)
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

SHT_info *SHT_OpenSecondaryIndex(char *sfileName)
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

int SHT_SecondaryInsertEntry(SHT_info *header_info, SecondaryRecord record2)
{
    int key = hashfunction_char(header_info->numBuckets, record2.record.surname);
    void *page;
    SecondaryBlock secondary_block;

     

    if (BF_ReadBlock(header_info->fileDesc, key, &page) < 0) //first page for this key
    {
        BF_PrintError("[!] Error in reading the block in SHT_SecondaryInsertEntry");
        BF_CloseFile(header_info->fileDesc);
        return -1;
    }

    memcpy(&secondary_block, page, sizeof(SecondaryBlock));

    while (secondary_block.nextBlock != -1) //check if there is a chain of blocks and get to the last one
    {
        key = secondary_block.nextBlock;
        if (BF_ReadBlock(header_info->fileDesc, key, &page) < 0)
        {
            BF_PrintError("[!] Error in reading the block in SHT_SecondaryInsertEntry");
            BF_CloseFile(header_info->fileDesc);
            return -1;
        }

        memcpy(&secondary_block, page, sizeof(SecondaryBlock));
    }

    if (secondary_block.maxRecords == secondary_block.currRecords) //block is full
    {
        void *new_page;
        SecondaryBlock new_secondary_block;
        new_secondary_block.maxRecords = 512 / (sizeof(SecondaryRecord) - 2 * sizeof(char) - sizeof(int));
        new_secondary_block.currRecords = 0;
        new_secondary_block.nextBlock = -1;
        new_secondary_block.records[0] = record2;
        new_secondary_block.currRecords++;

        int num_blocks;

        if ((num_blocks = BF_GetBlockCounter(header_info->fileDesc)) < 0)
        {
            BF_PrintError("[!] Error in getting the block counter inside SHT_SecondaryInsertEntry");
            return -1;
        }

        secondary_block.nextBlock = num_blocks;

        memcpy(page, &secondary_block, sizeof(SecondaryBlock));

        if (BF_WriteBlock(header_info->fileDesc, key) < 0)
        {
            BF_PrintError("[!] Error in writing block in SHT_SecondaryInsertEntry");
            BF_CloseFile(header_info->fileDesc);
            return -1;
        }

        if (BF_AllocateBlock(header_info->fileDesc) < 0)
        {
            BF_PrintError("[!] Error in allocating the block in SHT_SecondaryInsertEntry");
            BF_CloseFile(header_info->fileDesc);
            return -1;
        }
        if (BF_ReadBlock(header_info->fileDesc, num_blocks, &new_page) < 0)
        {
            BF_PrintError("[!] Error in reading the block in SHT_SecondaryInsertEntry");
            BF_CloseFile(header_info->fileDesc);
            return -1;
        }

        memcpy(new_page, &new_secondary_block, sizeof(SecondaryBlock));

        if (BF_WriteBlock(header_info->fileDesc, num_blocks) < 0)
        {
            BF_PrintError("[!] Error in writing block in SHT_SecondaryInsertEntry");
            BF_CloseFile(header_info->fileDesc);
            return -1;
        }
    }
    else //block has space left to save new record
    {
        secondary_block.records[secondary_block.currRecords] = record2;
        secondary_block.currRecords++;

        memcpy(page, &secondary_block, sizeof(SecondaryBlock));

        if (BF_WriteBlock(header_info->fileDesc, key) < 0)
        {
            BF_PrintError("[!] Error in writing block in SHT_SecondaryInsertEntry");
            BF_CloseFile(header_info->fileDesc);
            return -1;
        }
    }

    return 0;
}

int SHT_SecondaryGetAllEntries(SHT_info header_info_sht, HT_info header_info_ht, void *value)
{
    int key = hashfunction_char(header_info_ht.buckets, value);
    void *page;
    SecondaryBlock block;

    int blocks_read = 0;

    if (BF_ReadBlock(header_info_ht.fileDesc, key, &page) < 0) //first page for this key
    {
        BF_PrintError("[!] Error in reading the block in HT_CreateFile");
        BF_CloseFile(header_info_ht.fileDesc);
        return -1;
    }

    blocks_read++;

    memcpy(&block, page, sizeof(Block));

    for (int j = 0; j < block.currRecords; j++) //print records
    {
        SecondaryRecord currRecord = block.records[j];
        printf("%d %s %s %s \n", currRecord.record.id, currRecord.record.name, currRecord.record.surname, currRecord.record.address);
    }

    if (block.nextBlock != -1) //there are more pages for this key
    {
        while (block.nextBlock != -1) //until the last linked page
        {
            if (BF_ReadBlock(header_info_ht.fileDesc, block.nextBlock, &page) < 0)
            {
                BF_PrintError("[!] Error in reading the block in HT_CreateFile");
                BF_CloseFile(header_info_ht.fileDesc);
                return -1;
            }

            blocks_read++;

            memcpy(&block, page, sizeof(SecondaryBlock));

            for (int j = 0; j < block.currRecords; j++)
            {
                SecondaryRecord currRecord = block.records[j];
                printf("%d %s %s %s \n", currRecord.record.id, currRecord.record.name, currRecord.record.surname, currRecord.record.address);
            }
        }
    }

    return blocks_read;
}

int hashfunction_char(int numBuckets, void *value)
{
    int a = 678, p = 100019;

    int hashedId = 0;
    char *str = (char *)value;
    while (str[0] != '\0')
    {
        hashedId = (hashedId * a + str[0]) % p;
        str++;
    }
    return (hashedId % numBuckets) + 1; //+1 to avoid having 0 as bucket number (as it is the info bucket)
}