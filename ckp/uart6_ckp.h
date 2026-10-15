#ifndef __UART6_CKP_H
#define __UART6_CKP_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 

/** @brief 初始化 USART6 串口屏通信；bound 为波特率，单位 bit/s。 */
void uart6_init(uint32_t bound);
/** @brief 通过 USART6 轮询发送以 NUL 结尾的人机界面命令字符串。 */
void HMISends(char *buf1);		  
/** @brief 通过 USART6 重复发送非零字节；当前实现用于发送 3 字节结束符。 */
void HMISendb(uint8_t k);	

#endif
