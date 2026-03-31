#ifndef __CRT_WHEELCHASSIS_H
#define __CRT_WHEELCHASSIS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "can.h"

// #define PI 3.1415926
#define TIMESTEP 0.001f //时间步长，和定时中断频率有关

#ifdef __cplusplus
}
#endif

// C++ 专用接口（仅在 C++ 模式下可见）
#ifdef __cplusplus

#include "drv_math.h"
#include "alg_pid.h"
#include "dvc_motor.h"

// STM32F427 uses Cortex-M4; CMSIS-DSP requires one ARM_MATH_* core macro.
#ifndef ARM_MATH_CM4
#define ARM_MATH_CM4
#endif
#include "arm_math.h"

/**
 * @brief 底盘类接口约定
 *
 * 1) 单位约定:
 *    - 线速度: m/s
 *    - 角速度: rad/s
 *    - 角度:   rad
 *
 * 2) 调用顺序约定:
 *    - Init() 仅初始化一次（重复调用需幂等）
 *    - Set_Target_*() 设置控制目标
 *    - TIM_1ms_Control_PeriodElapsedCallback() 周期调用(1ms)
 *
 * 3) 职责约定:
 *    - Update_Now_State_From_Motor_Feedback(): 由电机反馈更新Now状态
 *    - Kinematics_Inverse_Resolution(): 仅计算目标轮速/角度
 *    - Output_To_Motor(): 仅将计算结果下发给电机
 */

/**
 * @brief Based，底盘控制基类
 */
class Class_Chassis_Base
{
public:
    // 底盘速度值PID
    Class_PID PID_Velocity_X;

    // 底盘速度方向PID
    Class_PID PID_Velocity_Y;

    // 底盘角速度PID
    Class_PID PID_Omega;

    virtual ~Class_Chassis_Base() = default;

    // 初始化函数，需在派生类中重写
    virtual void Init() {};

    // 获得底盘当前速度(m/s, m/s, rad/s)
    inline float Get_Now_Velocity_X();
    inline float Get_Now_Velocity_Y();
    inline float Get_Now_Omega();

    // 获得底盘目标速度(m/s, m/s, rad/s)
    inline float Get_Target_Velocity_X();
    inline float Get_Target_Velocity_Y();
    inline float Get_Target_Omega();

    // 设置底盘目标速度(m/s, m/s, rad/s)
    inline void Set_Target_Velocity_X(float __Target_Velocity_X);
    inline void Set_Target_Velocity_Y(float __Target_Velocity_Y);
    inline void Set_Target_Omega(float __Target_Omega);

    // 设置底盘当前速度反馈(m/s, m/s, rad/s)
    inline void Set_Now_Velocity_X(float __Now_Velocity_X);
    inline void Set_Now_Velocity_Y(float __Now_Velocity_Y);
    inline void Set_Now_Omega(float __Now_Omega);
    inline void Update_Now_State(float __Now_Velocity_X, float __Now_Velocity_Y, float __Now_Omega);

