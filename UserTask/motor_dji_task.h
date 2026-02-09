#ifndef MOTOR_DJI_TASK_H
#define MOTOR_DJI_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

/**
 * @brief 初始化M3508电机任务所需资源
 */
void DJI_M3508_Task_Init(void);

/**
 * @brief M3508电机单次迭代处理函数（由 freertos 循环调用）
 * @param argument 任务参数 (未使用)
 */
void DJI_M3508_Task(void *argument);

/**
 * @brief CAN接收回调函数，用于从主CAN回调中传递数据
 * @param Rx_Data 指向接收到的CAN数据缓冲区的指针
 */
void DJI_M3508_CAN_RxCpltCallback(uint8_t *Rx_Data);

#ifdef __cplusplus
}
#endif

#endif // MOTOR_DJI_TASK_H
