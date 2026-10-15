#ifndef _HMI_H_
#define _HMI_H_

/** @brief 通过人机界面串口重复发送非零字节；当前实现发送 3 次。 */
void HMISendb(u8 k);
/** @brief 通过人机界面串口轮询发送以 NUL 结尾的命令字符串。 */
void HMISends(char *buf1);

#endif
