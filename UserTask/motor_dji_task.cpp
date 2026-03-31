#include "motor_dji_task.h"
#include "dvc_motor.h"
#include "cmsis_os.h"
#include "uart_printf.h"

// 电机控制器静态实例
static Class_Motor_C620 motor_3508;

// 在can.c中定义的外部CAN句柄
extern CAN_HandleTypeDef hcan1;

/**
 * @brief 初始化M3508电机对象及其PID
 */
void DJI_M3508_Task_Init(void)
{
    // motor_3508.PID_Omega.Init(250.0787f, 207.519f, 0.172835f, 0.0f, 10000.0f, 10000.0f);
    // motor_3508.Init(&hcan1, CAN_Motor_ID_0x201, Control_Method_OMEGA);
    motor_3508.Init(&hcan1, CAN_Motor_ID_0x201, Control_Method_OPENLOOP);
}

/**
 * @brief CAN接收完成回调的包装函数
 * @param Rx_Data 指向接收到的CAN数据缓冲区的指针
 */
void DJI_M3508_CAN_RxCpltCallback(uint8_t *Rx_Data)
{
    motor_3508.CAN_RxCpltCallback(Rx_Data);
}

/**
 * @brief M3508电机的FreeRTOS任务入口函数
 * @param argument 任务参数 (未使用)
 */
void DJI_M3508_Task(void *argument)
{
    int time_counter = 0;
    for(;;)
    {
        // motor_3508.Set_Target_Omega(10.0f);

        if ((time_counter > 1000) && (time_counter <= 10000))
        {
            motor_3508.Set_Target_Torque(3000.0f);
        }
        else
        {
            motor_3508.Set_Target_Torque(0.0f);
            if (time_counter > 10000)
            {
                time_counter = 0;
            }
        }

        // 5ms采样一次
        if (time_counter % 5 == 0)
        {
            uart_printf("%f,%f,%f\r\n", motor_3508.Get_Now_Omega(),motor_3508.Get_Now_Torque(),motor_3508.Get_Target_Torque());// 通过串口打印当前速度
        }


        motor_3508.TIM_PID_PeriodElapsedCallback();
        TIM_CAN_PeriodElapsedCallback();

        time_counter++;

        osDelay(1);
    }
}
