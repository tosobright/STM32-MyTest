# STM32F103C8T6
    1.基于STM32CUBE生成的HAL库
    2.配置FreeRTOS，4个任务
    3.配置2路串口，一路为打印，另一路为Modbus
    4.配置2路定时器，TIM2为串口打印超时断帧，TIM3为Modbus超时断帧
    5.配置FreeModbus
    6.配置一路can，做loopback测试
    7.配置IIC测试
    8.支持IAP升级
