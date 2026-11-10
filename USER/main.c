#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "board.h"
#include "Emm_V5.h"
#include "uart6_ckp.h"
#include "Serial.h"
#include "yyb_move.h"
#include "imu.h"
#include "PWM.h"
#include "LED.h"
#include "OLED.h"
#include "swith.h"
#include "pid.h"
#include "usart5.h"
#include "wit_c_sdk.h"
#include "exti.h"

//x减物块右移（从外侧往内看）//y+物块往地图内侧走
#define  x_center_red       955
#define  y_center_red		519

#define  x_center_green		955
#define  y_center_green		519

#define  x_center_blue		955
#define  y_center_blue		519

char tjcstr[100];
int acc[4];
float direct_x = 0;
float direct_y = 0;
float distance_wukuai = 150;
float x_origin = 0;
float y_origin = 0;
int task[17]; //存储颜色信息
int ypcode[6]; //存储颜色信息
float angle = 0;

void transfer_position_wukuai(uint8_t task_choose,float distance,uint8_t color);
void transfer_position_sehuan(uint8_t fenbian_choose,uint8_t task_num);

void fangzhi(uint8_t task); //色环位置放置物块
void fangzhi_maduo(uint8_t task);  //码垛位置放置物块
void na(uint8_t task);   //色环位置拿物块

void yuantai_na(uint8_t task);//圆盘位置拿物块
void yuantai_na1(uint8_t task);//圆盘位置拿物块，识别就位
void yuantai_na2(uint8_t task);//圆盘位置拿物块，识别到物块静止，此时抓取

void wait_send_shumeipai(uint8_t message,uint16_t time);
void clear_ckp(void);
void jixie_reset(void);
char message[] = "Hello, BT - 04 from STM32F407!\r\n";
int main(void)
{	
/*
	所有的中断：
		1.OLED每隔500ms更新角度数据（最好不启用，里面耗时很长）（会影响delay函数的调用正确与否）
		2.delayms1函数在启用时最好关闭所有中断
		3.PID每间隔50ms进行一次PID计算，花25ms给所有电机发送指令（耗时中等）
		4.IMU每次接受信息触发一次中断
		5.树莓派给stm32发送信息时触发中断
		6.步进电机到位时发送信息中断
		7.舵机控制每隔20ms定时控制角度
*/
	delay_init(168);
	board_init();  //配置can总线协议
	LED_Init();  //配置LED指示灯
	OLED_Init();   //暂时不启用OLED //OLED中断和TIM5绑定在一起（两个必须同时用）
	Timer6_Init(); //delayms1的时钟
	PWM_Init();	  //使用定时器二，开启中断，实时控制舵机角度
	uart5_init(9600);
	uart6_init(9600);//串口屏用的UART6
	Serial_Init();	//通讯串口1初始化，用UART1
	Swith_Init();   //开关初始化
	imu_init();     //使用UART2
	PIDInit();		//用定时器实现定时中断//周期为50ms//该中断最好不要被别的中断打断
	Key_EXTI_Init();//按键中断//向树莓派发送重启命令
  clear_ckp();    //清空串口屏
		
		
		
		sprintf(tjcstr, "t0.txt=\"00000\"");
		HMISends(tjcstr);
		HMISendb(0xff);

	dianji_move_flag = 1;//使能电机的运动
	TIM_SetCompare1(TIM3,0);  //关闭补光灯
	
	
//**************************测试代码***************************//
//car_move1(0,1000,150,200);
//car_move2(150,0,150,150);
//delay_ms(3000);



//set_zhuashou_kai();
//delay_ms (3000);
//set_zhuashou_he();

//	move_all(-48.0f,1000,200   ,25,1000,200   ,135,13);
	
//	move_all(45.0f,1000,200   ,45,1000,200   ,190,13);
//	move_all(3.0f+5.8f,1000,200   ,132,1000,200   ,317.0f+1.4f,13);
		set_zhuashou_kai(); 
		delay_ms(1000);
		set_zhuashou_he();  
		move_all(-45.0f,1000,220   ,0,2000,220   ,130,13);
		move_all(-45.0f,1000,220   ,10,2000,220   ,130,13);
		
//		move_all(-48.0f,1000,200   ,0,2000,220   ,130,13);
//		move_all(-48.0f,1000,200   ,0,2000,220   ,130,13);
		set_zhuashou_kai();	 



}

void fangzhi_green(void)   //2
{
	move_all(-48.0f,1000,200   ,25,1000,200   ,135,13);
	move_all(-20.0f,1000,200   ,25,1000,200   ,135,13);
	move_all(-20.0f,1000,200   ,25,1000,200   ,278,13);
	move_all(-50.0f,1000,200   ,132,1000,200   ,278,13);
	set_zhuashou_kai();
}

