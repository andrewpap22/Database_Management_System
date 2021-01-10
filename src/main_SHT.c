#include "header_files/SHT/SHT.h"

#define SECONDARY_FILENAME "secondary_hash_file.txt"
#define FILENAME "hash_file.txt"

int main(void)
{
  HT_info *ht_info;
  SHT_info *sht_info;

  BF_Init();

  /*
   * Create HT 
  */

  if ((HT_CreateIndex(FILENAME, 'i', "id", sizeof(int), 3)) < 0)
  {
    fprintf(stderr, "[!] Error in creating hash_file in main()\n");
    return -1;
  }

  if ((ht_info = HT_OpenIndex(FILENAME)) == NULL)
  {
    fprintf(stderr, "[!] Error in opening hash_file in main()\n");
    HT_CloseIndex(ht_info);
    return -1;
  }

  /*
   * Create SHT 
  */

  if ((SHT_CreateSecondaryIndex(SECONDARY_FILENAME, "surname", sizeof(char *), 3, FILENAME)) < 0)
  {
    fprintf(stderr, "[!] Error in creating secondary hash_file in main()\n");
    return -1;
  }

  if ((sht_info = SHT_OpenSecondaryIndex(SECONDARY_FILENAME)) == NULL)
  {
    fprintf(stderr, "[!] Error in opening secondary hash_file in main()\n");
    SHT_CloseSecondaryIndex(sht_info);
    return -1;
  }

  // insert

  InsertEntries(ht_info);

  if (HT_CloseIndex(ht_info) < 0)
  {
    fprintf(stderr, "[!] Error in closing hashfile in main()\n");
    return -1;
  }

  if (SHT_CloseSecondaryIndex(sht_info) < 0)
  {
    fprintf(stderr, "[!] Error in closing secondary hashfile in main()\n");
    return -1;
  }

  return 0;
}