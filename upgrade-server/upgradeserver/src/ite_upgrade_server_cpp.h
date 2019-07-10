#ifndef __ITE_UPGRADE_SERVER_CPP_H_
#define __ITE_UPGRADE_SERVER_CPP_H_
#include "ace/Log_Msg.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/SString.h"
#include "ace/Reactor.h"
#include "ace/Svc_Handler.h"
#include "ace/Synch.h"
#include "ace/Acceptor.h"
#include "UpgradeService.h"
#include "UserTable.h"
#include "ImageFile.h"

/*
 *  * *���ڴ���ͻ������ӵĴ�����
 *   * *���ͻ��˳�������������ʱ��open�������ᱻ�ص�ִ��
 *    * */
class AcceptorHandler : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
{
public:
    int open(void*)
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("connection established\n")));
        return 0;
    }

};

typedef ACE_Acceptor<CUpgradeService, ACE_SOCK_ACCEPTOR> ITE_UPGRADE_ClientAcceptor;

class ITE_UPGRADE_Acceptor : public ITE_UPGRADE_ClientAcceptor
{
public:
    int get_local_addr(ACE_Addr &addr) const
    {
        ACE_SOCK_Acceptor* pACE_SOCK_Acceptor = (ACE_SOCK_Acceptor*)&peer_acceptor_;
        pACE_SOCK_Acceptor->get_local_addr(addr);
        return 0;
    }
    /* ���� */
    CImageFile  ImageFile;

};
/* ��������ѭ������ */
class ite_reactor_thread : public ACE_Task<ACE_MT_SYNCH>
{
public:
    ite_reactor_thread() : bActive(false)
    {
    }

public:
    virtual int svc(void);
    bool bActive;
};
/* ������������ */
class ITE_UPGRADE_Server
{
    ITE_UPGRADE_Server(void)
    {
    }

public:
    virtual ~ITE_UPGRADE_Server(void);
    static ITE_UPGRADE_Server *instance();

    int init(char *userTableFileName);
    int open(int *nport, char *name);
    int stop(void);
    int final(void);
    int start(void);
    int svc(void);
    int getServerIp(char *clientIp, char * serverIp);
private:
    static void * ace_thread_fun(void *);
    /* ������ѭ������ */
    // ite_reactor_thread   reactorThread;
    pthread_t  ACE_thread_loop;
    /* �洢������� */
    ITE_UPGRADE_Acceptor *m_pUpgradeAcceptor;
    bool bActive;
};
#endif /* __ITE_UPGRADE_SERVER_H_ */
