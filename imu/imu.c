#include "stm32f4xx.h"                  // Device header
#include "wit_c_sdk.h"
#include "imu.h"
#include "delay.h"
#include "timer.h"

#define ACC_UPDATE		0x01
#define GYRO_UPDATE		0x02
#define ANGLE_UPDATE	0x04
#define MAG_UPDATE		0x08
#define READ_UPDATE		0x80
float realbaud = 0;
static volatile char s_cDataUpdate = 0;
const uint32_t c_uiBaud[10] = {0, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600};
#define IMU_CMD_QUEUE_SIZE 4U
#define IMU_CMD_NONE 0xFFU
static volatile uint8_t s_cmdQueue[IMU_CMD_QUEUE_SIZE];
static volatile uint8_t s_cmdHead = 0U;
static volatile uint8_t s_cmdTail = 0U;
static uint8_t s_cmdParserState = 0U;
static uint8_t s_cmdParserValue = IMU_CMD_NONE;

static void CopeCmdData(uint8_t data);
static uint8_t IsImuCommand(uint8_t data);
static void QueueImuCommand(uint8_t command);
static void CmdProcess(uint8_t command);
static void ShowHelp(void);
static void AutoScanSensor(void);
/** @brief 解析串口命令字节；仅接受单字节命令后跟 CRLF 的格式并入队。 */
static void CopeCmdData(uint8_t data)
{
    if (s_cmdParserState == 0U)
    {
        if (IsImuCommand(data) != 0U)
        {
            s_cmdParserValue = data;
            s_cmdParserState = 1U;
        }
    }
    else if (s_cmdParserState == 1U)
    {
        if (data == '\r')
        {
            s_cmdParserState = 2U;
        }
        else
        {
            s_cmdParserState = 0U;
            s_cmdParserValue = IMU_CMD_NONE;
            if (IsImuCommand(data) != 0U)
            {
                s_cmdParserValue = data;
                s_cmdParserState = 1U;
            }
        }
    }
    else
    {
        if ((data == '\n') && (s_cmdParserValue != IMU_CMD_NONE))
        {
            QueueImuCommand(s_cmdParserValue);
        }
        s_cmdParserState = 0U;
        s_cmdParserValue = IMU_CMD_NONE;
        if (IsImuCommand(data) != 0U)
        {
            s_cmdParserValue = data;
            s_cmdParserState = 1U;
        }
    }
}

/** @brief 判断输入字节是否属于本模块支持的 IMU 配置命令。 */
static uint8_t IsImuCommand(uint8_t data)
{
    switch (data)
    {
        case 'a': case 'm': case 'e': case 'u': case 'U':
        case 'B': case 'b': case 'R': case 'r': case 'C':
        case 'c': case 'h':
            return 1U;
        default:
            return 0U;
    }
}

/* Bounded single-producer (USART IRQ), single-consumer (foreground) queue. */
static void QueueImuCommand(uint8_t command)
{
    uint8_t nextHead = (uint8_t)((s_cmdHead + 1U) % IMU_CMD_QUEUE_SIZE);
    if (nextHead != s_cmdTail)
    {
        s_cmdQueue[s_cmdHead] = command;
        s_cmdHead = nextHead;
    }
}

/* Must be called from foreground code, never from an interrupt handler. */
void imu_process(void)
{
    uint8_t command;
    if (s_cmdTail == s_cmdHead)
    {
        return;
    }

    command = s_cmdQueue[s_cmdTail];
    s_cmdTail = (uint8_t)((s_cmdTail + 1U) % IMU_CMD_QUEUE_SIZE);
    CmdProcess(command);
}

/** @brief 在前台执行已排队的 IMU 校准、波特率、输出频率和数据项配置命令。 */
static void CmdProcess(uint8_t command)
{
    switch (command)
    {
        case 'a': (void)WitStartAccCali(); break;
        case 'm': (void)WitStartMagCali(); break;
        case 'e': (void)WitStopMagCali(); break;
        case 'u': (void)WitSetBandwidth(BANDWIDTH_5HZ); break;
        case 'U': (void)WitSetBandwidth(BANDWIDTH_256HZ); break;
        case 'B':
            if (WitSetUartBaud(WIT_BAUD_115200) == WIT_HAL_OK)
            {
                Usart2Init(c_uiBaud[WIT_BAUD_115200]);
                realbaud = (float)c_uiBaud[WIT_BAUD_115200];
            }
            break;
        case 'b':
            if (WitSetUartBaud(WIT_BAUD_9600) == WIT_HAL_OK)
            {
                Usart2Init(c_uiBaud[WIT_BAUD_9600]);
                realbaud = (float)c_uiBaud[WIT_BAUD_9600];
            }
            break;
        case 'R': (void)WitSetOutputRate(RRATE_10HZ); break;
        case 'r': (void)WitSetOutputRate(RRATE_1HZ); break;
        case 'C': (void)WitSetContent(RSW_ACC | RSW_GYRO | RSW_ANGLE | RSW_MAG); break;
        case 'c': (void)WitSetContent(RSW_ACC); break;
        case 'h': ShowHelp(); break;
        default: break;
    }
}
/** @brief 通过 USART2 输出 IMU 串口命令列表。 */
static void ShowHelp(void)
{
    static unsigned char helpText[] =
        "WIT IMU commands (send one letter + CRLF):\r\n"
        "a: start accelerometer calibration\r\n"
        "m/e: start/stop magnetometer calibration\r\n"
        "u/U: set bandwidth to 5/256 Hz\r\n"
        "b/B: set baud rate to 9600/115200\r\n"
        "r/R: set output rate to 1/10 Hz\r\n"
        "c/C: output acceleration / acc+gyro+angle+mag\r\n"
        "h: show this help\r\n";
    Uart2Send(helpText, (unsigned int)(sizeof(helpText) - 1U));
}
static void SensorUartSend(uint8_t *p_data, uint32_t uiSize);
static void SensorDataUpdata(uint32_t uiReg, uint32_t uiRegNum);
static void Delayms(uint16_t ucMs);
void Usart2Init(unsigned int uiBaud);//函数的提前声明

