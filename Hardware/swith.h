#ifndef __SWITH_H
#define __SWITH_H

/** @brief 将 PC9 配置为带上拉的开关量输入。 */
void Swith_Init(void);
/** @brief 读取 PC9 原始电平，返回值为 0 或 1。 */
uint8_t swith_out(void);

#endif
