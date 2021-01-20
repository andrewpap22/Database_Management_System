#include "../../header_files/SHT/SHT.h"
#include "../../dotC_files/HT/HT.c"

int SHT_CreateSecondaryIndex(char *sfileName, char *attrName, int attrLength, long int buckets, char *fileName)
{
    SHT_info info;
    void *first_block; //first block holds info

    info.attrLength = attrLength;
    info.attrName = attrName;
    info.numBuckets = buckets;
    info.fileName = fileName;

    if (BF_CreateFile(sfileName) < 0)
    {
        BF_PrintError("[!] Error in creating the file in SHT_CreateSecondaryIndex");
        return -1;
    }

    if ((info.fileDesc = BF_OpenFile(sfileName)) < 0)
    {
        BF_PrintError("[!] Error in opening the file in SHT_CreateSecondaryIndex");
        return -1;
    }

    if (BF_AllocateBlock(info.fileDesc) < 0)
    {
        BF_PrintError("[!] Error in allocating the block in SHT_CreateSecondaryIndex");
        BF_CloseFile(info.fileDesc);
        return -1;
    }

    if (BF_ReadBlock(info.fileDesc, 0, &first_block) < 0)
    {
        BF_PrintError("[!] Error in reading the block in SHT_CreateSecondaryIndex");
        BF_CloseFile(info.fileDesc);
        return -1;
    }

    memcpy(first_block, &info, sizeof(SHT_info));

    if (BF_WriteBlock(info.fileDesc, 0) < 0)
    {
        BF_PrintError("[!] Error in writing to block in SHT_CreateSecondaryIndex");
        BF_CloseFile(info.fileDesc);
        return -1;
    }

    if (BF_CloseFile(info.fileDesc) < 0)
    {
        BF_PrintError("[!] Error in closing the file in SHT_CreateSecondaryIndex");
        return -1;
    }

    //same procedure but for each bucket:

    void *new_page;
    SecondaryBlock new_block;
    new_block.maxRecords = 512 / (sizeof(SRecord) - 2 * sizeof(char) - sizeof(int));
    new_block.currRecords = 0;
    new_block.nextBlock = -1;

    for (int i = 1; i <= buckets; i++)
    {

        if ((info.fileDesc = BF_OpenFile(sfileName)) < 0)
        {
            BF_PrintError("[!] Error in opening the file in SHT_CreateSecondaryIndex");
            return -1;
        }

        if (BF_AllocateBlock(info.fileDesc) < 0)
        {
            BF_PrintError("[!] Error in allocating the block in SHT_CreateSecondaryIndex");
            BF_CloseFile(info.fileDesc);
            return -1;
        }

        if (BF_ReadBlock(info.fileDesc, i, &new_page) < 0)
        {
            BF_PrintError("[!] Error in reading the block in SHT_CreateSecondaryIndex");
            BF_CloseFile(info.fileDesc);
            return -1;
        }

        memcpy(new_page, &new_block, sizeof(SHT_info));

        if (BF_WriteBlock(info.fileDesc, i) < 0)
        {
            BF_PrintError("[!] Error in writing to block in SHT_CreateSecondaryIndex");
            BF_CloseFile(info.fileDesc);
            return -1;
        }

        if (BF_CloseFile(info.fileDesc) < 0)
        {
            BF_PrintError("[!] Error in closing the file in SHT_CreateSecondaryIndex");
            return -1;
        }
    }

    return 0; //success
}

