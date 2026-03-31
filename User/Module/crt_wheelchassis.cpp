/**
 * @file crt_chassis.h
 * @author 
 * @brief 底盘电控
 * @version 0.1
 * @date 2026-2-23 0.1 26赛季定稿
 *
 * @copyright 
 *
 */

#include "crt_wheelchassis.h"
#include <math.h>

// 全向轮底盘控制类实现

/**
 * @brief 底盘初始化
 *
 * @param __Speed 底盘速度限制最大值
 */
void Class_Chassis_Omni::Init()
{
    // PID初始化
   for (int i = 0; i < 4; i++)
    {
        Chassis_Motor[i].PID_Omega.Init(83.3f, 97.3f, 0.0f, 0.0f, 10000.0f, 12000.0f); 
    }
    
    // 电机初始化
    Chassis_Motor[0].Init(&hcan1, CAN_Motor_ID_0x201, Control_Method_OMEGA, 19.0f, 20.0f);
    Chassis_Motor[1].Init(&hcan1, CAN_Motor_ID_0x202, Control_Method_OMEGA, 19.0f, 20.0f);
    Chassis_Motor[2].Init(&hcan1, CAN_Motor_ID_0x203, Control_Method_OMEGA, 19.0f, 20.0f);
    Chassis_Motor[3].Init(&hcan1, CAN_Motor_ID_0x204, Control_Method_OMEGA, 19.0f, 20.0f);
}

/**
 * @brief TIM定时器中断控制回调函数
 *
 */
void Class_Chassis_Omni::TIM_1ms_Control_PeriodElapsedCallback()
{
    Update_Now_State_From_Motor_Feedback();

    Kinematics_Inverse_Resolution();

    Output_To_Motor();
}

/**
 * @brief 由电机反馈更新底盘当前状态(全向轮)
 */
void Class_Chassis_Omni::Update_Now_State_From_Motor_Feedback()
{
    float w0 = Chassis_Motor[0].Get_Now_Omega();
    float w1 = Chassis_Motor[1].Get_Now_Omega();
    float w2 = Chassis_Motor[2].Get_Now_Omega();
    float w3 = Chassis_Motor[3].Get_Now_Omega();

    // 由逆解矩阵反推底盘速度
    Now_Velocity_X = (w2 + w3 - w0 - w1) * (Wheel_Radius / (4.0f * SQRT2_DIV_2));
    Now_Velocity_Y = (w1 + w2 - w0 - w3) * (Wheel_Radius / (4.0f * SQRT2_DIV_2));
    Now_Omega = -(w0 + w1 + w2 + w3) * Wheel_Radius / (4.0f * Chassis_L);
}

/**
 * @brief 运动学逆解算
 *
 */
void Class_Chassis_Omni::Kinematics_Inverse_Resolution()
{ 
    // 全向轮运动学逆解 Vx、Vy m/s 转 rad/s；Wz rad/s 转 rad/s
    Target_Wheel_Omega[0] = (-Target_Velocity_Y * SQRT2_DIV_2 - Target_Velocity_X * SQRT2_DIV_2 - Target_Omega * Chassis_L)/Wheel_Radius;
    Target_Wheel_Omega[1] = (+Target_Velocity_Y * SQRT2_DIV_2 - Target_Velocity_X * SQRT2_DIV_2 - Target_Omega * Chassis_L)/Wheel_Radius;
    Target_Wheel_Omega[2] = (+Target_Velocity_Y * SQRT2_DIV_2 + Target_Velocity_X * SQRT2_DIV_2 - Target_Omega * Chassis_L)/Wheel_Radius;
    Target_Wheel_Omega[3] = (-Target_Velocity_Y * SQRT2_DIV_2 + Target_Velocity_X * SQRT2_DIV_2 - Target_Omega * Chassis_L)/Wheel_Radius;
}

/**
 * @brief 输出控制量到电机
 *
 */
void Class_Chassis_Omni::Output_To_Motor()
{
    for (int i = 0; i < 4; i++)
    {
        Chassis_Motor[i].Set_Target_Omega(Target_Wheel_Omega[i]);
    }
}




// 麦轮底盘控制类实现

/**
 * @brief 底盘初始化
 *
 * @param __Speed 底盘速度限制最大值
 */
void Class_Chassis_Mecanum::Init()
{
    // PID初始化
   for (int i = 0; i < 4; i++)
    {
        if (i == 0 || i == 2) // 前左和后右轮
        {
            Chassis_Motor[i].PID_Omega.Init(83.3f, 97.3f, 0.0f, 0.0f, 10000.0f, 12000.0f);
        }
        else // 前右和后左轮
        {
            Chassis_Motor[i].PID_Omega.Init(83.3f, 97.3f, 0.0f, 0.0f, 10000.0f, 12000.0f);
        }    
    }
    
    // 电机初始化
    Chassis_Motor[0].Init(&hcan1, CAN_Motor_ID_0x201, Control_Method_OMEGA, 19.0f, 20.0f);
    Chassis_Motor[1].Init(&hcan1, CAN_Motor_ID_0x202, Control_Method_OMEGA, 19.0f, 20.0f);
    Chassis_Motor[2].Init(&hcan1, CAN_Motor_ID_0x203, Control_Method_OMEGA, 19.0f, 20.0f);
    Chassis_Motor[3].Init(&hcan1, CAN_Motor_ID_0x204, Control_Method_OMEGA, 19.0f, 20.0f);
}

