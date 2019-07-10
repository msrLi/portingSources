/**
 * @file UpgradeService.h
 * @brief UpgradeService API
 * @author  <itarge@itarge.com>
 * @version 1.0.0
 * @date 2018-01-10
 */
/* Copyright(C) 2009-2017, Itarge Inc.
 * All right reserved
 *
 */
#ifndef _UPGRADESERVICE_H_
#define _UPGRADESERVICE_H_

#include "ace/Synch_Traits.h"
#include "ace/Null_Condition.h"
#include "ace/Null_Mutex.h"

#include "ace/Message_Block.h"
#include "ace/SOCK_Stream.h"
#include "ace/Svc_Handler.h"
#include "UpgradeHandle.h"

#include "ImageFile.h"

class CUpgradeService :
    public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
{
    typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> super;

public:
    CUpgradeService(void);
    ~CUpgradeService(void);

public:
    int open(void *p = 0);

    virtual int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);

    virtual int handle_output(ACE_HANDLE fd = ACE_INVALID_HANDLE);

    virtual int handle_close(ACE_HANDLE handle,
                             ACE_Reactor_Mask close_mask);

public:
    int SendReply(char* reply);
    //  int SendPacket(ACE_Message_Block* pmb);

private:
    ACE_Message_Block*  pRecv_Block;
    CUpgradeHandle      upgradeHandle;

    ACE_TCHAR peer_name[256];
};
#endif /* _UPGRADESERVICE_H_ */
