# `User_File` 目录详细文档

## 概述

`User_File` 目录是 STM32 项目中用户代码的核心区域，它建立在 STM32CubeMX 生成的底层驱动之上，实现了代码的分层和模块化，极大地提高了代码的可读性、可维护性和可移植性。

-   **`1_Middleware` (中间件层)**: 提供与具体硬件设备无关的、可复用的软件组件，如外设驱动的二次封装和通用算法。
-   **`2_Device` (设备层)**: 封装了对具体硬件设备（如电机、传感器）的控制逻辑，调用中间件层的接口来实现功能。
-   **`3_Task` (任务层)**: 存放具体的业务逻辑和 FreeRTOS 任务，是应用的最高层。

---

## 1. `User_File/1_Middleware` - 用户中间件

### 1.1. `1_Driver` - 用户驱动层

此目录是对 HAL 库的二次封装，提供了更简洁、更符合应用逻辑的接口。

#### 1.1.1. `ADC` (`drv_adc.h`)

-   **作用**: 封装 ADC（模数转换器）的 DMA 采集功能。
-   **结构体**:
    -   `struct Struct_ADC_Manage_Object`: ADC 管理对象。
        -   `ADC_HandleTypeDef *ADC_Handler`: 指向 HAL 库的 ADC 句柄。
        -   `uint16_t ADC_Data[SAMPLE_BUFFER_SIZE]`: 存储 DMA 采集到的 ADC 原始数据。
-   **函数**:
    -   `void ADC_Init(ADC_HandleTypeDef *hadc, uint16_t Sample_Number)`: 初始化并启动指定 ADC 的 DMA 连续转换。
-   **使用方法**: 在 `main` 函数中，对需要使用的 ADC（如 `hadc1`）调用 `ADC_Init(&hadc1, ...)` 即可。之后，DMA 会自动将采集的数据更新到对应的 `ADCx_Manage_Object.ADC_Data` 数组中。

#### 1.1.2. `BSP` (`drv_djiboarda.h`)

-   **作用**: 板级支持包（Board Support Package），专门用于大疆 A 型开发板（RoboMaster Development Board Type A），提供对板载资源（LED、按键、电源输出等）的统一控制接口。
-   **宏定义**:
    -   `BSP_DC24_..._ON`, `BSP_LED_..._ON`: 用于 `BSP_Init` 函数的位掩码，方便一次性初始化多个外设的状态。
    -   `BoardA_..._Pin`, `BoardA_..._GPIO_Port`: 定义了 A 板上所有外设的 GPIO 引脚，方便移植和维护。
-   **枚举**:
    -   `Enum_BSP_DC24_Status`: 24V 电源输出的使能/失能状态。
    -   `Enum_BSP_LED_Status`: LED 的亮/灭状态。
    -   `Enum_BSP_Key_Status`: 按键状态（按下、释放、触发等）。
-   **函数**:
    -   `void BSP_Init(uint32_t Status, ...)`: 初始化板载外设。通过 `Status` 位掩码参数，可以方便地设置各路 24V 电源和 LED 的初始状态。
    -   `BSP_Set_...()`: 设置外设状态，如 `BSP_Set_LED_R(BSP_LED_Status_ENABLED)` 点亮红色 LED。
    -   `BSP_Get_...()`: 获取外设状态，如 `BSP_Get_Key()` 获取板载按键的状态。
    -   `BSP_Set_PWM_IMU_Heater(float Rate)`: 设置 IMU 加热片的 PWM 占空比。
    -   `BSP_Set_PWM_Buzzer(float Rate)`: 设置蜂鸣器的 PWM 占空比以控制其鸣叫。

#### 1.1.3. `CAN` (`drv_can.h`)

-   **作用**: 封装 CAN 总线通信，采用回调函数机制处理数据接收，并提供周期性发送功能。
-   **结构体**:
    -   `struct Struct_CAN_Rx_Buffer`: 存储一条接收到的 CAN 报文（包含报文头和 8 字节数据）。
    -   `struct Struct_CAN_Manage_Object`: CAN 管理对象。
        -   `CAN_HandleTypeDef *CAN_Handler`: 指向 HAL 库的 CAN 句柄。
        -   `CAN_Call_Back Callback_Function`: **核心功能**，一个函数指针，用于注册接收回调函数。