/**
 * @brief TIM定时器中断控制回调函数
 *
 */
void Class_Chassis_Mecanum::TIM_1ms_Control_PeriodElapsedCallback()
{
    Update_Now_State_From_Motor_Feedback();

    Kinematics_Inverse_Resolution();

    Output_To_Motor();
}   

/**
 * @brief 由电机反馈更新底盘当前状态(麦轮)
 */
void Class_Chassis_Mecanum::Update_Now_State_From_Motor_Feedback()
{
    float w0 = Chassis_Motor[0].Get_Now_Omega();
    float w1 = Chassis_Motor[1].Get_Now_Omega();
    float w2 = Chassis_Motor[2].Get_Now_Omega();
    float w3 = Chassis_Motor[3].Get_Now_Omega();
    float k = (Chassis_L + Chassis_W) / 2.0f;

    // 由逆解矩阵反推底盘速度
    Now_Velocity_X = (w2 + w3 - w0 - w1) * Wheel_Radius * 0.25f;
    Now_Velocity_Y = (w2 - w3 - w1 + w0) * Wheel_Radius * 0.25f;
    Now_Omega = (w0 - w1 - w2 + w3) * Wheel_Radius / (4.0f * k);
}

/**
 * @brief 运动学逆解算
 *
 */
void Class_Chassis_Mecanum::Kinematics_Inverse_Resolution()
{
    // 旋转解算系数 k = (L/2 + W/2)
    float k = (Chassis_L + Chassis_W) / 2;

     // 标准X型麦克纳姆轮解算公式
    // 左前轮 (motor_chassis[0])
    Target_Wheel_Omega[0] = (-(Target_Velocity_X - Target_Velocity_Y - Target_Omega * k)) / Wheel_Radius;

    // 右前轮 (motor_chassis[1])
    Target_Wheel_Omega[1] = (-(Target_Velocity_X + Target_Velocity_Y + Target_Omega * k)) / Wheel_Radius;

    // 左后轮 (motor_chassis[2])
    Target_Wheel_Omega[2] = (Target_Velocity_X + Target_Velocity_Y - Target_Omega * k) / Wheel_Radius;

    // 右后轮 (motor_chassis[3])
    Target_Wheel_Omega[3] = (Target_Velocity_X - Target_Velocity_Y + Target_Omega * k) / Wheel_Radius;
}


/**
 * @brief 输出控制量到电机
 *
 */
void Class_Chassis_Mecanum::Output_To_Motor()
{
    for (int i = 0; i < 4; i++)
    {
        Chassis_Motor[i].Set_Target_Omega(Target_Wheel_Omega[i]);
    }
}


// 舵轮底盘控制类实现

/**
 * @brief 底盘初始化
 *
 * @param __Speed 底盘速度限制最大值
 */
void Class_Chassis_Steering::Init()
{
    // PID初始化
   for (int i = 0; i < 4; i++)
    {
        // 没有调试过，先和全向轮一样，后续根据实际情况调整
        Steering_Motor[i].PID_Omega.Init(83.3f, 97.3f, 0.0f, 0.0f, 10000.0f, 12000.0f);
        Chassis_Motor[i].PID_Omega.Init(83.3f, 97.3f, 0.0f, 0.0f, 10000.0f, 12000.0f);
    }
    
    // 电机初始化
    // 舵轮电机ID 0x205~0x208，控制方式角度控制，减速比和最大扭矩没改
    Steering_Motor[0].Init(&hcan1, CAN_Motor_ID_0x205, Control_Method_ANGLE, 19.0f, 20.0f);
    Steering_Motor[1].Init(&hcan1, CAN_Motor_ID_0x206, Control_Method_ANGLE, 19.0f, 20.0f);
    Steering_Motor[2].Init(&hcan1, CAN_Motor_ID_0x207, Control_Method_ANGLE, 19.0f, 20.0f);
    Steering_Motor[3].Init(&hcan1, CAN_Motor_ID_0x208, Control_Method_ANGLE, 19.0f, 20.0f);

    // 轮向电机ID 0x201~0x204，控制方式速度控制
    Chassis_Motor[0].Init(&hcan1, CAN_Motor_ID_0x201, Control_Method_OMEGA, 19.0f, 20.0f);
    Chassis_Motor[1].Init(&hcan1, CAN_Motor_ID_0x202, Control_Method_OMEGA, 19.0f, 20.0f);
    Chassis_Motor[2].Init(&hcan1, CAN_Motor_ID_0x203, Control_Method_OMEGA, 19.0f, 20.0f);
    Chassis_Motor[3].Init(&hcan1, CAN_Motor_ID_0x204, Control_Method_OMEGA, 19.0f, 20.0f);
}