    // 定时器回调函数，周期1ms
    virtual void TIM_1ms_Control_PeriodElapsedCallback() {};

protected:
    // 内部变量
    // 轮向电机角速度目标值(rad/s)
    float Target_Wheel_Omega[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    // 轮向电机电流目标值
    float Target_Wheel_Current[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    // 读变量
    float Now_Velocity_X = 0.0f;
    float Now_Velocity_Y = 0.0f;
    float Now_Omega = 0.0f;

    // 写变量
    float Target_Velocity_X = 0.0f;
    float Target_Velocity_Y = 0.0f;
    float Target_Omega = 0.0f;

    // 内部函数
    virtual void Update_Now_State_From_Motor_Feedback() {};
    virtual void Kinematics_Inverse_Resolution() {};
    virtual void Output_To_Motor() {};
};

/**
 * @brief Specialized,全向轮底盘控制类
 *
 */
class Class_Chassis_Omni : public Class_Chassis_Base
{
public:
    // 4个底盘电机对象 
    Class_Motor_C620 Chassis_Motor[4]; 

    // 初始化函数，重写基类的Init
    virtual void Init(void) override;

    // 定时器回调函数，周期1ms，重写基类的TIM_1ms_Control_PeriodElapsedCallback
    virtual void TIM_1ms_Control_PeriodElapsedCallback() override;

protected :
    // 初始化相关常量
    
    // 常量

    // 轮组半径（m）
    const float Wheel_Radius = 0.0715f;

    // 底盘旋转中心到轮子中心的距离 (m)
    const float Chassis_L = 0.300f;

    // √2/2
    const float SQRT2_DIV_2 = 0.70710678f;

    // 内部函数

    virtual void Update_Now_State_From_Motor_Feedback() override;

    virtual void Kinematics_Inverse_Resolution() override;

    virtual void Output_To_Motor() override;
};


/**
 * @brief Specialized,麦轮底盘控制类
 *
 */
class Class_Chassis_Mecanum : public Class_Chassis_Base 
{
public:
    // 4个底盘电机对象 
    Class_Motor_C620 Chassis_Motor[4];
    
    // 初始化函数，重写基类的Init
    virtual void Init(void) override;

    // 定时器回调函数，周期1ms，重写基类的TIM_1ms_Control_PeriodElapsedCallback
    virtual void TIM_1ms_Control_PeriodElapsedCallback() override;

protected :
    // 初始化相关常量
    
    // 常量

    // 轮组半径（m）直径152mm
    const float Wheel_Radius = 0.076f;

    // 左右轮中心距 (车宽)   480mm 485.56mm
    const float Chassis_W = 0.48556f;

    // 前后轮中心距 (车长) 390mm
    const float Chassis_L = 0.390f;

    // 底盘旋转中心到轮子中心的距离 (m) 630mm
    const float Chassis_R = 0.315f;

    // √2/2
    const float SQRT2_DIV_2 = 0.70710678f;

    // 内部函数

    virtual void Update_Now_State_From_Motor_Feedback() override;

    virtual void Kinematics_Inverse_Resolution() override;

    virtual void Output_To_Motor() override;
};


/**
 * @brief Specialized,舵轮底盘控制类
 *
 */
class Class_Chassis_Steering : public Class_Chassis_Base
{
public:
    // 4个舵轮电机对象
    Class_Motor_C610 Steering_Motor[4]; 

    // 4个轮向电机对象
    Class_Motor_C620 Chassis_Motor[4];

    // 初始化函数，重写基类的Init
    virtual void Init(void) override;

    // 定时器回调函数，周期1ms，重写基类的TIM_1ms_Control_PeriodElapsedCallback
    virtual void TIM_1ms_Control_PeriodElapsedCallback() override;

protected:
    // 初始化相关常量
    
    // 常量

    // 轮组半径
    const float Wheel_Radius = 0.058f;


    // 极坐标系下轮组位置
    // 轮距中心长度
    const float Wheel_To_Core_Distance[4] = {0.207f,
                                             0.207f,
                                             0.207f,
                                             0.207f,};


    // 轮组方位角
    const float Wheel_Azimuth[4] = {PI / 4.0f,
                                    3.0f * PI / 4.0f,
                                    5.0f * PI / 4.0f,
                                    7.0f * PI / 4.0f,};


    // 内部变量

    // 舵向电机角度目标值
    float Target_Steer_Angle[4];

    // 内部函数
    virtual void Update_Now_State_From_Motor_Feedback() override;

    virtual void Kinematics_Inverse_Resolution() override;
    
    void _Steer_Motor_Kinematics_Nearest_Transposition();

    virtual void Output_To_Motor() override;
};

/**
 * @brief 获取当前速度X
 *
 * @return float 当前速度X
 */
inline float Class_Chassis_Base::Get_Now_Velocity_X()
{
    return (Now_Velocity_X);
}

/**
 * @brief 获取当前速度Y
 *
 * @return float 当前速度Y
 */
inline float Class_Chassis_Base::Get_Now_Velocity_Y()
{
    return (Now_Velocity_Y);
}

/**
 * @brief 获取当前角速度
 *
 * @return float 当前角速度
 */
inline float Class_Chassis_Base::Get_Now_Omega()
{
    return (Now_Omega);
}

/**
 * @brief 获取目标速度X
 *
 * @return float 目标速度X
 */
inline float Class_Chassis_Base::Get_Target_Velocity_X()
{
    return (Target_Velocity_X);
}

/**
 * @brief 获取目标速度Y
 *
 * @return float 目标速度Y
 */
inline float Class_Chassis_Base::Get_Target_Velocity_Y()
{
    return (Target_Velocity_Y);
}

/**
 * @brief 获取目标角速度
 *
 * @return float 目标角速度
 */
inline float Class_Chassis_Base::Get_Target_Omega()
{
    return (Target_Omega);
}

/**
 * @brief 设定目标速度X
 *
 * @param __Target_Velocity_X 目标速度X
 */
inline void Class_Chassis_Base::Set_Target_Velocity_X(float __Target_Velocity_X)
{
    Target_Velocity_X = __Target_Velocity_X;
}

/**
 * @brief 设定目标速度Y
 *
 * @param __Target_Velocity_Y 目标速度Y
 */
inline void Class_Chassis_Base::Set_Target_Velocity_Y(float __Target_Velocity_Y)
{
    Target_Velocity_Y = __Target_Velocity_Y;
}

/**
 * @brief 设定目标角速度
 *
 * @param __Target_Omega 目标角速度
 */
inline void Class_Chassis_Base::Set_Target_Omega(float __Target_Omega)
{
    Target_Omega = __Target_Omega;
}

/**
 * @brief 设定当前速度X反馈
 */
inline void Class_Chassis_Base::Set_Now_Velocity_X(float __Now_Velocity_X)
{
    Now_Velocity_X = __Now_Velocity_X;
}

/**
 * @brief 设定当前速度Y反馈
 */
inline void Class_Chassis_Base::Set_Now_Velocity_Y(float __Now_Velocity_Y)
{
    Now_Velocity_Y = __Now_Velocity_Y;
}

/**
 * @brief 设定当前角速度反馈
 */
inline void Class_Chassis_Base::Set_Now_Omega(float __Now_Omega)
{
    Now_Omega = __Now_Omega;
}

/**
 * @brief 一次性更新当前状态反馈
 */
inline void Class_Chassis_Base::Update_Now_State(float __Now_Velocity_X, float __Now_Velocity_Y, float __Now_Omega)
{
    Now_Velocity_X = __Now_Velocity_X;
    Now_Velocity_Y = __Now_Velocity_Y;
    Now_Omega = __Now_Omega;
}

#endif // __cplusplus

#endif
