#include "../../header_files/HP/HP.h"

int HP_CreateFile(char *fileName, char attrType, char *attrName, int attrLength)
{
  HP_info info;
  void *block;

  info.attrLength = attrLength;
  info.attrName = attrName;
  info.attrType = attrType;

  if (BF_CreateFile(fileName) < 0)
  {
    BF_PrintError("[!] Error in creating the file in HP_CreateFile!");
    return -1;
  }

  if ((info.fileDesc = BF_OpenFile(fileName)) < 0)
  {
    BF_PrintError("[!] Error in opening the file in HP_CreateFile");
    return -1;
  }

  /*
   * Store the HP_info struct for use of the type of file recognition (fileDesc)
  */
  if (BF_AllocateBlock(info.fileDesc) < 0)
  {
    BF_PrintError("[!] Error in allocating the block in HP_CreateFile");
    BF_CloseFile(info.fileDesc);
    return -1;
  }

  if (BF_ReadBlock(info.fileDesc, 0, &block) < 0)
  {
    BF_PrintError("[!] Error in reading the block in HP_CreateFile");
    BF_CloseFile(info.fileDesc);
    return -1;
  }

  memcpy(block, &info, sizeof(HP_info));

  if (BF_WriteBlock(info.fileDesc, 0) < 0)
  {
    BF_PrintError("[!] Error in writing to block in HP_CreateFile");
    BF_CloseFile(info.fileDesc);
    return -1;
  }

  if(BF_CloseFile(info.fileDesc) < 0)
  {
    BF_PrintError("[!] Error in closing the file in HP_CreateFile");
    return -1;
  }

  return 0; // Everything went smoothly!
}

HP_info *HP_OpenFile(char *fileName)
{

}

int HP_CloseFile(HP_info *header_info)
{

}

int HP_InsertEntry(HP_info header_info, Record record)
{

}

int HP_DeleteEntry(HP_info header_info, void *value)
{

}

int HP_GetAllEntries(HP_info header_info, void *value)
{

}