SHT_info *SHT_OpenSecondaryIndex(char *sfileName)
{
    SHT_info *info;
    SecondaryBlock *block;

    info = malloc(sizeof(SHT_info));

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

int SHT_SecondaryInsertEntry(SHT_info *header_info, SecondaryRecord record)
{
    int num;
    SRecord srecord; //create an SRecord to keep the data and the id so that there is no need to store the while record
    srecord.blockId = record.blockId;
    void *page;

    if (strcmp(header_info->attrName, "name") == 0)
    {
        strcpy(srecord.data, record.record.name);
    }
    else if (strcmp(header_info->attrName, "surname") == 0)
    {
        strcpy(srecord.data, record.record.surname);
    }
    else if (strcmp(header_info->attrName, "address") == 0)
    {
        strcpy(srecord.data, record.record.address);
    }
    else
    {
        return -1;
    }

    int key = hashfunction('c', header_info->numBuckets, srecord.data);
    SecondaryBlock block;

    if (BF_ReadBlock(header_info->fileDesc, key, &page) < 0) //first page for this key
    {
        BF_PrintError("[!] Error in reading the block in SHT_SecondaryInsertEntry");
        BF_CloseFile(header_info->fileDesc);
        return -1;
    }

    memcpy(&block, page, sizeof(SecondaryBlock));

    while (block.nextBlock != -1) //check if there is a chain of blocks and get to the last one
    {
        key = block.nextBlock;
        if (BF_ReadBlock(header_info->fileDesc, key, &page) < 0)
        {
            BF_PrintError("[!] Error in reading the block in SHT_SecondaryInsertEntry");
            BF_CloseFile(header_info->fileDesc);
            return -1;
        }

        memcpy(&block, page, sizeof(SecondaryBlock));
    }

    if (block.maxRecords == block.currRecords) //block is full
    {
        void *new_page;
        SecondaryBlock new_block;
        new_block.maxRecords = 512 / (sizeof(SRecord) - 2 * sizeof(char) - sizeof(int));
        new_block.currRecords = 0;
        new_block.nextBlock = -1;
        new_block.records[0] = srecord;
        new_block.currRecords++;

        int num_blocks;

        if ((num_blocks = BF_GetBlockCounter(header_info->fileDesc)) < 0)
        {
            BF_PrintError("[!] Error in getting the block counter inside SHT_SecondaryInsertEntry");
            return -1;
        }

        block.nextBlock = num_blocks;

        memcpy(page, &block, sizeof(SecondaryBlock));

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

        memcpy(new_page, &new_block, sizeof(SecondaryBlock));

        if (BF_WriteBlock(header_info->fileDesc, num_blocks) < 0)
        {
            BF_PrintError("[!] Error in writing block in SHT_SecondaryInsertEntry");
            BF_CloseFile(header_info->fileDesc);
            return -1;
        }
    }
    else //block has space left to save new record
    {
        block.records[block.currRecords] = srecord;
        block.currRecords++;

        memcpy(page, &block, sizeof(SecondaryBlock));

        if (BF_WriteBlock(header_info->fileDesc, key) < 0)
        {
            BF_PrintError("[!] Error in writing block in SHT_SecondaryInsertEntry");
            BF_CloseFile(header_info->fileDesc);
            return -1;
        }
    }

    return key;
}

int SHT_SecondaryGetAllEntries(SHT_info header_info_sht, HT_info header_info_ht, void *value)
{
    int key = hashfunction('c', header_info_sht.numBuckets, value);

    int blocks_read = 0;
    int records_read = 0;
    char val[40];
    strcpy(val, (char *)value);
    int id;

    printf("Looking for record with surname: %s\n", val);

    while (1)
    {
        void *spage;
        SecondaryBlock *block;

        if (BF_ReadBlock(header_info_sht.fileDesc, key, &spage) < 0)
        {
            BF_PrintError("[!] Error in reading the block in SHT_SecondaryGetAllEntries");
            BF_CloseFile(header_info_ht.fileDesc);
            return -1;
        }

        blocks_read++;
        block = (SecondaryBlock *)spage;

        for (int j = 0; j < block->currRecords; j++)
        {

            if (strcmp(block->records[j].data, val) == 0) //found the Secondary Record with the same data
            {
                id = block->records[j].blockId; //get id of primary table block

                do
                {
                    void *ppage;
                    blocks_read++;

                    if (BF_ReadBlock(header_info_ht.fileDesc, id, &ppage) < 0) //read primary block
                    {
                        BF_PrintError("[!] Error in reading the block in SHT_SecondaryGetAllEntries");
                        return -1;
                    }

                    Block *pblock;
                    pblock = (Block *)ppage;

                    for (int k = 0; k < pblock->currRecords; k++) //search primary block to find and print record
                    {
                        if ((strcmp(pblock->records[k].name, val) == 0) || (strcmp(pblock->records[k].surname, val) == 0) || (strcmp(pblock->records[k].address, val) == 0))
                        {
                            printf("%d %s %s %s \n", pblock->records[k].id, pblock->records[k].name, pblock->records[k].surname, pblock->records[k].address);
                            return blocks_read;
                        }
                    }

                    id = pblock->nextBlock;

                } while (id > 0);
            }
        }
        if (block->nextBlock == -1)
        {
            break;
        }
        else
        {
            key = block->nextBlock;
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

int SHT_HashStatistics(char *fileName)
{
    SHT_info *sht_info;
    void *first_block;

    if ((sht_info = SHT_OpenSecondaryIndex(fileName)) == NULL)
    {
        fprintf(stderr, "[!] Error in opening hash_file in main()\n");
        SHT_CloseSecondaryIndex(sht_info);
        return -1;
    }
    if (BF_ReadBlock(sht_info->fileDesc, 0, &first_block) < 0)
    {
        BF_PrintError("[!] Error in reading the block in SHT_HashStatistics");
        BF_CloseFile(sht_info->fileDesc);
        return -1;
    }

    memcpy(sht_info, first_block, sizeof(SHT_info));

    int num_blocks = 0; //holds the number of blocks of the file;
    int min_records;
    int max_records;
    int average_records;
    int overload_buckets = 0;
    int overload[sht_info->numBuckets];
    memset(overload, 0, sht_info->numBuckets);

    for (int i = 1; i <= sht_info->numBuckets; i++) //for each bucket
    {
        SecondaryBlock block;
        void *page;

        if (BF_ReadBlock(sht_info->fileDesc, i, &page) < 0)
        {
            BF_PrintError("[!] Error in reading the block in SHT_HashStatistics");
            BF_CloseFile(sht_info->fileDesc);
            return -1;
        }

        num_blocks++;

        memcpy(&block, page, sizeof(SecondaryBlock));

        min_records = block.currRecords;
        max_records = block.currRecords;
        average_records = block.currRecords;
        int j = 1;

        while (block.nextBlock != -1) //more blocks linked
        {
            overload_buckets++;
            j++; //holds the number of blocks per bucket;

            if (BF_ReadBlock(sht_info->fileDesc, block.nextBlock, &page) < 0)
            {
                BF_PrintError("[!] Error in reading the block in SHT_HashStatistics");
                BF_CloseFile(sht_info->fileDesc);
                return -1;
            }

            num_blocks++;

            memcpy(&block, page, sizeof(SecondaryBlock));

            if (block.currRecords > max_records)
            {
                max_records = block.currRecords;
            }
            else if (block.currRecords < min_records)
            {
                min_records = block.currRecords;
            }

            average_records += block.currRecords;
        }

        overload[i - 1] = overload_buckets;

        average_records = average_records / j;

        printf("Bucket number %d has max number of records = %d, min number of records = %d and an average number of %d records\n", i, max_records, min_records, average_records);
    }

    printf("File %s has %d blocks\n", fileName, num_blocks);

    int average_blocks = num_blocks / sht_info->numBuckets;
    printf("The average number of blocks is %d\n", average_blocks);

    printf("There are %d overload buckets:\n", overload_buckets);
    for (int k = 0; k < sht_info->numBuckets; k++)
    {
        if (overload[k] != 0)
        {
            printf("bucket number %d has %d blocks\n", k + 1, overload[k]);
        }
    }

    if (SHT_CloseSecondaryIndex(sht_info) < 0)
    {
        fprintf(stderr, "[!] Error in closing hashfile in main()\n");
        return -1;
    }

    return 0;
}

void SHT_InsertEntries(HT_info *info, SHT_info *sinfo)
{
    FILE *FP;
    char *line = NULL;
    size_t length = 0;
    ssize_t read; // size type with an error value (-1)
    FP = stdin;
    Record record;
    int id;

    while ((read = getline(&line, &length, FP)) != -1)
    {
        line[read - 2] = 0;
        line++;
        char *tmp;

        tmp = strtok(line, ",");
        record.id = atoi(tmp);

        tmp = strtok(NULL, ",");
        tmp++;
        tmp[strlen(tmp) - 1] = '\0';
        strncpy(record.name, tmp, sizeof(record.name));

        tmp = strtok(NULL, ",");
        tmp++;
        tmp[strlen(tmp) - 1] = '\0';
        strncpy(record.surname, tmp, sizeof(record.surname));

        tmp = strtok(NULL, ",");
        tmp++;
        tmp[strlen(tmp) - 1] = '\0';
        strncpy(record.address, tmp, sizeof(record.address));

        line--;

        id = HT_InsertEntry(info, record);

        SecondaryRecord srecord;

        srecord.record = record;
        srecord.blockId = id;

        SHT_SecondaryInsertEntry(sinfo, srecord);
    }
    free(line);
}
