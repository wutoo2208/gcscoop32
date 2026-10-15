#ifndef __LED_H
#define __LED_H

/** @brief 初始化工程使用的 LED GPIO。 */
void LED_Init(void);
/** @brief 翻转 PF9 指示灯。 */
void LED_Toggle0(void);
/** @brief 翻转 PF10 指示灯。 */
void LED_Toggle1(void);
/** @brief 同时翻转 PF9 和 PF10 指示灯。 */
void LED_Toggle(void);
/** @brief 翻转 PA8 指示灯。 */
void LED_Toggle2(void);

#endif