-   **函数**:
    -   `void CAN_Init(CAN_HandleTypeDef *hcan, CAN_Call_Back Callback_Function)`: 初始化 CAN 外设，并**注册一个回调函数**。当该 CAN 总线收到任何数据时，都会调用这个注册的函数进行处理。
    -   `uint8_t CAN_Send_Data(CAN_HandleTypeDef *hcan, uint16_t ID, uint8_t *Data, uint16_t Length)`: 发送一帧 CAN 数据。
    -   `void TIM_1ms_CAN_PeriodElapsedCallback()`: 一个 1ms 周期调用函数，通常用于**周期性地发送控制指令**（如电机控制报文），确保控制的实时性。
-   **使用方法**:
    1.  在 `can.c` 的 `MX_CANx_Init` 之后，调用 `CAN_Init(&hcanx, Your_Callback_Function)`。
    2.  实现 `Your_Callback_Function(Struct_CAN_Rx_Buffer *rx_buffer)`，在此函数内通过 `switch(rx_buffer->Header.StdId)` 判断报文 ID，并将数据分发给对应的设备处理函数（如 `DJI_Motor.CAN_RxCpltCallback(...)`）。

#### 1.1.4. `Math` (`drv_math.h`)

-   **作用**: 提供一个包含常用数学运算、宏定义和模板函数的工具库。
-   **宏定义**:
    -   `RPM_TO_RADPS`: 转速 (rpm) 到角速度 (rad/s) 的转换系数。
    -   `DEG_TO_RAD`: 角度 (°) 到弧度 (rad) 的转换系数。
-   **模板函数 (泛型编程)**:
    -   `Type Math_Constrain(Type *x, Type Min, Type Max)`: **极常用**，将一个值 `x` 限制在 `Min` 和 `Max` 之间。
    -   `Type Math_Abs(Type x)`: 求绝对值。
    -   `Type Math_Modulus_Normalization(Type x, Type modulus)`: 取模归一化，常用于将多圈角度归一化到 `[-modulus/2, +modulus/2]` 区间（如 `[-PI, +PI]`）。
-   **普通函数**:
    -   `Math_Endian_Reverse_16/32`: 大小端转换，用于处理通信协议中的多字节数据。
    -   `Math_Float_To_Int / Math_Int_To_Float`: 在两个不同范围的整数和浮点数之间进行线性映射。

#### 1.1.5. `SPI` (`drv_spi.h`)

-   **作用**: 封装 SPI 通信，同样采用回调函数机制，并简化了带片选（CS）的通信过程。
-   **结构体**:
    -   `struct Struct_SPI_Manage_Object`: SPI 管理对象，包含 SPI 句柄、收发缓冲区、当前通信的 CS 引脚以及回调函数。
-   **函数**:
    -   `void SPI_Init(SPI_HandleTypeDef *hspi, SPI_Call_Back Callback_Function)`: 初始化 SPI 并注册接收完成回调函数。
    -   `uint8_t SPI_Send_Receive_Data(SPI_HandleTypeDef *hspi, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, ...)`: **核心函数**，发送和接收 SPI 数据。函数内部会自动处理 CS 引脚的拉低和拉高。
    -   `void TIM_100us_SPI_PeriodElapsedCallback()`: 100us 周期调用函数，用于处理需要高速轮询的 SPI 设备。

#### 1.1.6. `TIM` (`drv_tim.h`)

-   **作用**: 对硬件定时器的中断功能进行统一管理和分发。
-   **结构体**:
    -   `struct Struct_TIM_Manage_Object`: 定时器管理对象，将定时器句柄和其对应的回调函数绑定。
-   **函数**:
    -   `void TIM_Init(TIM_HandleTypeDef *htim, TIM_Call_Back Callback_Function)`: 初始化定时器并为其注册一个中断回调函数。
-   **使用方法**:
    1.  在 `tim.c` 的 `MX_TIMx_Init` 之后，调用 `TIM_Init(&htimx, Your_TIM_Callback)`。
    2.  在 `stm32f4xx_it.c` 的 `HAL_TIM_PeriodElapsedCallback` 函数中，会根据中断来源 `htim->Instance` 自动调用已注册的 `Your_TIM_Callback`。这使得中断服务函数非常整洁。

