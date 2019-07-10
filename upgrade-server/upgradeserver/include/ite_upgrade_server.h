/**
 * @file ite_upgrade_server.h
 * @brief  服务器升级API
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
 *          升级服务器端初始化
 * @param[in/out]    port       传入地址，获取服务器开启的端口号
 * @param[in]        file_name  升级文件路径和名称
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
 *          输入升级的客户端ip和 ITE_UPGARDE_Init函数返回的port端口号
 * @param[in]   clientIP    要升级的客户端ip
 * @param[in]   port        ITE_UPGARDE_Init函数返回的port端口号
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
 *          升级完成环境清理函数
 * @return
 *      0  -->  success;
 */
/* --------------------------------------------------------------------------*/
int ITE_UPGRADE_Server_Clean(void);

/* --------------------------------------------------------------------------*/
/**
 * @brief ITE_UPGRADE_Server_Get_Process_Value
 *          获取升级进度
 * @param[in]     Ip     要查询的客户端ip
 * @param[in/out] value  升级进度百分比返回值
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
 *         设置串口显示升级进度条
 * @param[in]    level      0 -- PTTy不打进度条; 1 -- PTTy显示升级进度条
 *
 * @return
 *      0   -->     success;
 */
/* --------------------------------------------------------------------------*/
int ITE_UPGARDE_Monitor_Set(int level);

/* --------------------------------------------------------------------------*/
/**
 * @brief ITE_UPGRADE_Server_Start
 *          客户端升级启动
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


