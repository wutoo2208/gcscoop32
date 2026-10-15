#ifndef __IMU_H
#define __IMU_H

/** @brief 初始化 IMU 通信与 SDK，并扫描当前传感器波特率；调用会阻塞。 */
void imu_init(void);
void imu_process(void); /* Call from foreground to handle UART commands. */
/** @brief 读取并换算三轴加速度（g）、角速度（度/秒）和角度（度）；每个输出数组至少有 3 个元素。 */
void imu_scan(float* fAcc,float* fGyro,float* fAngle);
/** @brief 通过 USART2 发送 uiSize 个字节；轮询等待发送完成，属于阻塞调用。 */
void Uart2Send(unsigned char *p_data, unsigned int uiSize);
/** @brief 初始化 USART2 收发和接收中断；uiBaud 单位为 bit/s。 */
void Usart2Init(unsigned int uiBaud);

#endif
