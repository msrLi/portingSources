/**
 * @file UpgradeHandle.h
 * @brief  UpgradeHandle
 * @author  <itarge@itarge.com>
 * @version 1.0.1
 * @date 2018-01-10
 */
/* Copyright(C) 2009-2017, Itarge Inc.
 * All right reserved
 *
 */

#ifndef __UPGRADEHANDLE_H__
#define __UPGRADEHANDLE_H__

#include "ace/Message_Block.h"
#include "UserTable.h"
#include "ImageFile.h"
#include "ite_about.h"

//�����붨��
#define AUTHENTICATION  0x1         //�����֤
#define REQUESTFILE     0x2         //�����ļ�
#define REQUESTDATA     0x3         //�������ݿ�
#define CRC32_FAILURE   0xFFFF      //crcЧ��ʧ��
#define REQUESTCLOSE    0x10000     //����Ͽ�

#define RESPONSE        0xa         //�ɹ���Ӧ��


#define c_packetLength             (ITE_PACKAGE_SIZE)

class CUpgradeHandle
{
    typedef struct UpgradePacket
    {
        ACE_INT32   crc32;
        ACE_INT32   packetDataLength;
        ACE_INT32   operateCode;
        ACE_TCHAR   packet[c_packetLength];

        UpgradePacket() {Reset();}
        void Reset() {ACE_OS::memset(this, 0, sizeof(UpgradePacket));}
    }* PUpgradePacket;

public:
    CUpgradeHandle(/*CImageFile *p*/);
    ~CUpgradeHandle(void);

public:
    int  handleData(ACE_Message_Block * pmb, char *&reply, char* szIP = NULL);

    int  SetImageFile(CImageFile *p);

private:
    int handleAuthentication(PUpgradePacket pPacket);
    int handleRequestFile(PUpgradePacket pPacket, char* szIP = NULL);
    int handleRequestData(PUpgradePacket pPacket);
    int handleRequestClose(PUpgradePacket pPacket);

    int SendPacket(PUpgradePacket pPacket);

    ACE_UINT32  crc32(ACE_UINT32 crc, const unsigned char  *buf, ACE_UINT32 len);

private:
    UpgradePacket replyPacket;
    bool bAuthentication;

    CImageFile* pImageFile;
    PerInFileInfo * pCurInFileInfo;
    ACE_INT32   curBlockNum;
};

#endif /* __UPGRADEHANDLE_H__ */
