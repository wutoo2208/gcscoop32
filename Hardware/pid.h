#ifndef __PID_H
#define __PID_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h"
#include "LED.h"
#include "Emm_V5.h"

typedef struct
{
   	float kp, ki, kd; //三个系数
    float error, lastError; //误差、上次误差
    float integral, maxIntegral; //积分、积分限幅
    float output, maxOutput; //输出、输出限幅
}PID;

extern int flag;
extern PID mypid;
extern uint8_t dianji_move_flag;
extern uint8_t PID_control_flag;
extern float fAcc[3], fGyro[3], fAngle[3];
/** @brief 设置底盘运动分量、控制时长（ms）和目标航向角，供定时 PID 任务使用。 */
void PID_DIL(int v_x,int v_y,int w,uint32_t time,int target);

/** @brief 初始化 PID 默认参数和 TIM4 控制定时器。 */
void PIDInit(void);
/**
 * @brief 按给定目标执行阻塞式 PID 底盘运动。
 * @param time 控制时长，单位 ms；@p target 为目标航向角；@p pid_choose 选择 PID 参数组。
 * @return 当前实现完成后返回 1。
 * @note 内部等待完成标志且无超时，不可在中断服务程序中调用。
 */
uint8_t PID_move(int v_x,int v_y,int w,uint32_t time,float target,int pid_choose);
//单位是mm
void car_move_distance(float x,float y,int a_in,int speed_in);
/**
 * @brief 按 X/Y 位移（mm）和目标航向角执行闭环底盘移动。
 * @param angel 目标航向角；@p speed 为运动速度标度。
 * @note 通过 PID_move 执行，调用会等待控制任务完成。
 */
void PID_car_move_distance(float x,float y,float angel,float speed);

#endif