void fangzhi_red(void)   //3
{
	move_all(-48.0f,1000,200   ,25,1000,200   ,135,13);
	move_all(3.0f+1.0f,1000,200   ,25,1000,200   ,236.0f,13);
	move_all(3.0f+1.0f,1000,200   ,132,1000,200   ,236.0f,13);
	
	set_zhuashou_kai();
}
void fangzhi_blue(void)   //1
{
	move_all(-48.0f,1000,200   ,25,1000,200   ,135,13);
	move_all(3.0f+5.8f,1000,200   ,25,1000,200   ,317.0f+1.4f,13);
	move_all(3.0f+5.8f,1000,200   ,132,1000,200   ,317.0f+1.4f,13);
	set_zhuashou_kai();
}

void fangzhi(uint8_t task)
{
	if(task == 0x01){
		fangzhi_blue();
	}else if(task == 0x02){
		fangzhi_green();
	}else if(task == 0x03){
		fangzhi_red();
	}
}

void fangzhi_maduo(uint8_t task)
{
	if(task == 0x03){   //red   3
		move_all(-48.0f,1000,200   ,25,1000,200   ,135,13);
		move_all(3.0f,1000,200   ,25,1000,200   ,236,13);
		move_all(3.0f,1000,200   ,130-70,1000,200   ,236,13);
		set_zhuashou_kai();
	}else if(task == 0x02){   //green   2
		move_all(-48.0f,1000,200   ,25,1000,200   ,135,13);
		move_all(-48.0f,1000,200   ,25,1000,200   ,180,13);
		move_all(-120.0f,1000,200   ,25,1000,200   ,278,13);
		move_all(-50.0f,1000,200   ,130-70,1000,200   ,278,13);
		set_zhuashou_kai();
	}else if(task == 0x01){   //blue   1
		move_all(-48.0f,1000,200   ,25,1000,200   ,135,13);
		move_all(-48.0f,1000,200   ,25,1000,200   ,180,13);
		move_all(-120.0f,1000,200   ,25,1000,200   ,317,13);
		move_all(3.0f,1000,200   ,130-70,1000,200   ,317,13);
		set_zhuashou_kai();
	}
}

void na_green(void)
{	
	move_all(-10.0f,1000,200   ,130,1000,200   ,278,13);
	move_all(-50.0f,1000,200   ,130,1000,200   ,278,13);
	set_zhuashou_he();
	move_all(-50.0f,1000,200   ,50,1000,220   ,278,13);
	move_all(-48.0f,1000,200   ,25,1000,220   ,135,13);
	move_all(-48.0f,1000,200   ,33,1000,220   ,135,13);
	set_zhuashou_kai();
}

void na_blue(void)
{
	move_all(3.0f,1000,200   ,110,1000,200   ,317,13);
	move_all(3.0f,1000,200   ,130,1000,200   ,317,13);
	set_zhuashou_he();
	move_all(8.0f,1000,200   ,25,1000,220   ,317,13);
	move_all(-48.0f,1000,200   ,25,1000,220   ,135,13);
	move_all(-48.0f,1000,200   ,33,1000,220   ,135,13);
	set_zhuashou_kai();
}

void na_red(void)
{
	move_all(3.0f,1000,200   ,130,1000,200   ,236,13);
	set_zhuashou_he();
	move_all(-50.0f,1000,200   ,50,1000,220   ,236,13);
	move_all(-48.0f,1000,200   ,25,1000,220   ,135,13);
	move_all(-48.0f,1000,200   ,33,1000,220   ,135,13);
	set_zhuashou_kai();
}

void na(uint8_t task)
{
	if(task == 0x01){
		na_blue();
	}else if(task == 0x02){
		na_green();
	}else if(task == 0x03){
		na_red();
	}
}

void yuantai_na(uint8_t task)
{
	if(task == 0x01){
		set_zhuashou_kai();  
		move_all(-100.0f,1000,220   ,50,2000,220   ,278,13);
		set_zhuashou_he();  
		move_all(-48.0f,1000,200   ,25,2000,220   ,135,13);
		move_all(-48.0f,1000,200   ,33,2000,220   ,135,13);
		set_zhuashou_kai();	 
	}else if(task == 0x02){
		set_zhuashou_kai();	
		move_all(63.0f,1000,200   ,48,2000,220   ,295,13);
		set_zhuashou_he();   
		move_all(20.0f,1000,200   ,25,2000,220   ,295,13);//**********多一个回收的动作
		move_all(-20.0f,1000,200   ,25,2000,220   ,135,13);
		move_all(-48.0f,1000,200   ,25,2000,220   ,135,13);
		move_all(-48.0f,1000,200   ,33,2000,220   ,135,13);
		set_zhuashou_kai();  
	}else if(task == 0x03){
		set_zhuashou_kai();  
		move_all(63.0f,1000,200   ,48,2000,220   ,261,13);
		set_zhuashou_he();  
		move_all(-48.0f,1000,200   ,25,2000,220   ,135,13);
		move_all(-48.0f,1000,200   ,33,2000,220   ,135,13);
		set_zhuashou_kai();  
	}
}