#### 1.1.7. `UART` (`drv_uart.h`)

-   **作用**: 封装 UART（串口）通信，使用 **DMA + 空闲中断** 的方式高效接收不定长数据。
-   **结构体**:
    -   `struct Struct_UART_Manage_Object`: UART 管理对象，包含句柄、收发缓冲区和回调函数。
-   **函数**:
    -   `void UART_Init(UART_HandleTypeDef *huart, UART_Call_Back Callback_Function, uint16_t Rx_Buffer_Length)`: 初始化 UART，启动 DMA 空闲中断接收，并注册数据接收回调函数。
    -   `uint8_t UART_Send_Data(UART_HandleTypeDef *huart, uint8_t *Data, uint16_t Length)`: 使用 DMA 发送数据。
    -   `void UART_Reinit(UART_HandleTypeDef *huart)`: 当串口出错时，用于重新初始化接收。
-   **工作原理**:
    1.  调用 `UART_Init` 后，DMA 会一直等待接收数据。
    2.  当总线上超过1个字节的时间没有数据时，会触发空闲中断（IDLE Interrupt）。
    3.  在中断服务函数中，计算本次接收到的数据长度，然后调用已注册的回调函数，将接收到的数据 `Buffer` 和 `Length` 传递给上层应用。
    4.  这种方式非常适合接收如遥控器、裁判系统等长度不固定的数据帧。

#### 1.1.8. `WDG` (`drv_wdg.h`)

-   **作用**: 封装独立看门狗（IWDG）的功能。
-   **函数**:
    -   `void IWDG_Independent_Feed()`: “喂狗”函数，重置看门狗计数器。
    -   `void TIM_1ms_IWDG_PeriodElapsedCallback()`: 1ms 周期调用函数，在此函数内部调用 `IWDG_Independent_Feed()`，实现周期性自动喂狗。
-   **使用方法**: 在主循环或一个可靠的定时任务中周期性调用喂狗函数。如果程序卡死，无法喂狗，看门狗将超时并复位单片机，保证系统的可靠性。

### 1.2. `2_Algorithm` - 用户算法层

此目录存放与具体业务逻辑相关的核心算法，以 C++ 类的形式提供，便于实例化和复用。

#### 1.2.1. `Filter` (`alg_filter.h`)

-   **作用**: 提供数字滤波器算法。
-   **类**:
    -   `class Class_Filter_Fourier<uint32_t Filter_Fourier_Order>`: **FIR 滤波器**。
        -   `Init()`: 初始化滤波器类型（低通、高通、带通、带阻）、截止频率和采样频率。
        -   `Set_Now(float __Now)`: 传入当前采样值。
        -   `TIM_Calculate_PeriodElapsedCallback()`: **周期调用**，执行卷积运算，更新输出。
        -   `Get_Out()`: 获取滤波后的输出。
    -   `class Class_Filter_Kalman`: **卡尔曼滤波器**。
        -   `Init()`: 初始化测量误差和估计误差。
        -   `Set_Now(float __Now)`: 传入当前测量值。
        -   `TIM_Calculate_PeriodElapsedCallback()`: **周期调用**，执行卡尔曼滤波迭代计算。
        -   `Get_Out()`: 获取滤波后的最优估计值。

#### 1.2.2. `FSM` (`alg_fsm.h`)

-   **作用**: 提供一个有限状态机（Finite State Machine）的基类框架。
-   **类**:
    -   `class Class_FSM`: 状态机基类。
        -   `Status[STATUS_MAX]`: 存储每个状态的阶段（使能/失能）和在该状态下持续的时间。
        -   `Init()`: 初始化状态机的状态数量和初始状态。
        -   `Set_Status(uint8_t Next_Status_serial)`: **核心函数**，用于从当前状态切换到下一个状态。
        -   `Get_Now_Status_Serial()`: 获取当前处于哪个状态。
        -   `TIM_Calculate_PeriodElapsedCallback()`: **周期调用**，用于更新当前状态的持续时间 `Count_Time`。
