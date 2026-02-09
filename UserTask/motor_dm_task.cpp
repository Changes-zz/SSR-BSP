#include "motor_dm_task.h"
#include "dvc_motor_dm.h"
#include "cmsis_os.h"
#include "main.h" // For PI

// 电机控制器静态实例
static Class_Motor_DM_Normal motor_j4310;

// 在can.c中定义的外部CAN句柄
extern CAN_HandleTypeDef hcan1;

/**
 * @brief 初始化J4310电机对象
 */
void J4310_Task_Init(void)
{
    motor_j4310.Init(&hcan1, 0x00, 0x01, Motor_DM_Control_Method_NORMAL_MIT);
}

/**
 * @brief CAN接收完成回调的包装函数
 *        此函数在main.c的主CAN回调中被调用
 * @param Rx_Data 指向接收到的CAN数据缓冲区的指针
 */
void J4310_CAN_RxCpltCallback(uint8_t *Rx_Data)
{
    motor_j4310.CAN_RxCpltCallback(Rx_Data);
}

/**
 * @brief J4310电机的FreeRTOS任务入口函数
 * @param argument 任务参数 (未使用)
 */
void J4310_Task(void *argument)
{
    static uint32_t Counter = 0;
    static uint32_t Counter_KeepAlive = 0;

    for(;;)
    {
        Counter++;

        // 来自main.c while循环的控制逻辑
        motor_j4310.Set_Control_Angle((Counter / 3000) % 2 == 0 ? 0.0f : 2*PI);
        motor_j4310.Set_Control_Omega(0.0f);
        motor_j4310.Set_Control_Torque(0.0f);
        motor_j4310.Set_K_P(20.0f);
        motor_j4310.Set_K_D(0.3f);
        motor_j4310.TIM_Send_PeriodElapsedCallback();

        // 保持存活逻辑
        if (Counter_KeepAlive++ > 100)
        {
            Counter_KeepAlive = 0;
            motor_j4310.TIM_Alive_PeriodElapsedCallback();
        }

        osDelay(1);
    }
}
