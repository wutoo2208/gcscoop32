#include "stm32f4xx.h"                  // Device header

/** @brief 将 PC9 配置为带上拉的开关量输入。 */
void Swith_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);			//开启GPIOC的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;					
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	        //配置PC9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);							//初始化GPIOC
}

/** @brief 读取 PC9 原始电平；返回 1 表示高电平，返回 0 表示低电平。 */
uint8_t swith_out(void)
{   
   return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9);
}