-   **使用方法**: 创建一个新类继承自 `Class_FSM`，在新类中实现每个状态的具体行为逻辑。通过 `Set_Status` 控制状态转移。

#### 1.2.3. `PID` (`alg_pid.h`)

-   **作用**: 实现一个功能丰富的 PID 控制器。
-   **类**:
    -   `class Class_PID`: PID 控制器。
        -   `Init()`: 初始化 PID 参数（Kp, Ki, Kd, Kf）、积分限幅、输出限幅、死区等。
        -   `Set_Target(float __Target)`: 设定目标值。
        -   `Set_Now(float __Now)`: 设定当前测量值。
        -   `TIM_Calculate_PeriodElapsedCallback()`: **核心函数，周期调用**，执行一次 PID 计算，更新输出值。
        -   `Get_Out()`: 获取 PID 的计算输出。
        -   **高级功能**: 支持前馈（Kf）、积分分离、变速积分、微分先行等，功能非常全面。

#### 1.2.4. `Queue` (`alg_queue.h`)

-   **作用**: 提供一个线程安全的静态循环队列。
-   **类**:
    -   `template<typename Type, uint32_t Max_Size>` `class Class_Queue`: 队列模板类。
        -   `Push(Type __Data)`: 向队尾添加一个元素。
        -   `Pop()`: 从队首弹出一个元素。
        -   `Get_Front() / Get_Rear()`: 获取队首/队尾元素，但不弹出。
        -   `Get_Length()`: 获取队列当前长度。
        -   `Clear()`: 清空队列。
-   **使用场景**: 在生产者-消费者模型中用作缓冲区，例如在中断中接收数据（生产者），在主任务中处理数据（消费者）。

#### 1.2.5. `Slope` (`alg_slope.h`)

-   **作用**: 实现斜坡函数，用于平滑地改变一个值，常用于速度规划。
-   **类**:
    -   `class Class_Slope`: 斜坡函数生成器。
        -   `Init()`: 初始化增/减速率。
        -   `Set_Target(float __Target)`: 设定最终目标值。
        -   `TIM_Calculate_PeriodElapsedCallback()`: **周期调用**，根据增/减速率，使当前输出值逐步逼近目标值。
        -   `Get_Out()`: 获取当前时刻的平滑输出值。
-   **使用场景**: 当遥控器给定一个突变的目标速度时，通过斜坡函数处理，可以使机器人平稳地加速或减速，避免冲击。

#### 1.2.6. `Timer` (`alg_timer.h`)

-   **作用**: 提供一个软件定时器，用于实现非阻塞延时或状态延时触发。
-   **类**:
    -   `class Class_Timer`: 软件定时器。
        -   `Init()`: 初始化，但不启动。
        -   `Set_Delay(uint32_t __Delay)`: **核心函数**，设置一个延时时间（单位通常是 ms）并启动计时器。
        -   `TIM_1ms_Calculate_PeriodElapsedCallback()`: **必须在 1ms 定时中断中调用**，用于更新内部计数值。
        -   `Get_Now_Status()`: 获取定时器当前状态（复位、等待、触发、超时）。
-   **使用方法**: 当需要延时 `N` 毫秒后执行某个动作时，调用 `MyTimer.Set_Delay(N)`，然后在主循环中轮询 `MyTimer.Get_Now_Status() == Timer_Status_TIMEOUT`。

---

## 2. `User_File/2_Device` - 用户设备层

此目录封装了对具体硬件设备的控制，它调用 `1_Middleware` 层的接口。

### 2.1. `Motor` - 电机

#### 2.1.1. `Motor_DJI` (`dvc_motor_dji.h`)

