# 嵌入式软件代码编写规范

## 1. 概述

本文档旨在为嵌入式软件开发团队提供一套统一的代码编写标准，覆盖从代码风格、命名约定到开发流程的各个方面。遵循此规范有助于提高代码质量、可读性、可维护性，并降低团队协作成本。

所有新代码都应遵循此规范，历史代码在修改时也建议逐步重构以符合规范。

---

## 2. 文件与目录结构

项目采用分层架构，新增文件必须放置在对应的逻辑目录中：

-   **`Core/`**: 由 STM32CubeMX 自动生成，存放核心启动文件和主要外设初始化代码。**原则上不手动修改此目录内容**，应通过 CubeMX 重新生成。
-   **`Drivers/`**: 存放 MCU 的 HAL/LL 库和 CMSIS 核心文件。
-   **`Middlewares/`**: 存放官方或第三方中间件，如 FreeRTOS、USB 协议栈等。
-   **`User_File/`**: **用户代码核心开发区**。
    -   **`1_Middleware/`**: 用户自定义的中间件。
        -   `1_Driver/`: 对 HAL 库进行二次封装，提供更友好的外设操作接口（如 `drv_can.c`）。
        -   `2_Algorithm/`: 存放与硬件无关的通用算法模块（如 `alg_pid.c`）。
    -   **`2_Device/`**: 针对具体硬件设备的驱动程序（如 `dvc_motor_dji.c`），调用 `1_Middleware` 层的接口。
    -   **`3_Task/`**: FreeRTOS 任务的实现文件和业务逻辑。

---

## 3. 命名规范

清晰、一致的命名是代码可读性的基石。

### 3.1 文件命名

采用**小写下划线命名法 (snake_case)**，并添加**层级前缀**以明示其所属模块。

-   **用户驱动层**: `drv_[模块名].c/.h` (e.g., `drv_uart.h`)
-   **用户算法层**: `alg_[模块名].c/.h` (e.g., `alg_filter.h`)
-   **用户设备层**: `dvc_[模块名].c/.h` (e.g., `dvc_mpu6500.h`)
-   **任务层**: `[任务名]_task.c/.h` (e.g., `chassis_task.h`)

### 3.2 变量命名

-   **局部变量**: 采用**小驼峰命名法 (camelCase)**。
    ```c
    uint16_t adcValue;
    float motorSpeed;
    ```
-   **全局变量**: 采用**小写下划线命名法**，并添加 `g_` 前缀以明确其全局作用域。
    ```c
    // drv_can.c
    uint8_t g_can1_tx_buffer[8];
    // helloworld.c
    float g_target_angle;
    ```
-   **常量与宏定义**: 采用**全大写下划线命名法 (UPPER_SNAKE_CASE)**。
    ```c
    #define MAX_MOTOR_SPEED 8191.0f
    #define ADC_TIMEOUT_MS 100
    ```
-   **枚举 (Enum)**:
    -   枚举类型名: `Enum` + `PascalCase`。
    -   枚举成员: `[类型名缩写]_` + `UPPER_SNAKE_CASE`。
    ```c
    typedef enum
    {
        MOTOR_STATUS_OK,
        MOTOR_STATUS_OFFLINE,
        MOTOR_STATUS_OVER_TEMP,
    } Enum_Motor_Status;
    ```
-   **结构体 (Struct) 与类 (Class)**:
    -   类型名: `Struct_` / `Class_` + `PascalCase`。
    -   实例对象: `PascalCase` 或 `camelCase`，并以模块名作为前缀或后缀。
    ```c
    // 类型定义
    typedef struct
    {
        CAN_HandleTypeDef *can_handler;
        // ...
    } Struct_CAN_Manage_Object;

    // 实例
    Struct_CAN_Manage_Object CAN1_Manage_Object;
    Class_PID Chassis_Speed_PID;
    ```

### 3.3 函数命名

采用**模块前缀 + 帕斯卡命名法 (PascalCase)**，力求函数名清晰地表达其“模块”和“动作”。

-   **格式**: `[模块名]_[动作]` 或 `[模块]_[子模块]_[动作]`
-   **常用动词**:
    -   `Init`: 初始化模块。
    -   `Set`: 设置参数或状态。
    -   `Get`: 获取参数或状态。
    -   `Enable`/`Disable`: 使能/失能功能。
    -   `Send`/`Receive`: 发送/接收数据。
    -   `Calculate`: 执行计算（常用于 PID、Filter 等算法类）。
    -   `Register`: 注册回调函数。
    -   `Callback`: 作为回调函数。

-   **示例**:
    ```c
    // CAN 驱动
    void CAN_Init(CAN_HandleTypeDef *hcan, CAN_Call_Back callback);
    uint8_t CAN_Send_Data(CAN_HandleTypeDef *hcan, uint16_t id, uint8_t *data, uint16_t length);

    // PID 算法
    void PID_Init(Class_PID *pid, ...);
    float PID_Calculate(Class_PID *pid, float target, float current);

    // 电机设备
    void Motor_DJI_Set_Target_Angle(Class_Motor_DJI *motor, float angle);
    float Motor_DJI_Get_Now_Speed(Class_Motor_DJI *motor);
    ```