void yuantai_na1(uint8_t task)
{
	if(task == 0x01){
		set_zhuashou_kai();
		move_all(40.0f,1000,200   ,0,2000,200   ,278,13);
	}else if(task == 0x02){
		set_zhuashou_kai();	
		move_all(63.0f,1000,200   ,18,2000,220   ,295,13);
	}else if(task == 0x03){
		set_zhuashou_kai();  
		move_all(63.0f,1000,200   ,18,2000,220   ,261,13);
	}
}

void yuantai_na2(uint8_t task)
{
	if(task == 0x01){
		move_all(-100.0f,1000,220   ,50,2000,220   ,278,13);
		set_zhuashou_he();   
		move_all(-48.0f,1000,200   ,25,2000,220   ,135,13);
		move_all(-48.0f,1000,200   ,33,2000,220   ,135,13);
		set_zhuashou_kai();	 
	}else if(task == 0x02){
		move_all(63.0f,1000,200   ,48,2000,220   ,295,13);
		set_zhuashou_he();   
		move_all(20.0f,1000,200   ,25,2000,220   ,295,13);//**********多一个回收的动作
		move_all(-20.0f,1000,200   ,25,2000,220   ,135,13);
		move_all(-48.0f,1000,200   ,25,2000,220   ,135,13);
		move_all(-48.0f,1000,200   ,33,2000,220   ,135,13);
		set_zhuashou_kai();  
	}else if(task == 0x03){
		move_all(63.0f,1000,200   ,48,2000,220   ,261,13);
		set_zhuashou_he();  
		move_all(-48.0f,1000,200   ,25,2000,220   ,135,13);
		move_all(-48.0f,1000,200   ,33,2000,220   ,135,13);
		set_zhuashou_kai();
	}
}


//if task_choose = 0x01,则为检测转盘处的物块圆心
//if task_choose = 0x02,则为检测码垛处的物块圆心
//distance 为物块的高度
//color 为选择定标到哪一个物块的位置 为0表示任何位置都可以
void transfer_position_wukuai(uint8_t task_choose,float distance,uint8_t color)
{
	x_origin = (Serial_RxPacket[0]-0x30)*1000+(Serial_RxPacket[1]-0x30)*100+(Serial_RxPacket[2]-0x30)*10+(Serial_RxPacket[3]-0x30);
	y_origin = (Serial_RxPacket[4]-0x30)*1000+(Serial_RxPacket[5]-0x30)*100+(Serial_RxPacket[6]-0x30)*10+(Serial_RxPacket[7]-0x30);
	float bilichi = 0;
	if(task_choose == 0x01){
		bilichi = -0.0035f*(distance+80.0f) + 1.1967f;
		x_origin = x_origin - 322;
		y_origin = y_origin - 165;
	}else if(task_choose == 0x02){
		bilichi = -0.0033f*(distance) + 1.1655f;
		x_origin = x_origin - 314;
		y_origin = y_origin - 238;
	}
	direct_x = x_origin * bilichi;   
	direct_y = y_origin * bilichi;
	//下面的代码仅仅和颜色有关
	if(color == 2){
		if(Serial_RxPacket[8] == 0x32){
		}else if(Serial_RxPacket[8] == 0x31){
				direct_x = direct_x + distance_wukuai;
		}else if(Serial_RxPacket[8] == 0x33){
				direct_x = direct_x - distance_wukuai;
		}
	}else if(color == 1){
		if(Serial_RxPacket[8] == 0x31){
		}else if(Serial_RxPacket[8] == 0x32){
			direct_x = direct_x - distance_wukuai;
		}else if(Serial_RxPacket[8] == 0x33){
				direct_x = direct_x - 2*distance_wukuai;
		}
	}else if(color == 3){
		if(Serial_RxPacket[8] == 0x33){
		}else if(Serial_RxPacket[8] == 0x32){
			direct_x = direct_x + distance_wukuai;
		}else if(Serial_RxPacket[8] == 0x31){
				direct_x = direct_x + 2*distance_wukuai;
		}
	}
	if(Serial_RxPacket[8] == 0x34){
		direct_y = 5;direct_x = -5;
	}
}