-   **作用**: 封装对大疆智能电机（如 M2006, M3508, GM6020）的驱动和控制。
-   **类**:
    -   `class Class_Motor_DJI`: 大疆电机驱动类。
        -   内置了 `Class_PID PID_Angle` 和 `Class_PID PID_Omega`，表明该类**自带闭环控制器**。
        -   `Init()`: 初始化电机，绑定 CAN、电机类型（M2006/M3508/GM6020）、编码器零点偏移等。
        -   `CAN_RxCpltCallback()`: **数据输入接口**，在 CAN 接收回调中调用，用于解析电调返回的包含电机角度、速度、电流等信息的数据。
        -   `TIM_1ms_Calculate_PeriodElapsedCallback()`: **核心函数，周期调用**，执行内置的位置-速度串级 PID 控制算法。
        -   `Set_Target_Angle/Omega()`: 设置角度/速度环的目标值。
        -   `Get_Now_Angle/Omega/Current()`: 获取电机当前的反馈数据。
        -   `Set_Out/Get_Out()`: 设置/获取最终输出到电调的电流/电压值。
-   **使用方法**:
    1.  为每个电机实例化一个 `Class_Motor_DJI` 对象。
    2.  在 CAN 接收回调中，根据报文 ID 将数据喂给对应的电机对象。
    3.  在 1ms 定时任务中，周期性调用所有电机对象的 `TIM_1ms_Calculate_PeriodElapsedCallback()`。
    4.  在 `TIM_1ms_CAN_PeriodElapsedCallback()` 中，将所有电机的 `Out` 值打包并通过 `CAN_Send_Data` 发送出去。

#### 2.1.2. `Motor_DM` (`dvc_motor_dm.h`)

-   **作用**: 封装对达妙智能（DM）系列电机的驱动和控制。
-   **类**:
    -   `class Class_Motor_DM_Normal`: 针对达妙**传统模式**的电机。
        -   `Init()`: 初始化电机，绑定 CAN 句柄、收发 ID、控制模式和物理参数（最大角度/速度/扭矩）。
        -   `CAN_RxCpltCallback()`: **数据输入接口**，在 CAN 接收回调中调用，用于解析电机返回的数据。
        -   `TIM_Send_PeriodElapsedCallback()`: **周期调用**，用于向电机发送控制指令。
        -   `Set_Control_...()`: 设置电机的目标控制量（角度、速度、扭矩等）。
        -   `Get_Now_...()`: 获取电机当前的反馈数据（角度、速度、温度等）。
        -   `CAN_Send_Enter() / Exit()`: 发送进入/退出电机控制模式的指令。
    -   `class Class_Motor_DM_1_To_4`: 针对达妙**一拖四模式**的电机。
        -   内置了 `Class_PID PID_Angle` 和 `Class_PID PID_Omega`，表明该类**自带闭环控制器**。
        -   `Init()`: 初始化电机，绑定 CAN、电机 ID、控制模式和编码器零点偏移。
        -   `Set_Target_Angle/Omega/Current()`: 设置角度/速度/电流环的目标值。
        -   `TIM_1ms_Calculate_PeriodElapsedCallback()`: **周期调用**，执行内置 PID 控制算法。
        -   `CAN_RxCpltCallback()`: 解析电机反馈数据。
        -   输出由内部 PID 计算后，通过 CAN 发送给驱动板。

---

## 3. `User_File/3_Task` - 任务层

此目录是应用逻辑的最高层，通常包含 FreeRTOS 的任务定义和 C/C++ 混合编程的接口。

### 3.1. `dvc_motor_dm_api.h`

-   **作用**: 提供一组 C 语言风格的 API 接口，用于启动和管理达妙电机的 FreeRTOS 任务。
-   **函数**:
    -   `void J4310_Task_Init(void)`: 初始化 J4310 电机任务所需的资源。
    -   `void J4310_Task_Entry(void *argument)`: **FreeRTOS 任务函数**，是 J4310 电机控制逻辑的入口。
    -   `void J4310_CAN_RxCpltCallback(uint8_t *Rx_Data)`: CAN 接收回调的 C 语言封装，内部会调用 C++ 对象的相应方法。
-   **目的**: 这是典型的 C 与 C++ 混合编程的接口层。由于 FreeRTOS 的任务函数是 C 函数，因此需要一个 C 函数 `J4310_Task_Entry` 作为入口，在这个函数内部再去调用 C++ 编写的电机控制类的对象和方法。

### 3.2. `helloworld.h`

-   **作用**: 一个示例性的任务，通常用于测试开发环境、RTOS 调度和基础外设（如串口打印）是否正常工作。可以作为创建新应用任务的模板。
