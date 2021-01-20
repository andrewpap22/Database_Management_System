#include "header_files/SHT/SHT.h"

#define FILENAME "hash_file.txt"
#define SECONDARY_FILENAME "secondary_hash_file.txt"

int main(void)
{
  HT_info *info;

  BF_Init();

  if ((HT_CreateIndex(FILENAME, 'i', "id", sizeof(int), 6)) < 0)
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

  SHT_info *sht_info;

  if ((SHT_CreateSecondaryIndex(SECONDARY_FILENAME, "surname", sizeof(char *), 3, FILENAME)) < 0)
  {
    fprintf(stderr, "[!] Error in creating secondary index in main().");
    return -1;
  }

  if ((sht_info = SHT_OpenSecondaryIndex(SECONDARY_FILENAME)) == NULL)
  {
    fprintf(stderr, "[!] Error in opening secondary hash file in main().");
    SHT_CloseSecondaryIndex(sht_info);
    exit(EXIT_FAILURE);
  }

  SHT_InsertEntries(info, sht_info);

  printf("============================================================\n");
  printf("INSERT DONE FOR BOTH HT AND SHT\n");

  printf("============================================================\n");
  printf("Get All Entries for HT:\n");

  int value_ht = 4;
  int blocks_read;
  if ((blocks_read = HT_GetAllEntries(*info, &value_ht)) < 0)
  {
    fprintf(stderr, "[!] Error in getting all the entries in main()\n");
    return -1;
  }

  printf("Blocks read for HT = %d\n", blocks_read);

  printf("============================================================\n");
  printf("Get All Entries for SHT:\n");

  char value_sht[40] = "surname_4980";
  blocks_read = SHT_SecondaryGetAllEntries(*sht_info, *info, value_sht);

  printf("Blocks read for SHT= %d\n", blocks_read);

  printf("============================================================\n");
  printf("Statistics for Primary Hash Table: \n");
  HashStatistics(FILENAME);

  printf("============================================================\n");
  printf("Statistics for Secondary Hash Table: \n");
  SHT_HashStatistics(SECONDARY_FILENAME);

  return 0; // program terminated successfully.
}