//与任务码相关联，定位色环位置
//fenbian_choose
	//fenbian_choose == 0x01 则为小分辨率
	//fenbian_choose == 0x02 则为大分辨率
//task_num 
	//选择移动到对应颜色的位置 为零表示与颜色无关
void transfer_position_sehuan(uint8_t fenbian_choose,uint8_t task_num)
{
	x_origin = (Serial_RxPacket[0]-0x30)*1000+(Serial_RxPacket[1]-0x30)*100+(Serial_RxPacket[2]-0x30)*10+(Serial_RxPacket[3]-0x30);
	y_origin = (Serial_RxPacket[4]-0x30)*1000+(Serial_RxPacket[5]-0x30)*100+(Serial_RxPacket[6]-0x30)*10+(Serial_RxPacket[7]-0x30);
	float x_center,y_center;//x加物块右移//y+物块下移
	if(fenbian_choose == 0x01){
		x_center =316;y_center =238;
		x_origin = x_origin - x_center;
		y_origin = y_origin - y_center;
		
		direct_x = x_origin * 1.1805555;    //实际尺寸系数
		direct_y = y_origin * 1.1805555;
	}else if(fenbian_choose == 0x02){
		if(Serial_RxPacket[8] == 0x31){
			x_center = x_center_red;y_center = y_center_red;
		}else if(Serial_RxPacket[8] == 0x32){
			x_center = x_center_green;y_center = y_center_green;
		}else if(Serial_RxPacket[8] == 0x33){
			x_center = x_center_blue;y_center = y_center_blue;
		}
		x_origin = x_origin - x_center;
		y_origin = y_origin - y_center;
	
		direct_x = x_origin * 0.29239766;    //实际尺寸系数
		direct_y = y_origin * 0.29239766;
	}
	
	if(task_num == 2){
		if(Serial_RxPacket[8] == 0x32){
		}else if(Serial_RxPacket[8] == 0x31){
				direct_x = direct_x + distance_wukuai;
		}else if(Serial_RxPacket[8] == 0x33){
				direct_x = direct_x - distance_wukuai;
		}
	}else if(task_num == 1){
		if(Serial_RxPacket[8] == 0x31){
		}else if(Serial_RxPacket[8] == 0x32){
			direct_x = direct_x - distance_wukuai;
		}else if(Serial_RxPacket[8] == 0x33){
				direct_x = direct_x - 2*distance_wukuai;
		}
	}else if(task_num == 3){
		if(Serial_RxPacket[8] == 0x33){
		}else if(Serial_RxPacket[8] == 0x32){
			direct_x = direct_x + distance_wukuai;
		}else if(Serial_RxPacket[8] == 0x31){
				direct_x = direct_x + 2*distance_wukuai;
		}
	}
	if(Serial_RxPacket[8] == 0x34){
				direct_y = 3;direct_x = -3;
	}
}

void jixie_reset(void)
{
	while(swith_out() == 1){
		LED_Toggle2();
		delay_ms1(50);
	}
	
	set_zhuashou_kai();
	move_all(0.0f,1000,200   ,0,1000,200   ,68.4,13);
	set_wukuaipingtai_Angle(20.0f,300);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	TIM_Cmd(TIM5, ENABLE); 
	while(1){}
}

/*
	先发送命令，等待一段时间如果还没有返回
	则继续发送，直到收到信息，立刻退出
*/
void wait_send_shumeipai(uint8_t message,uint16_t time)
{
	Serial_GetRxFlag();//清空标志位
	Serial_SendByte(message);
	while(1){
		if(Serial_GetRxFlag() == 1){
			break;
		}
	}
}

//定时显示OLED和LCD显示屏
void TIM5_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) == SET)
	{
		imu_scan(fAcc, fGyro, fAngle);
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
		OLED_ShowSignedNum(1,1,fAngle[2],3);
		OLED_ShowChar(1,5,'.');
		OLED_ShowNum(1,6,ABS((fAngle[2]-(int)fAngle[2])*1000),3);
		OLED_ShowSignedNum(2,1,angle,3);
		OLED_ShowChar(2,5,'.');
		OLED_ShowNum(2,6,ABS((angle-(int)angle)*1000),3);
		
	}
}

void clear_ckp(void)
{
	sprintf(tjcstr, "t0.txt=\"  \"");
			HMISends(tjcstr);
	        HMISendb(0xff);//在显示屏上面显示任务
}