### 3.4 RTOS 相关命名

-   **任务函数**: `[任务名]_Task_Entry`。
    ```c
    void Chassis_Task_Entry(void *argument);
    ```
-   **任务句柄**: `[任务名]_Task_Handle`。
    ```c
    osThreadId_t Chassis_Task_Handle;
    ```
-   **信号量/互斥锁句柄**: `g_[资源名]_Mutex/Semaphore`。
    ```c
    osMutexId_t g_uart1_Tx_Mutex;
    ```
-   **消息队列句柄**: `g_[队列内容]_Queue`。
    ```c
    osMessageQueueId_t g_imu_Data_Queue;
    ```

---

## 4. 代码风格与格式

-   **缩进**: 使用 **4个空格** 进行缩进，禁止使用 Tab 字符。
-   **大括号 (`{}`)**: 采用 **Allman 风格**，即左大括号和右大括号均单独占一行。
    ```c
    // 推荐
    if (condition)
    {
        // code
    }
    else
    {
        // code
    }

    // 不推荐
    if (condition) {
        // code
    }
    ```
-   **空格**:
    -   操作符（`+`, `-`, `*`, `/`, `=`, `==`, `>` 等）两边应有空格。
    -   逗号 `,` 和分号 `;` 后面应有空格。
    -   关键字（`if`, `while`, `for`）后应有空格。
    ```c
    for (int i = 0; i < 10; i++)
    {
        sum += array[i];
    }
    ```
-   **注释**:
    -   **文件头注释**: 每个 `.c` 和 `.h` 文件都必须包含 Doxygen 风格的文件头注释，说明文件作用、作者、版本历史。
        ```c
        /**
         * @file drv_can.h
         * @author Your Name (your.email@example.com)
         * @brief CAN 总线驱动二次封装
         * @version 0.1
         * @date 2025-10-23
         *
         * @copyright Copyright (c) 2025
         *
         */
        ```
    -   **函数注释**: 每个函数声明前都应有 Doxygen 风格的注释，说明函数功能、参数、返回值。
        ```c
        /**
         * @brief 发送一帧 CAN 数据
         * @param hcan CAN 句柄
         * @param id 报文 ID
         * @param data 数据指针
         * @param length 数据长度
         * @return uint8_t 0:成功, 1:失败
         */
        uint8_t CAN_Send_Data(CAN_HandleTypeDef *hcan, uint16_t id, uint8_t *data, uint16_t length);
        ```
    -   **行内注释**: 对复杂或不直观的代码逻辑，在行尾或代码上方添加 `//` 注释。

---

## 5. 推荐开发流程

在添加一个新功能（例如：集成一个新的陀螺仪传感器）时，建议遵循以下自下而上的分层开发流程：

1.  **CubeMX 配置**: 在 CubeMX 中配置好传感器所需的硬件接口（如 SPI 或 I2C）。
2.  **编写 `drv_` 层**: 在 `User_File/1_Middleware/1_Driver/` 中创建 `drv_new_imu.c/.h`。封装 SPI/I2C 的读写函数，提供如 `IMU_Read_Reg()`、`IMU_Write_Reg()` 的基础接口。
3.  **编写 `dvc_` 层**: 在 `User_File/2_Device/` 中创建 `dvc_new_imu.c/.h`。
    -   包含 `drv_new_imu.h`。
    -   实现 `IMU_Init()` 函数，在其中调用 `IMU_Write_Reg()` 来配置传感器内部寄存器。
    -   实现 `IMU_Get_Data()` 函数，在其中调用 `IMU_Read_Reg()` 读取加速度、角速度等数据，并进行单位转换。
4.  **编写 `alg_` 层 (如果需要)**: 如果需要对传感器数据进行滤波或姿态解算，在 `User_File/1_Middleware/2_Algorithm/` 中创建或使用现有的算法模块（如 `alg_filter.h`）。
5.  **编写 `Task` 层**: 在 `User_File/3_Task/` 中创建 `imu_task.c/.h`。
    -   创建 `IMU_Task_Entry` 任务函数。
    -   在任务的初始化部分，调用 `IMU_Init()`。
    -   在任务的 `for(;;)` 循环中，以固定的频率（如 `osDelay(1)`）调用 `IMU_Get_Data()` 获取数据，然后可以通过消息队列 `osMessageQueuePut()` 将处理好的数据发送给其他需要姿态信息的任务（如底盘控制任务）。
6.  **主函数集成**: 在 `main.c` 或 `freertos.c` 中创建并启动 `IMU_Task`。

通过遵循此流程，可以确保代码结构清晰，各层职责单一，便于未来调试和功能扩展。
