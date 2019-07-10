#include "ite_upgrade_server_cpp.h"
#include "ace/SOCK_Connector.h"
#include <ace/Init_ACE.h>

ITE_UPGRADE_Server::~ITE_UPGRADE_Server(void)
{

}


ITE_UPGRADE_Server * ITE_UPGRADE_Server::instance()
{
    static  ITE_UPGRADE_Server UpgradeServer;
    return &UpgradeServer;
}

int ITE_UPGRADE_Server::init(char *userTableFileName)
{
    return CUserTable::instance()->InitUserTable(userTableFileName);
    return 0;
}

int ITE_UPGRADE_Server::open(int *nport, char *file_name)
{
    /* ACE_DEBUG((LM_DEBUG, ACE_TEXT("ITE_UPGRADE_Server::open\n"))); */

    /* 创建一个监听服务器程序 */
    u_short port_number = 0;
    ACE_INET_Addr  port_to_listen(port_number, INADDR_ANY), client_addr;
    ITE_UPGRADE_Acceptor *pAcceptor = new ITE_UPGRADE_Acceptor;
    pAcceptor->open(port_to_listen, ACE_Reactor::instance(), ACE_NONBLOCK);

    /* 获取端口号 */
    pAcceptor->get_local_addr(port_to_listen);
    *nport = port_to_listen.get_port_number();
    /* 打开文件服务 */
    int nRet = pAcceptor->ImageFile.Init(file_name/*,type*/);
    m_pUpgradeAcceptor = pAcceptor;
    return 0;

}

int ITE_UPGRADE_Server::stop()
{

    if (m_pUpgradeAcceptor != NULL)
    {
        m_pUpgradeAcceptor->close();
        delete m_pUpgradeAcceptor;
        m_pUpgradeAcceptor = NULL;
    }
    return 0;
}

int ITE_UPGRADE_Server::final(void)
{
    return 0;
}
int ITE_UPGRADE_Server::getServerIp(char *clientIp, char * serverIp)
{
    ACE_INET_Addr addr_IPNC;
    ACE_SOCK_Connector connector_IPNC;
    ACE_SOCK_Stream peer_IPNC;
    ACE_Time_Value ConnectWaiTime(1);

    if (NULL == clientIp || NULL == serverIp)
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("ITE_UPGRADE_Server please entry addrss if Addr\n")));
        return 1;
    }

    string strIP = clientIp;
    strIP  += ":8080";
    addr_IPNC.set(strIP.c_str());
    if (connector_IPNC.connect(peer_IPNC, addr_IPNC, &ConnectWaiTime) != 0)
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("Connet timeout or fault\n")));
        return 2;
    }

    ACE_INET_Addr addr_Client;
    peer_IPNC.get_local_addr(addr_Client);

    in_addr ipaddr;
    //ipaddr.S_un.S_addr = htonl(addr_Client.get_ip_address()); //windows
    ipaddr.s_addr  = htonl(addr_Client.get_ip_address()); //linux

    strcpy(serverIp, inet_ntoa(ipaddr));
    peer_IPNC.close();

    return 0;
}
void * ITE_UPGRADE_Server::ace_thread_fun(void *argv)
{
    // ACE_DEBUG((LM_DEBUG, ACE_TEXT("ITE_UPGRADE_Thread ::run \n")));
    ACE::init();
    while (1)
    {
        ACE_Reactor::instance()->handle_events();
        // ACE_Reactor::instance()->run_reactor_event_loop();
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("ITE_UPGRADE_Thread ::end \n")));
    return NULL;
}
int ITE_UPGRADE_Server::start(void)
{
    // ACE_thread_loop;
    /*
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("ITE_UPGRADE_Server::start\n")));
    */
    if (!bActive)
    {
#if 0
        /* 启动reactor 线程 */
        if (pthread_create(&ACE_thread_loop, NULL, ace_thread_fun, this))
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("create thread error\n")));
            return 1;
        }
#endif
        bActive = true;
    }

    /* ACE_DEBUG((LM_DEBUG, ACE_TEXT("ITE_UPGRADE_Server::start\n"))); */

    return 0;
}
int ite_reactor_thread::svc(void)
{
#if 0
    /* ACE reactor 复位 */
    ACE_Reactor::instance()->reset_reactor_event_loop();
    /* ACE reactor 循环处理 */
    ACE_Reactor::instance()->run_reactor_event_loop();
#endif
    /* 标志设置为 false */
    bActive = false;
    return 0;
}
