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

//x��������ƣ���������ڿ���//y+�������ͼ�ڲ���
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
int task[17]; //�洢��ɫ��Ϣ
int ypcode[6]; //�洢��ɫ��Ϣ
float angle = 0;

void transfer_position_wukuai(uint8_t task_choose,float distance,uint8_t color);
void transfer_position_sehuan(uint8_t fenbian_choose,uint8_t task_num);

void fangzhi(uint8_t task); //ɫ��λ�÷������
void fangzhi_maduo(uint8_t task);  //���λ�÷������
void na(uint8_t task);   //ɫ��λ�������

void yuantai_na(uint8_t task);//Բ��λ�������
void yuantai_na1(uint8_t task);//Բ��λ������飬ʶ���λ
void yuantai_na2(uint8_t task);//Բ��λ������飬ʶ����龲ֹ����ʱץȡ

void wait_send_shumeipai(uint8_t message,uint16_t time);
void clear_ckp(void);
void jixie_reset(void);
char message[] = "Hello, BT - 04 from STM32F407!\r\n";
int main(void)
{	
/*
	���е��жϣ�
		1.OLEDÿ��500ms���½Ƕ����ݣ���ò����ã������ʱ�ܳ�������Ӱ��delay�����ĵ�����ȷ���
		2.delayms1����������ʱ��ùر������ж�
		3.PIDÿ���50ms����һ��PID���㣬��25ms�����е������ָ���ʱ�еȣ�
		4.IMUÿ�ν�����Ϣ����һ���ж�
		5.��ݮ�ɸ�stm32������Ϣʱ�����ж�
		6.���������λʱ������Ϣ�ж�
		7.�������ÿ��20ms��ʱ���ƽǶ�
*/
	delay_init(168);
	board_init();  //����can����Э��
	LED_Init();  //����LEDָʾ��
	OLED_Init();   //��ʱ������OLED //OLED�жϺ�TIM5����һ����������ͬʱ�ã�
	Timer6_Init(); //delayms1��ʱ��
	PWM_Init();	  //ʹ�ö�ʱ�����������жϣ�ʵʱ���ƶ���Ƕ�
	uart5_init(9600);
	uart6_init(9600);//�������õ�UART6
	Serial_Init();	//ͨѶ����1��ʼ������UART1
	Swith_Init();   //���س�ʼ��
	imu_init();     //ʹ��UART2
	PIDInit();		//�ö�ʱ��ʵ�ֶ�ʱ�ж�//����Ϊ50ms//���ж���ò�Ҫ������жϴ��
	Key_EXTI_Init();//�����ж�//����ݮ�ɷ�����������
  clear_ckp();    //��մ�����
		
		
		
		sprintf(tjcstr, "t0.txt=\"00000\"");
		HMISends(tjcstr);
		HMISendb(0xff);

	dianji_move_flag = 1;//ʹ�ܵ�����˶�
	TIM_SetCompare1(TIM3,0);  //�رղ����
	
	
//**************************���Դ���***************************//
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
		move_all(20.0f,1000,200   ,25,2000,220   ,295,13);//**********��һ�����յĶ���
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
		move_all(20.0f,1000,200   ,25,2000,220   ,295,13);//**********��һ�����յĶ���
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


//if task_choose = 0x01,��Ϊ���ת�̴������Բ��
//if task_choose = 0x02,��Ϊ�����⴦�����Բ��
//distance Ϊ���ĸ߶�
//color Ϊѡ�񶨱굽��һ������λ�� Ϊ0��ʾ�κ�λ�ö�����
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
	//����Ĵ����������ɫ�й�
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


//�����������������λɫ��λ��
//fenbian_choose
	//fenbian_choose == 0x01 ��ΪС�ֱ���
	//fenbian_choose == 0x02 ��Ϊ��ֱ���
//task_num 
	//ѡ���ƶ�����Ӧ��ɫ��λ�� Ϊ���ʾ����ɫ�޹�
void transfer_position_sehuan(uint8_t fenbian_choose,uint8_t task_num)
{
	x_origin = (Serial_RxPacket[0]-0x30)*1000+(Serial_RxPacket[1]-0x30)*100+(Serial_RxPacket[2]-0x30)*10+(Serial_RxPacket[3]-0x30);
	y_origin = (Serial_RxPacket[4]-0x30)*1000+(Serial_RxPacket[5]-0x30)*100+(Serial_RxPacket[6]-0x30)*10+(Serial_RxPacket[7]-0x30);
	float x_center,y_center;//x���������//y+�������
	if(fenbian_choose == 0x01){
		x_center =316;y_center =238;
		x_origin = x_origin - x_center;
		y_origin = y_origin - y_center;
		
		direct_x = x_origin * 1.1805555;    //ʵ�ʳߴ�ϵ��
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
	
		direct_x = x_origin * 0.29239766;    //ʵ�ʳߴ�ϵ��
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
	�ȷ�������ȴ�һ��ʱ�������û�з���
	��������ͣ�ֱ���յ���Ϣ�������˳�
*/
void wait_send_shumeipai(uint8_t message,uint16_t time)
{
	Serial_GetRxFlag();//��ձ�־λ
	Serial_SendByte(message);
	while(1){
		if(Serial_GetRxFlag() == 1){
			break;
		}
	}
}

//��ʱ��ʾOLED��LCD��ʾ��
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
	        HMISendb(0xff);//����ʾ��������ʾ����
}
