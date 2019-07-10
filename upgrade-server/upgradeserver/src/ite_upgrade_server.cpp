/**
 * @file ite_upgrade_server.cpp
 * @brief  �����������API����
 * @author  <itarge@itarge.com>
 * @version 1.0.0
 * @date 2018-01-12
 */
/* Copyright(C) 2009-2017, Itarge Inc.
 * All right reserved
 *
 */

#include <ace/ACE.h>
#include "ace/Log_Msg.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Connector.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/SString.h"

#include "ace/Reactor.h"
#include "ace/Svc_Handler.h"
#include "ace/Synch.h"
#include "ace/Acceptor.h"
#include <netinet/tcp.h>
#include <ace/Init_ACE.h>
#include <assert.h>
#include <string>
#include <iostream>

#include "ite_upgrade_server_cpp.h"
#include "ite_upgradeNICP.h"
#include "ite_bar_process.h"
#include "ite_upgrade_server.h"

#define MAX_TRY_COUNT  5
#define MAX_UPGRADE_PARAM_LEN 32
#define MAX_IP_LIST   5

typedef struct ITE_UPGRADE_PARAM_TAG
{
    char szMode[MAX_UPGRADE_PARAM_LEN];
    char szServerIP[MAX_UPGRADE_PARAM_LEN];
    char szPort[MAX_UPGRADE_PARAM_LEN];
    char szUserName[MAX_UPGRADE_PARAM_LEN];
    char szPassword[MAX_UPGRADE_PARAM_LEN];
    char szIPNCIP[MAX_UPGRADE_PARAM_LEN];
    char szMAC[7];
    int  nIndex;
} ITE_UPGRADE_PARAM_T;

static int CONFIG_HOST_PRINT = 0;
/* ����ͷ�ļ���Ϣ */
OutFile_Info ITE_Upgrade_file_info_g;
/* �洢����ip�б� */
string ITE_Upgrade_IP_List[MAX_IP_LIST];
/* ����IP���� */
static int ITE_Upgrade_IP_Number = 0;
/* �����˿������Ϣ */
static ITE_UPGRADE_PARAM_T *ITE_Upgrade_Param_p[MAX_IP_LIST];
/* �����������߳�*/
static pthread_t *ITE_UPGRADE_Fun[MAX_IP_LIST];
/* �洢����ֵ */
static int ITE_Upgrade_Process_Bar_Value[MAX_IP_LIST];
static int ITE_Upgrade_Process_Ready[MAX_IP_LIST];

pthread_mutex_t ITE_Upgrade_Process_mutex[MAX_IP_LIST] = {PTHREAD_MUTEX_INITIALIZER};
/* ����������  */
void *ITE_UPGRADE_Thread_Loop(void * argv)
{
    int retryCount = 0;
    ITE_UPGRADE_PARAM_T *pUpgradeParam = (ITE_UPGRADE_PARAM_T *)argv;
    int i = 0;
    unsigned int time_delay = 0;
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("ITE_UPGRADE_Thread ::run \n")));
    ITE_Upgrade_Process_Ready[pUpgradeParam->nIndex] = 0;
