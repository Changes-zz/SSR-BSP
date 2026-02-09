#ifndef MOTOR_DM_TASK_H
#define MOTOR_DM_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

// 初始化J4310电机任务所需资源
void J4310_Task_Init(void);

// J4310电机单次迭代处理函数（由 freertos 循环调用）
void J4310_Task(void *argument);

// CAN接收回调函数，用于从主CAN回调中传递数据
void J4310_CAN_RxCpltCallback(uint8_t *Rx_Data);

#ifdef __cplusplus
}
#endif

#endif // MOTOR_DM_TASK_H
