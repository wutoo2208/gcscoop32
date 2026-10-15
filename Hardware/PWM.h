#ifndef __PWM_H
#define __PWM_H

// 定义伺服机构参数结构体
typedef struct {
    float start_angle;    	//从什么角度开始
    float target_angle;	  	//目标是旋转多少角度
    float current_angle;  	//当前的角度是多少
    int moving_flag;    // 插值更新标志，1: 正在运动；0: 空闲
    int t;            // 时间变量，用于插值计算
	int time;		  //整个插值运动所需的时间
    void (*SetServoAngle)(float angle);  // 设置当前角度的函数指针
} Servo_t;

/** @brief 初始化 TIM2 舵机 PWM 输出及周期更新中断。 */
void PWM_Init(void);
/** @brief 设置 TIM2 通道 2 的比较值；Compare 为定时器计数值。 */
void PWM_SetCompare2(uint16_t Compare);
/** @brief 设置 TIM2 通道 3 的比较值；Compare 为定时器计数值。 */
void PWM_SetCompare3(uint16_t Compare);
/** @brief 设置 TIM2 通道 4 的比较值；Compare 为定时器计数值。 */
void PWM_SetCompare4(uint16_t Compare);


/** @brief 将机械夹爪移动到闭合/夹取姿态；当前实现等待舵机运动完成。 */
void set_zhuashou_he(void);
/** @brief 将机械夹爪移动到张开姿态；当前实现等待舵机运动完成。 */
void set_zhuashou_kai(void);
/** @brief 设置转盘/云台目标角度（度）和运动时长（ms）；该接口异步返回。 */
void set_yuantai_Angle(float target, float time);
/** @brief 设置夹爪目标角度（度）；第二参数实际作为运动时长（ms），调用会阻塞等待。 */
void set_zhuashou_Angle(float target_abs, float speed);
/** @brief 设置物块平台目标角度（度）；第二参数实际作为运动时长（ms），该接口异步返回。 */
void set_wukuaipingtai_Angle(float target_abs, float speed);
/** @brief 按位置编号 1~3 将物块平台舵机转到对应放置位。 */
void set_wukuaipingtai_weizhi(int pos);

/**
 * @brief 下发平推、升降和云台目标，并等待估算的最长运动时间。
 * @param target_pos 平推目标位置（mm）；@p target_pos1 为升降目标坐标。
 * @param target 云台目标角度（度）；速度/加速度参数沿用各驱动器标度。
 * @return 估算等待时长，单位 ms。
 * @note 当前实现会覆盖部分速度/加速度入参，speed_of_yuntai 未参与计算。
 */
int move_all(float target_pos, uint16_t speed, uint8_t accel,float target_pos1, uint16_t speed1, uint8_t accel1,float target, float speed_of_yuntai);
/**
 * @brief 下发平推、升降和云台目标后立即返回预计最长运动时长。
 * @param target_pos 平推目标位置（mm）；@p target_pos1 为升降目标坐标。
 * @param target 云台目标角度（度）；速度/加速度参数沿用各驱动器标度。
 * @return 估算时长，单位 ms。
 * @note 当前实现会覆盖部分速度/加速度入参，speed_of_yuntai 未参与计算。
 */
int move_all1(float target_pos, uint16_t speed, uint8_t accel,float target_pos1, uint16_t speed1, uint8_t accel1,float target, float speed_of_yuntai);
#endif
