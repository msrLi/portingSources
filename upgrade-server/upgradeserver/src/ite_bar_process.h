/**
 * @file ite_bar_process.h
 * @brief 进度条操作API
 * @author  <itarge@itarge.com>
 * @version 1.0.1
 * @date 2018-01-12
 */
/* --------------------------------------------------------------------------*/
/**
 * @brief ITE_Bar_Process
 *          C语言进度条
 * @param[in]  bar_name  显示进度条名称(可以为NULL)
 * @param[in]  number    进度占比%   [0--100]
 */
/* --------------------------------------------------------------------------*/
void ITE_Bar_Process(char *bar_name, unsigned char number);
