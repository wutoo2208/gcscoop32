#include "stm32f4xx.h"

void LED_Toggle(void);

/** @brief 初始化 PF9、PF10 和 PA8 指示灯 GPIO，并执行一次双灯翻转。 */
void LED_Init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;       // 设置为输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      // 开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // 50MHz的速度
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;         
 	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //配置PA8
	
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;       // 设置为输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      // 开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // 50MHz的速度
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;         
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	LED_Toggle();
}


/** @brief 翻转 GPIOF 的 PF9 指示灯输出。 */
void LED_Toggle0(void)
{
	GPIO_ToggleBits(GPIOF,GPIO_Pin_9);
}

/** @brief 翻转 GPIOF 的 PF10 指示灯输出。 */
void LED_Toggle1(void)
{
	GPIO_ToggleBits(GPIOF,GPIO_Pin_10);
}

/** @brief 同时翻转 GPIOF 的 PF9 和 PF10 输出。 */
void LED_Toggle(void)
{
	GPIO_ToggleBits(GPIOF,GPIO_Pin_9);
	GPIO_ToggleBits(GPIOF,GPIO_Pin_10);
}

/** @brief 翻转 GPIOA 的 PA8 指示灯输出。 */
void LED_Toggle2(void)
{
	GPIO_ToggleBits(GPIOA,GPIO_Pin_8);
}