/**
 * @brief 初始化 IMU SDK、USART2 和定时基础服务，并扫描传感器波特率。
 * @note 扫描过程包含轮询与延时，会阻塞初始化流程；不可在中断中调用。
 */
void imu_init(void)
{ 
	Timer_Init();
	Usart2Init(115200);
	WitInit(WIT_PROTOCOL_NORMAL, 0x50);
	WitSerialWriteRegister(SensorUartSend);
	WitRegisterCallBack(SensorDataUpdata);
	WitDelayMsRegister(Delayms);
	AutoScanSensor();
}

/** @brief 配置 USART2 的 PD5/PD6 收发复用和接收中断；uiBaud 单位为 bit/s。 */
void Usart2Init(unsigned int uiBaud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure; 

    // Enable clocks for GPIOD and USART2
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    
    // Configure PD5 as alternate function (TX)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; // Enable pull-up resistor
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    // Configure PD6 as alternate function (RX)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // No pull-up/pull-down for RX
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    // Connect pins to USART2
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);

    // USART configuration
    USART_InitStructure.USART_BaudRate = uiBaud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure); 

    // Enable RX interrupt
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    
    // Enable USART
    USART_Cmd(USART2, ENABLE);
    
	/*NVIC中断分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			//配置NVIC为分组2
	
    // NVIC configuration for USART2
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/** @brief 接收 USART2 字节并交给 WIT 协议解析器及命令行解析器处理。 */
void USART2_IRQHandler(void)
{
    unsigned char ucTemp;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        ucTemp = USART_ReceiveData(USART2);
        WitSerialDataIn(ucTemp);
        CopeCmdData(ucTemp);
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

/** @brief 通过 USART2 轮询发送指定长度数据，直到最后一字节发送完成后返回。 */
void Uart2Send(unsigned char *p_data, unsigned int uiSize)
{   
    unsigned int i;
    for (i = 0; i < uiSize; i++)
    {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
        USART_SendData(USART2, *p_data++);
    }
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
}

/** @brief 将 WIT SDK 的发送回调适配到本模块 USART2 发送函数。 */
static void SensorUartSend(uint8_t *p_data, uint32_t uiSize)
{
	Uart2Send(p_data, uiSize);
}

/** @brief 将 WIT SDK 的毫秒延时回调转接到工程延时服务。 */
static void Delayms(uint16_t ucMs)
{
	delay_ms(ucMs);
}

/** @brief 将 WIT SDK 的寄存器更新范围归类为加速度、陀螺仪、角度等更新标志。 */
static void SensorDataUpdata(uint32_t uiReg, uint32_t uiRegNum)
{
	int i;
    for(i = 0; i < uiRegNum; i++)
    {
        switch(uiReg)
        {
//            case AX:
//            case AY:
            case AZ:
				s_cDataUpdate |= ACC_UPDATE;
            break;
//            case GX:
//            case GY:
            case GZ:
				s_cDataUpdate |= GYRO_UPDATE;
            break;
//            case HX:
//            case HY:
            case HZ:
				s_cDataUpdate |= MAG_UPDATE;
            break;
//            case Roll:
//            case Pitch:
            case Yaw:
				s_cDataUpdate |= ANGLE_UPDATE;
            break;
            default:
				s_cDataUpdate |= READ_UPDATE;
			break;
        }
		uiReg++;
    }
}

/** @brief 逐个尝试预设波特率并读取寄存器，收到有效更新后结束扫描。 */
static void AutoScanSensor(void)
{
	int i, iRetry;
	
	for(i = 1; i < 10; i++)
	{
		Usart2Init(c_uiBaud[i]);
		realbaud = c_uiBaud[i];
		iRetry = 2;
		do
		{
			s_cDataUpdate = 0;
			WitReadReg(AX, 3);
			delay_ms(100);
			if(s_cDataUpdate != 0)
			{
				return ;
			}
			iRetry--;
		}while(iRetry);		
	}
}

/** @brief 将 IMU 原始寄存器换算为三轴加速度、角速度和角度数组；物理量单位依次为 g、度/秒和度。 */
void imu_scan(float* fAcc,float* fGyro,float* fAngle)
{   
	int i;
	float fAngle_tmp = 0;
		if(s_cDataUpdate)
		{
			for(i = 0; i < 3; i++)
			{
				fAcc[i] = sReg[AX+i] / 32768.0f * 16.0f;
				fGyro[i] = sReg[GX+i] / 32768.0f * 2000.0f;
			}
			if(sReg[TEMP] - sReg[Roll] > 10 | sReg[Roll] - sReg[TEMP] > 10)
			{
				fAngle[0] = sReg[Roll] / 32768.0f * 180.0f;
				fAngle_tmp = fAngle[0];
			}
			else   fAngle[0] = fAngle_tmp;
			
			for(i = 1; i<3;i++)
			{
				fAngle[i] = sReg[Roll+i] / 32768.0f * 180.0f;
			}
		}
}

