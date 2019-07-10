#include "ace/OS_NS_errno.h"
#include "ace/OS_NS_sys_time.h"
#include "ace/os_include/os_netdb.h"
#include "ace/INET_Addr.h"
#include "UpgradeService.h"
#include "ite_upgrade_server_cpp.h"
#include "ite_about.h"

const int PACKLENGTH = ITE_PACKLENGTH;

CUpgradeService::CUpgradeService(void) : pRecv_Block(NULL)
{
    ACE_NEW_NORETURN(pRecv_Block, ACE_Message_Block(PACKLENGTH));
}

CUpgradeService::~CUpgradeService(void)
{
    CUpgradeService *pserver = new CUpgradeService;
    if (pRecv_Block)
    {
        pRecv_Block->release();
    }
}

int CUpgradeService::open(void * p/*= 0*/)
{
    if (super::open(p) == -1)
    {
        return -1;
    }


    ITE_UPGRADE_Acceptor *pUpgradeAcceptor = (ITE_UPGRADE_Acceptor*)p;
    upgradeHandle.SetImageFile(&pUpgradeAcceptor->ImageFile);

    ACE_INET_Addr peer_addr;
    if (this->peer().get_remote_addr(peer_addr) == 0 &&
            peer_addr.addr_to_string(peer_name, MAXHOSTNAMELEN) == 0)
    {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("->%T Connection from %s\n"),
                   peer_name));
    }
    return 0;
}

int CUpgradeService::handle_input(ACE_HANDLE fd /*= ACE_INVALID_HANDLE*/)
{
    ssize_t recv_cnt = this->peer().recv(pRecv_Block->wr_ptr(), pRecv_Block->space());
    if (recv_cnt <= 0)
    {
#if UPGRADE_DEBUG
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("->%T Connection closed\n")));
#endif /* UPGRADE_DEBUG */
        return -1;
    }

    pRecv_Block->wr_ptr(recv_cnt);
    recv_cnt = pRecv_Block->length();
    if (PACKLENGTH > recv_cnt)
    {
        return 0;
    }

    //处理完整包
    char* reply = NULL;
    int nret = upgradeHandle.handleData(pRecv_Block, reply);
    if (reply)
    {
        nret |= SendReply(reply);
    }

    pRecv_Block->reset();

    return nret;
}

int CUpgradeService::handle_output(ACE_HANDLE fd /*= ACE_INVALID_HANDLE*/)
{
    ACE_Message_Block *mb = 0;
    ACE_Time_Value nowait(ACE_OS::gettimeofday());
    while (-1 != this->getq(mb, &nowait))
    {
        ssize_t send_cnt =
            this->peer().send(mb->rd_ptr(), mb->length());
        if (send_cnt == -1)
            ACE_ERROR((LM_ERROR,
                       ACE_TEXT("(%P|%t) %p\n"),
                       ACE_TEXT("send")));
        else
        {
            mb->rd_ptr(static_cast<size_t>(send_cnt));
        }
        if (mb->length() > 0)
        {
            this->ungetq(mb);
            break;
        }
        mb->release();
    }

    return (this->msg_queue()->is_empty()) ? -1 : 0;
}

int CUpgradeService::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask)
{
    if (close_mask == ACE_Event_Handler::WRITE_MASK)
    {
        return 0;
    }

    return super::handle_close(handle, close_mask);
}


int CUpgradeService::SendReply(char* reply)
{
    ssize_t send_cnt = 0;
    int output_off = this->msg_queue()->is_empty();
    if (output_off)
    {
        send_cnt = this->peer().send(reply, PACKLENGTH);

        if (send_cnt == PACKLENGTH)
        {
            return 0;
        }

        if (send_cnt == -1 && ACE_OS::last_error() != EWOULDBLOCK)
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("(%P|%t) %p\n"),
                              ACE_TEXT("send")),
                             0);
        if (send_cnt == -1)
        {
            send_cnt = 0;
        }
    }

    ACE_Message_Block* pmb = NULL;
    ACE_NEW_RETURN(pmb, ACE_Message_Block(PACKLENGTH - send_cnt), -1);

    pmb->copy((reply + send_cnt), PACKLENGTH - send_cnt);

    ACE_Time_Value nowait(ACE_OS::gettimeofday());
    if (this->putq(pmb, &nowait) == -1)
    {
        ACE_ERROR((LM_ERROR,
                   ACE_TEXT("(%P|%t) %p; discarding data\n"),
                   ACE_TEXT("enqueue failed")));
        pmb->release();
        return -1;
    }
    if (output_off)
        return this->reactor()->register_handler
               (this, ACE_Event_Handler::WRITE_MASK);

    return 0;
}
