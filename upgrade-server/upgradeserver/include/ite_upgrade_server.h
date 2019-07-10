/**
 * @file ite_upgrade_server.h
 * @brief  ����������API
 * @author  <itarge@itarge.com>
 * @version 1.0.0
 * @date 2018-01-12
 */

/* Copyright(C) 2009-2017, Itarge Inc.
 * All right reserved
 *
 */

#ifndef __ITE_UPGRADE_SERVER_H_H_
#define __ITE_UPGRADE_SERVER_H_H_



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


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
int ITE_UPGARDE_Init(int *port, char *file_name);


/* --------------------------------------------------------------------------*/
/**
 * @brief ITE_UPGARDE_Server_Send_By_IP
 *          ���������Ŀͻ���ip�� ITE_UPGARDE_Init�������ص�port�˿ں�
 * @param[in]   clientIP    Ҫ�����Ŀͻ���ip
 * @param[in]   port        ITE_UPGARDE_Init�������ص�port�˿ں�
 *
 * @return
 *      0  -->   success
 *     >0  -->   fault
 */
/* --------------------------------------------------------------------------*/
int ITE_UPGARDE_Server_Send_By_IP(char *clientIP, int port);


/* --------------------------------------------------------------------------*/
/**
 * @brief ITE_UPGRADE_Server_Clean
 *          ������ɻ���������
 * @return
 *      0  -->  success;
 */
/* --------------------------------------------------------------------------*/
int ITE_UPGRADE_Server_Clean(void);

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
int ITE_UPGRADE_Server_Get_Process_Value(char *Ip, int *value);


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
int ITE_UPGARDE_Monitor_Set(int level);

/* --------------------------------------------------------------------------*/
/**
 * @brief ITE_UPGRADE_Server_Start
 *          �ͻ�����������
 * @return
 *      0   -->     success;
 *      >0  -->     fault;
 */
/* --------------------------------------------------------------------------*/
int ITE_UPGRADE_Server_Start(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* __ITE_UPGRADE_SERVER_H_H_ */


