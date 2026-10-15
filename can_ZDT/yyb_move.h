#ifndef __YYB_MOVE_H
#define __YYB_MOVE_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "Emm_V5.h"

/** @brief 设置运动封装使用的电机加速度参数（沿用驱动协议标度）。 */
void change_A(int a_in);
/** @brief 设置底盘位置运动使用的加速度参数和速度参数；速度单位沿用电机驱动器标度。 */
void change_SNA1(int a,int speed);
/** @brief 按平移分量 v_x、v_y 和旋转分量 w 控制四轮速度；会同步发送多条 CAN 命令。 */
void car_move(int v_x,int v_y,int w);
/**
 * @brief 将升降轴移动到目标坐标。
 * @param target_pos 目标坐标，当前实现范围为 0~135。
 * @param speed 速度参数；@p accel 为加速度参数，均使用驱动器标度。
 * @return 估算等待时间，单位 ms。
 */
double shengjiang_control(int target_pos, uint16_t speed, uint8_t accel);
/**
 * @brief 将平推轴移动到目标位置。
 * @param target_pos 目标位置，单位 mm，当前实现范围为 -122~65。
 * @param speed 速度参数；@p accel 为加速度参数，均使用驱动器标度。
 * @return 估算等待时间，单位 ms。
 */
double pingtui_control(float target_pos, uint16_t speed, uint8_t accel);
/** @brief 命令平推轴回到程序坐标零位。 */
void pingtui_reset(void);
/** @brief 按 X 轴坐标约定发送底盘位置运动命令；x 的单位为 mm。 */
void car_move_distance_x(float x);
/** @brief 按 Y 轴坐标约定发送底盘位置运动命令；y 的单位为 mm。 */
void car_move_distance_y(float y);
/** @brief 执行 Y 向位置移动并等待估算时长；当前实现使用 y，x 参数未参与计算。 */
void car_move1(float x,float y,int a_in,int car_speed);
/** @brief 执行 X 向位置移动并等待估算时长；当前实现使用 x，y 参数未参与计算。 */
void car_move2(float x,float y,int a_in,int car_speed);
/** @brief 执行 Y 向位置移动并立即返回，不等待估算的运动完成时间。 */
void car_move3(float x,float y,int a_in,int car_speed);
/** @brief 执行 X 向位置移动并立即返回，不等待估算的运动完成时间。 */
void car_move4(float x,float y,int a_in,int car_speed);
/**
 * @brief 根据位移、速度和加速度估算运动等待时间。
 * @param distance 位移，单位 mm；@p direct 选择当前实现中的 X/Y 轴参数。
 * @return 估算时长，单位 ms。
 */
double car_move_delay(float distance,uint16_t a_of_car,uint16_t speed_of_car,uint8_t direct);

#endif
