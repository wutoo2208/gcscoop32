#ifndef __USART5_H
#define __USART5_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 

/** @brief 初始化 UART5 蓝牙通信接口及接收中断；bound 为波特率，单位 bit/s。 */
void uart5_init(u32 bound);
/** @brief 通过 UART5 发送以 NUL 结尾的字符串；轮询等待发送，调用会阻塞。 */
void UART5_SendString(char *str);
/** @brief 通过 UART5 发送 len 个字节；轮询等待发送完成，调用会阻塞。 */
void UART5_SendArray(uint8_t *data, uint16_t len);
/** @brief 发送当前源码中固定写入的任务码示例数据。 */
void fasong_task_code(void);

#endif