RETRY:
    retryCount++;
    /* ���ӿͻ����������շ����� */
    ACE_INET_Addr addr_IPNC;
    ACE_UINT32 ip_addr = ntohl(inet_addr(pUpgradeParam->szIPNCIP));
    addr_IPNC.set(8080, ip_addr);
    ACE_SOCK_Stream peer_control;
    ACE_SOCK_Connector connector_IPNC;
    ACE_Time_Value timeout(0, 1000 * 1000);
    if (connector_IPNC.connect(peer_control, addr_IPNC, &timeout) != 0)
    {
        printf("ITE_ITARGE Debug: %s this is connect %d's failed \n", pUpgradeParam->szIPNCIP, retryCount);
        /* �������ӻ��Ǵ���ʧ�� */
        if (retryCount < MAX_TRY_COUNT)
        {
            goto RETRY;
        }
        else
        {
            ITE_Upgrade_Process_Ready[pUpgradeParam->nIndex] = -1;
            printf("ITE_ITARGE Error: upgrade Error1\n");
            return NULL;
        }
    }
    else
    {
        /* ���ӳɹ� */
        ITE_Upgrade_Process_Ready[pUpgradeParam->nIndex] = 1;
    }
    /* ѭ���ȴ������豸���ӳɹ�������ʱ�˳� */
    time_delay = 10;
    retryCount = 0;
    do
    {
        for (i = 0; i < ITE_Upgrade_IP_Number; i++)
        {
            if (ITE_Upgrade_Process_Ready[i] == 1)
            {
                retryCount++;
            }
        }
        if (retryCount == ITE_Upgrade_IP_Number)
        {
            break;
        }
        sleep(1);
        time_delay--;
    }
    while (time_delay == 0);
    if (time_delay == 0)
    {
        return NULL;
    }
    int flag = 1;
    peer_control.set_option(IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));

    /* ���������ļ�ͷ���豸 */
    unsigned int cmdIndex = 0;
    char *send_buf = new char[2048];
    memset(send_buf, 0, 2048);
    PITE_NICP_HEAD head = (PITE_NICP_HEAD)send_buf;
    head->Ver = eNICPVer11;
    head->Encryption = NON_Encryption;
    head->Cmd = SET_UPDATE_INFO;
    head->CmdIndex = cmdIndex;
    head->Len = sizeof(SdkRequestUpdateInfo);

    SdkRequestUpdateInfo *request = (SdkRequestUpdateInfo *)(send_buf + sizeof(ITE_NICP_HEAD));
    strcpy(request->header.serverip, pUpgradeParam->szServerIP);
    request->header.port = atoi(pUpgradeParam->szPort);
    strcpy(request->header.username, pUpgradeParam->szUserName);
    strcpy(request->header.passwd, pUpgradeParam->szPassword);
    strcpy(request->header.version, "V2.0.0");
    /* ��֮ǰ�洢���ļ���Ϣcopy�������еȴ����� */
    memcpy(&request->updatefileinfo, &ITE_Upgrade_file_info_g, sizeof(OutFile_Info));
    /* �������ݵ��ͻ���8080 */
    size_t sent_size = 0;
    peer_control.send_n(send_buf, sizeof(ITE_NICP_HEAD) + head->Len, &timeout, &sent_size);
    if (sent_size != sizeof(ITE_NICP_HEAD) + head->Len)
    {
        printf("ITE_ITARGE Debug:%s: this is connect %d's failed \n", pUpgradeParam->szIPNCIP, retryCount);
        delete []send_buf;

        peer_control.close_reader();
        peer_control.close_writer();
        peer_control.close();

        if (retryCount < MAX_TRY_COUNT)
        {
            goto RETRY;
        }
        else
        {
            printf("ITE_ITARGE Debug:upgrade Error2\n");
            return NULL;
        }
    }
    /* �������������Ϣ */
    char *recv_buf = send_buf;
    memset(recv_buf, 0, 2 * 1024);
    head = (PITE_NICP_HEAD)recv_buf;
    int nRet = peer_control.recv_n(recv_buf, sizeof(ITE_NICP_HEAD), &timeout);

    if ((nRet != sizeof(ITE_NICP_HEAD)) || (head->Result < 0))
    {
        printf("ITE_ITARGE Debug:%s: this is connect %d's failed \n", pUpgradeParam->szIPNCIP, retryCount);
        delete []send_buf;

        peer_control.close_reader();
        peer_control.close_writer();
        peer_control.close();

        if (retryCount < MAX_TRY_COUNT)
        {
            goto RETRY;
        }
        else
        {
            printf("ITE_ITARGE Debug:upgrade Error2\n");
            return NULL;
        }
    }
    printf("cmd is %d ,index %d, Len %d SET_UPDATE_INFO=%d\n", head->Cmd, head->CmdIndex, head->Len, SET_UPDATE_INFO);
    assert(head->Cmd == SET_UPDATE_INFO);
    assert(head->CmdIndex == cmdIndex);
    assert(head->Len > 0);

    char *msg_body = recv_buf + sizeof(ITE_NICP_HEAD);
    int msg_size = head->Len;
    nRet = peer_control.recv_n(msg_body, head->Len, &timeout);
    if (nRet != msg_size)
    {
        printf("ITE_ITARGE Debug:%s: this is connect %d's failed \n", pUpgradeParam->szIPNCIP, retryCount);
        delete []send_buf;

        peer_control.close_reader();
        peer_control.close_writer();
        peer_control.close();

        if (retryCount < MAX_TRY_COUNT)
        {
            goto RETRY;

        }
        else
        {
            printf("ITE_ITARGE Debug:upgrade Error2\n");
            return NULL;

        }
    }
    int *requiredUpgradeFiles = (int *)msg_body;
    if (*requiredUpgradeFiles <= 0)
    {
        printf("ITE_ITARGE Debug:%s %d is runing ...\n", __func__, __LINE__);
    }

    /* ��ȡ״ֵ̬ */
    unsigned int excp_count = 0;
    while (1)
    {
        if (excp_count > 10)
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("ITE_UPGRADE_Thread interrup \n")));
        }

        memset(send_buf, 0, 2 * 1024);
        head = (PITE_NICP_HEAD)send_buf;
        head->Ver = eNICPVer11;
        head->Encryption = NON_Encryption;
        head->Cmd = GET_UPDATE_STATUS;
        head->CmdIndex = ++cmdIndex;
        head->Len = 0;
        sent_size = 0;
        peer_control.send_n(send_buf, sizeof(ITE_NICP_HEAD), &timeout, &sent_size);
        if (sent_size != sizeof(ITE_NICP_HEAD))
        {
            excp_count++;
            printf("ITE Error: %s send status count is %d \n", pUpgradeParam->szIPNCIP, excp_count);
            usleep(10000);
            continue;
        }
        else
        {
            excp_count = 0;
        }

        recv_buf = send_buf;
        memset(recv_buf, 0, 2 * 1024);
        head = (PITE_NICP_HEAD)recv_buf;
        nRet = peer_control.recv_n(recv_buf, sizeof(ITE_NICP_HEAD), &timeout);
        if (nRet != sizeof(ITE_NICP_HEAD))
        {
            excp_count++;
            printf("%s: send status count is %d \n", pUpgradeParam->szIPNCIP, excp_count);
            usleep(10000);
            continue;
        }
        else
        {
            excp_count = 0;
        }

        if (head->Data > 100)
        {
            printf("Upgrade error %s %d \n", __func__, __LINE__);
            break;
        }
        else
        {
            /* �洢����ֵ */
            pthread_mutex_lock(&ITE_Upgrade_Process_mutex[pUpgradeParam->nIndex]);
            ITE_Upgrade_Process_Bar_Value[pUpgradeParam->nIndex] = head->Data;
            pthread_mutex_unlock(&ITE_Upgrade_Process_mutex[pUpgradeParam->nIndex]);
        }
        if (head->Data == 100)
        {
            memset(send_buf, 0, 2 * 1024);
            head = (PITE_NICP_HEAD)send_buf;
            head->Ver = eNICPVer11;
            head->Encryption = NON_Encryption;
            head->Cmd = SET_UPDATE_RESTART;
            head->CmdIndex = ++cmdIndex;
            head->Len = 0;
            sent_size = 0;
            peer_control.send_n(send_buf, sizeof(ITE_NICP_HEAD), &timeout, &sent_size);
            if (sent_size != sizeof(ITE_NICP_HEAD))
            {
                printf("upgrade ok, but send reboot error\n");
            }
            else
            {
                /* ��������ͳɹ�, ����һ����Ӧ��Ϣ���˳� */
                recv_buf = send_buf;
                memset(recv_buf, 0, 2 * 1024);
                head = (PITE_NICP_HEAD)recv_buf;
                nRet = peer_control.recv_n(recv_buf, sizeof(ITE_NICP_HEAD), &timeout);
            }
            break;
        }
    }

    return NULL;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief ITE_UPGARDE_Init
 *          �����������˳�ʼ��
 * @param[in/out]    port       �����ַ����ȡ�����������Ķ˿ں�
 * @param[in]        file_name  �����ļ�·��������
 *
 * @return
 *      0  -->   success
 *     >0  -->   fault
 */
