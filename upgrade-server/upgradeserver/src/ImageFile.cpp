#include "ace/OS_NS_string.h"
#include "ImageFile.h"
#include <string>
#include "md5.h"
#include "ite_about.h"

using std::string;

#define  NUM_LEN            (ITE_PACKAGE_SIZE)
char g_szImageType[32];
char g_szImageVer[32];

//char ImageFileList[FILE_MAX][PER_INFO_SIZE] = {"ubl.it","uboot.it","kernel.it","rootfs.it"};

CImageFile::CImageFile(void) : pFile(NULL), pOutFile_Info(NULL) /*,pPerInFileInfo(NULL)*/
{
    ACE_OS::memset(szmd5, 0, 32 * FILE_MAX);
}

CImageFile::~CImageFile(void)
{
}

int CImageFile::Init(char* szFileName /*,int type*/)
{
    MD5 md5;
    int i = 0;
    unsigned int offset = 0;
    unsigned int size = 0;
    unsigned int *ptr = NULL;

    if (mmapFile.map((const ACE_TCHAR *)szFileName, -1, O_RDONLY, 0) == -1)
    {
        int err = ACE_OS::last_error();
        return 2;
    }

    pFile = (char *) mmapFile.addr();
    pOutFile_Info = (OutFile_Info*)pFile;


    for (i = 0; i < pOutFile_Info->InputFileInfo.HowManyFile; i++)
    {
        ptr = (unsigned int *)pOutFile_Info->InputFileInfo.perfileinfo[i].offset;
        offset = *ptr;
        ptr = (unsigned int *)pOutFile_Info->InputFileInfo.perfileinfo[i].size;
        size = *ptr;
        md5.update(pFile + offset, size);

        string strMD5 = md5.toString();

        ACE_OS::memcpy(szmd5[i], strMD5.c_str(), 32);
    }

    return 0;
}

int CImageFile::GetFileInfo(char* szFileName, ResponseFileInfo & fileInfo, PerInFileInfo* &pFileInfo)
{
    pFileInfo = NULL;
    //  PerInFileInfo * pInFileInfo = NULL;
    //  pPerInFileInfo = NULL;
    int fileNum = pOutFile_Info->InputFileInfo.HowManyFile;
    unsigned int size = 0;
    unsigned int *ptr = NULL;

    int i = 0;
    for (; i < fileNum; i++)
    {
        if (0 == ACE_OS::strcmp(szFileName, pOutFile_Info->InputFileInfo.perfileinfo[i].name))
        {
            pFileInfo = &(pOutFile_Info->InputFileInfo.perfileinfo[i]);
            break;
        }
    }

    if (pFileInfo == NULL)
    {
        return -1;
    }

    ptr = (unsigned int *)pFileInfo->size;
    size = *ptr;
    fileInfo.blockNum = size / NUM_LEN;
    fileInfo.blockNum += size % NUM_LEN ? 1 : 0;
    ACE_OS::memcpy(fileInfo.md5, szmd5[i], 32);

    //curfileInfo.blockNum = fileInfo.blockNum;

    return 0;
}

int CImageFile::GetFileData(int nIndex, char* buffer, int &nlength, PerInFileInfo* pFileInfo, ACE_INT32    BlockNum)
{
    unsigned int offset = 0;
    unsigned int size = 0;
    unsigned int *ptr = NULL;

    if (pFileInfo == NULL)
    {
        return -1;
    }

    ptr = (unsigned int *)pFileInfo->offset;
    offset = *ptr;
    char* pPosition = pFile + offset + (nIndex - 1) * NUM_LEN;

    
    ptr = (unsigned int *)pFileInfo->size;
    size = *ptr;
    nlength = size % NUM_LEN;
    if (nlength == 0)
    {
        nlength = NUM_LEN;
    }

    if (nIndex < BlockNum)
    {
        memcpy(buffer, pPosition, NUM_LEN);
    }
    else if (nIndex == BlockNum)
    {
        memcpy(buffer, pPosition, nlength);
    }
    else
    {
        return -1;
    }

    return 0;
}