/**
 * @brief TIM定时器中断控制回调函数
 *
 */
void Class_Chassis_Steering::TIM_1ms_Control_PeriodElapsedCallback()
{
    Update_Now_State_From_Motor_Feedback();

    Kinematics_Inverse_Resolution();

    Output_To_Motor();
}

/**
 * @brief 由电机反馈更新底盘当前状态(舵轮)
 */
void Class_Chassis_Steering::Update_Now_State_From_Motor_Feedback()
{
    float velocity_x_sum = 0.0f;
    float velocity_y_sum = 0.0f;
    float omega_sum = 0.0f;

    for (int i = 0; i < 4; i++)
    {
        float wheel_omega = Chassis_Motor[i].Get_Now_Omega();
        float wheel_speed = wheel_omega * Wheel_Radius;
        float steer_angle = Steering_Motor[i].Get_Now_Angle();

        float wheel_vx = wheel_speed * cosf(steer_angle);
        float wheel_vy = wheel_speed * sinf(steer_angle);

        velocity_x_sum += wheel_vx;
        velocity_y_sum += wheel_vy;

        float lever_x = Wheel_To_Core_Distance[i] * cosf(Wheel_Azimuth[i]);
        float lever_y = Wheel_To_Core_Distance[i] * sinf(Wheel_Azimuth[i]);
        float lever_norm2 = lever_x * lever_x + lever_y * lever_y;

        if (lever_norm2 > 1e-6f)
        {
            omega_sum += ((-lever_y) * wheel_vx + lever_x * wheel_vy) / lever_norm2;
        }
    }

    Now_Velocity_X = velocity_x_sum * 0.25f;
    Now_Velocity_Y = velocity_y_sum * 0.25f;
    Now_Omega = omega_sum * 0.25f;
}

/**
 * @brief 运动学逆解算
 *
 */
void Class_Chassis_Steering::Kinematics_Inverse_Resolution()
{
    // 舵轮底盘运动学逆解算，计算每个舵轮的目标角度和每个轮向电机的目标速度
    for (int i = 0; i < 4; i++)
    {
        float tmp_velocity_x, tmp_velocity_y, tmp_velocity_modulus;

        // 解算到每个轮组的具体线速度
        tmp_velocity_x = Target_Velocity_X - Target_Omega * Wheel_To_Core_Distance[i] * sinf(Wheel_Azimuth[i]);
        tmp_velocity_y = Target_Velocity_Y + Target_Omega * Wheel_To_Core_Distance[i] * cosf(Wheel_Azimuth[i]);
        arm_sqrt_f32(tmp_velocity_x * tmp_velocity_x + tmp_velocity_y * tmp_velocity_y, &tmp_velocity_modulus);

        // 根据线速度决定轮向电机角速度
        Target_Wheel_Omega[i] = tmp_velocity_modulus / Wheel_Radius;

        // 根据速度的xy分量分别决定舵向电机角度
        if (tmp_velocity_modulus == 0.0f)
        {
            // 排除除零问题
            Target_Steer_Angle[i] = Steering_Motor[i].Get_Now_Angle();
        }
        else
        {
            // 没有除零问题
            Target_Steer_Angle[i] = atan2f(tmp_velocity_y, tmp_velocity_x);
        }
    }

    _Steer_Motor_Kinematics_Nearest_Transposition();
}

/**
 * @brief 舵向电机依照轮向电机目标角速度就近转位
 *
 */
void Class_Chassis_Steering::_Steer_Motor_Kinematics_Nearest_Transposition()
{
    // 舵向电机依照轮向电机目标角速度就近转位，计算每个舵轮的目标角度，使得舵轮转动最小角度达到目标速度方向
    for (int i = 0; i < 4; i++)
    {
        float tmp_delta_angle = Math_Modulus_Normalization(Target_Steer_Angle[i] - Steering_Motor[i].Get_Now_Angle(), 2.0f * PI);

        // 根据转动角度范围决定是否需要就近转位
        if (-PI / 2.0f <= tmp_delta_angle && tmp_delta_angle <= PI / 2.0f)
        {
            // ±PI / 2之间无需反向就近转位
            Target_Steer_Angle[i] = tmp_delta_angle + Steering_Motor[i].Get_Now_Angle();
        }
        else
        {
            // 需要反转扣圈情况
            Target_Steer_Angle[i] = Math_Modulus_Normalization(tmp_delta_angle + PI, 2.0f * PI) + Steering_Motor[i].Get_Now_Angle();
            Target_Wheel_Omega[i] *= -1.0f;
        }
    }
}

/**
 * @brief 输出控制量到电机
 *
 */
void Class_Chassis_Steering::Output_To_Motor()
{
    // 输出控制量到电机，舵轮电机输出目标角度，轮向电机输出目标速度
    for (int i = 0; i < 4; i++)
    {
        Steering_Motor[i].Set_Target_Angle(Target_Steer_Angle[i]);
        Chassis_Motor[i].Set_Target_Omega(Target_Wheel_Omega[i]);
    }
}