/* --------------------------------------------------------------------------*/
int ITE_UPGARDE_Init(int *port, char *file_name)
{
    int _port = 0, ret = 0;
    FILE *fp = NULL;
    if (NULL == file_name || NULL == port)
    {
        printf("ITE %s paramer is error\n", __func__);
        return 1;
    }

    ACE::init();

    /* ��ȡ�����ļ�ͷ��Ϣ */
    memset(&ITE_Upgrade_file_info_g, 0, sizeof(OutFile_Info));
    fp  = fopen(file_name, "rb");
    if (NULL == fp)
    {
        printf("ITE Error: fopen error\n");
        return 2;
    }

    /* ��ȡ�����ļ���Ϣͷ */
    ret = fread(&ITE_Upgrade_file_info_g, sizeof(OutFile_Info), 1, fp);
    assert(ret == 1);
    fclose(fp);

    ITE_UPGRADE_Server::instance()->init(file_name);

    ret = ITE_UPGRADE_Server::instance()->open(&_port, file_name);
    switch (ret)
    {
        case 0:
            break;
        case 1:
            printf("open upgrade server failed\n");
            break;
        case 2:
            printf("open upgrade file failed\n");
            break;
        case 3:
            printf("open upgrade file is too old\n");
            break;
        case 4:
            printf("arsing the firmware update file failed\n");
            break;
        case 5:
        case 6:
            printf("The firmware version is not the kernel\n");
            break;
        case 7:
            printf("The firmware arsing failed or The file is not wanted firmware\n");
            break;
        case 8:
        case 9:
            printf("The firmware version is not UBL version\n");
            break;
        default:
            printf("Open firmware file failed\n");
            break;
    }
    if (ret != 0)
    {
        goto err;
    }
    ITE_UPGRADE_Server::instance()->start();
    *port = _port;

    return 0;
err:
    ITE_UPGRADE_Server::instance()->stop();
    ITE_UPGRADE_Server::instance()->final();
    return ret;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief ITE_UPGARDE_Server_Send_By_IP
 *          ���������Ŀͻ���ip�� ITE_UPGARDE_Init�������ص�port�˿ں�
 * @param[in]   clientIP    Ҫ�����Ŀͻ���ip
 * @param[in]   port        ITE_UPGARDE_Init�������ص�port�˿ں�
 *
 * @return
 *      0   -->   success;
 *     >0   -->   fault;
 */
/* --------------------------------------------------------------------------*/
int ITE_UPGARDE_Server_Send_By_IP(char *clientIP, int port)
{
    int str_len = 0, ret = 0;
    if (NULL == clientIP)
    {
        return 1;
    }

    if (MAX_IP_LIST <= ITE_Upgrade_IP_Number)
    {
        printf("ITE Debug: The Max ip list is %d \n", MAX_IP_LIST);
        return 2;
    }

    ITE_Upgrade_IP_List[ITE_Upgrade_IP_Number] = clientIP;
    /* �����µ�������Ϣ */
    ITE_Upgrade_Param_p[ITE_Upgrade_IP_Number] = (ITE_UPGRADE_PARAM_T*) malloc(sizeof(ITE_UPGRADE_PARAM_T));
    if (NULL == ITE_Upgrade_Param_p[ITE_Upgrade_IP_Number])
    {
        printf("ITE Error malloc fault\n");
        ret = 3;
        goto err;
    }

    /* ������� */
    memset(ITE_Upgrade_Param_p[ITE_Upgrade_IP_Number], 0, sizeof(ITE_UPGRADE_PARAM_T));

    /* У�鴫���ip�ֽ��� */
    if (MAX_UPGRADE_PARAM_LEN <= strlen(clientIP))
    {
        printf("ITE Error: IP address size is too long \n");
        return 4;
    }
    /* ��ȡ��Ӧ�ķ����ַip */
    ret = ITE_UPGRADE_Server::instance()->getServerIp(clientIP, ITE_Upgrade_Param_p[ITE_Upgrade_IP_Number]->szServerIP);
    if (ret)
    {
        printf("ITE Error: Get Server Ip error\n");
        goto err;
    }
    printf("ITE Debug: serverIp=%s\n", ITE_Upgrade_Param_p[ITE_Upgrade_IP_Number]->szServerIP);
    /* �洢Ŀ���ַip*/
    strcpy(ITE_Upgrade_Param_p[ITE_Upgrade_IP_Number]->szIPNCIP, clientIP);
    printf("ITE Debug: ITE_Upgrade_Param_p[%d]->szIPNCIP=%s \n", ITE_Upgrade_IP_Number, ITE_Upgrade_Param_p[ITE_Upgrade_IP_Number]->szIPNCIP);
    /* �洢�û��������� */
    strcpy(ITE_Upgrade_Param_p[ITE_Upgrade_IP_Number]->szUserName, "iTarge");
    strcpy(ITE_Upgrade_Param_p[ITE_Upgrade_IP_Number]->szPassword, "iTarge");
    /* �洢�˿ں� */
    ACE_OS::itoa(port, ITE_Upgrade_Param_p[ITE_Upgrade_IP_Number]->szPort, 10);
    printf("ITE Debug: ITE_Upgrade_Param_p[%d]->szPort=%s\n", ITE_Upgrade_IP_Number, ITE_Upgrade_Param_p[ITE_Upgrade_IP_Number]->szPort);
    ITE_Upgrade_Param_p[ITE_Upgrade_IP_Number]->nIndex = ITE_Upgrade_IP_Number;
    ITE_Upgrade_IP_Number++;

    return 0;
err:
    if (NULL != ITE_Upgrade_Param_p[ITE_Upgrade_IP_Number])
    {
        free(ITE_Upgrade_Param_p[ITE_Upgrade_IP_Number]);
    }
    return ret;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief ITE_UPGRADE_Server_Clean
 *          ������ɻ���������
 * @return
 *      0  -->  success;
 */
/* --------------------------------------------------------------------------*/
int ITE_UPGRADE_Server_Clean(void)
{
    int i = 0;
    for (i = 0; i < ITE_Upgrade_IP_Number; i++)
    {
        if (ITE_UPGRADE_Fun[i])
        {
            free(ITE_UPGRADE_Fun[i]);
        }
        if (ITE_Upgrade_Param_p[i])
        {
            free(ITE_Upgrade_Param_p[i]);
        }
    }
    ITE_Upgrade_IP_Number = 0;
    return 0;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief ITE_UPGRADE_Server_Get_Process_Value
 *          ��ȡ��������
 * @param[in]     Ip     Ҫ��ѯ�Ŀͻ���ip
 * @param[in/out] value  �������Ȱٷֱȷ���ֵ
 *
 * @return
 *      0   -->   success;
 *      >0  -->   fault;
 */
/* --------------------------------------------------------------------------*/
int ITE_UPGRADE_Server_Get_Process_Value(char *Ip, int *value)
{
    int i = 0;
    for (i = 0; i < ITE_Upgrade_IP_Number; i++)
    {
        /* ���� ip ��ַ���� ״ֵ̬ */
        if (0 == strcmp(Ip, ITE_Upgrade_Param_p[i]->szIPNCIP))
        {
            pthread_mutex_lock(&ITE_Upgrade_Process_mutex[i]);
            *value = ITE_Upgrade_Process_Bar_Value[i];
            pthread_mutex_unlock(&ITE_Upgrade_Process_mutex[i]);
            return 0;
        }
    }
    return 1;
}


/* --------------------------------------------------------------------------*/
/**
 * @brief ITE_UPGARDE_Monitor_Set
 *         ���ô�����ʾ����������
 * @param[in]    level      0 -- PTTy���������; 1 -- PTTy��ʾ����������
 *
 * @return
 *      0   -->     success;
 */
/* --------------------------------------------------------------------------*/
int ITE_UPGARDE_Monitor_Set(int level)
{
    if (level)
    {
        CONFIG_HOST_PRINT = 1;
    }
    else
    {
        CONFIG_HOST_PRINT = 0;
    }
    return 0;
}

void *ITE_UPGARDE_Monitor(void *)
{
    int i = 0;
    int stop_flges = 0, value = 0;
    if (ITE_Upgrade_IP_Number == 0)
    {
        ACE_Reactor::instance()->end_reactor_event_loop();
        return NULL;
    }
    for (i = 0; i < ITE_Upgrade_IP_Number; i++)
    {
        pthread_mutex_lock(&ITE_Upgrade_Process_mutex[i]);
        ITE_Upgrade_Process_Bar_Value[i] = 0;
        pthread_mutex_unlock(&ITE_Upgrade_Process_mutex[i]);
        stop_flges |= (0x1 << i);
    }
    while (1)
    {
        if (CONFIG_HOST_PRINT)
        {
            /* ���� */
            system("clear");
            /* ��ӡ�������� */
            printf("\t\t\t\t\t\t ITE UPGRADE SEVER STATUES\n\n\n");
        }

        for (i = 0; i < ITE_Upgrade_IP_Number; i++)
        {
            /* ��ǰ���̴ﵽ100%ʱ�����־ */
            pthread_mutex_lock(&ITE_Upgrade_Process_mutex[i]);
            value = ITE_Upgrade_Process_Bar_Value[i];
            pthread_mutex_unlock(&ITE_Upgrade_Process_mutex[i]);
            if (100 == value)
            {
                stop_flges &= (~(1 << i));
            }
            if (CONFIG_HOST_PRINT)
            {
                /* ���½����� */
                ITE_Bar_Process(ITE_Upgrade_Param_p[i]->szIPNCIP, value);
            }
        }
        /* ��ʾ�����������̶��ﵽ100 */
        if (0 == stop_flges)
        {
            break;
        }
        /* ��ʱ500 ms */
        usleep(500000);
    }
    /* ACE  ������ ֹͣѭ�� */
    ACE_Reactor::instance()->end_reactor_event_loop();
#if 0
    /* �������������� */
    for (i = 0; i < ITE_Upgrade_IP_Number; i++)
    {
        pthread_mutex_lock(&ITE_Upgrade_Process_mutex[i]);
        ITE_Upgrade_Process_Bar_Value[i] = 0;
        pthread_mutex_unlock(&ITE_Upgrade_Process_mutex[i]);
    }
#endif
    return NULL;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief ITE_UPGRADE_Server_Start
 *          �ͻ�����������
 * @return
 *      0   -->     success;
 *      >0  -->     fault;
 */
/* --------------------------------------------------------------------------*/
int ITE_UPGRADE_Server_Start(void)
{
    int ret = 0, i = 0;
    pthread_t monitor;
    if (ITE_Upgrade_IP_Number == 0)
    {
        printf("ITE there is no server to clinet\n");
        return 1;
    }
    for (i = 0; i < ITE_Upgrade_IP_Number; i++)
    {
        ITE_UPGRADE_Fun[i] = (pthread_t*)  malloc(sizeof(pthread_t));
        memset(ITE_UPGRADE_Fun[i], 0, sizeof(pthread_t));
        ret = pthread_create(ITE_UPGRADE_Fun[i], NULL, ITE_UPGRADE_Thread_Loop, ITE_Upgrade_Param_p[i]);
        if (ret)
        {
            printf("ITE Error: create thread error\n");
            ret = 2;
            goto  err;
        }
    }
    /* ����һ������ȴ������������ ͬʱ��ӡ������Ϣ */
    ret = pthread_create(&monitor, NULL, ITE_UPGARDE_Monitor, NULL);
    ACE_Reactor::instance()->run_reactor_event_loop();
    /* printf("ITE Upgrade success !!!\n"); */
    return 0;
err:
    ITE_UPGRADE_Server_Clean();
    return ret;
}

int ACE_TMAIN_SERVER(int argc, char** argv)
{
    int port = 0;
    int ret = 0;
    ITE_UPGARDE_Monitor_Set(1);
    ITE_UPGARDE_Init(&port, (char *)"/work/nand.bin");

    printf("port is %d \n", port);

    ret = ITE_UPGARDE_Server_Send_By_IP((char *)"192.168.3.44", port);
    if (ret)
    {
        printf("ITE Error: connetc to the 192.168.3.44 error\n");
        goto err;
    }
    ret = ITE_UPGARDE_Server_Send_By_IP((char *)"192.168.3.14", port);
    if (ret != 0)
    {
        goto err;
    }
    ITE_UPGRADE_Server_Start();
    ITE_UPGRADE_Server_Clean();
err:
    ITE_UPGRADE_Server::instance()->stop();
    ITE_UPGRADE_Server::instance()->final();
    return ret;
}

