
/**
 * @file ImageFile.h
 * @brief ÄÚºËÎÄ¼þAPI
 * @author  <itarge@itarge.com>
 * @version 1.0.1
 * @date 2018-01-10
 */
/* Copyright(C) 2009-2017, Itarge Inc.
 * All right reserved
 *
 */

#ifndef __IMAGEFILE_H__
#define __IMAGEFILE_H__


#include "ace/Mem_Map.h"


#define FILE_MAX 16
#define PER_INFO_SIZE 32
#define MAX_INFO_MUM 32
//#define global_offset (PER_INFO_SIZE*MAX_INFO_MUM)
#define INT_SIZE 4     // use as int in 32Bit system


typedef struct
{
    char Pro_Name[PER_INFO_SIZE];
    char Pro_Ver[PER_INFO_SIZE];
    char ALG_ver[PER_INFO_SIZE];
    char NICP_ver[PER_INFO_SIZE];
    char Pro_BuildTime[PER_INFO_SIZE];
} Product_Info;


//typedef struct
//{
//  char size[INT_SIZE];
//  char offset[INT_SIZE];
//  char name[PER_INFO_SIZE];
//}PerInFileInfo;

typedef struct
{
    char size[INT_SIZE];
    char offset[INT_SIZE];
    char name[PER_INFO_SIZE];
    char  version[PER_INFO_SIZE];// module version
} PerInFileInfo;

typedef struct
{
    int HowManyFile;
    PerInFileInfo perfileinfo[FILE_MAX];
} Input_Info;

typedef struct
{
    Product_Info pro_info;
    Input_Info InputFileInfo;
} OutFile_Info;


typedef struct
{
    ACE_INT32   blockNum;
    char    md5[32];
} ResponseFileInfo;


class CImageFile
{
public:
    CImageFile(void);
    ~CImageFile(void);

    int Init(char* szFileName/*,int type*/);

    int GetFileInfo(char* szFileName, ResponseFileInfo & fileInfo, PerInFileInfo* &pFileInfo);
    int GetFileData(int nIndex, char* buffer, int &nlength, PerInFileInfo* pFileInfo, ACE_INT32 BlockNum);

public:

    ACE_Mem_Map mmapFile;

    char* pFile;

    OutFile_Info* pOutFile_Info;

    char    szmd5[FILE_MAX][32];

    //  PerInFileInfo * pPerInFileInfo;
    //  ResponseFileInfo  curfileInfo;
};

#endif /* __IMAGEFILE_H__ */
