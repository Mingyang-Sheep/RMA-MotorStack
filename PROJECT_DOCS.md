# RMA-MotorStack 工程框架说明文档

## 1. 基本信息

| 项目 | 内容 |
|---|---|
| **开发板型号** | 大疆 RoboMaster 开发板 A 型 (Type A) |
| **MCU** | STM32F427IIHx (Cortex-M4, 2MB Flash, 256KB SRAM + 64KB CCM) |
| **主频** | 168MHz (HSE 12MHz × PLL) |
| **开发环境** | Keil MDK5 (uVision 5) |
| **HAL 库** | STM32F4xx HAL Driver |
| **代码来源** | https://github.com/RoboMaster/DevelopmentBoard-Examples.git |
| **产品页面** | https://store.dji.com/cn/product/rm-development-board-type-a?vid=42041 |

## 2. 工程目录结构

```
RMA-MotorStack/
├── Drivers/                        # CMSIS + STM32F4xx HAL 驱动库
│   ├── CMSIS/
│   │   ├── Device/ST/STM32F4xx/   # 启动文件、系统文件
│   │   ├── DSP_Lib/               # CMSIS DSP 库 (IMU 姿态解算)
│   │   ├── Include/               # CMSIS 核心头文件
│   │   └── RTOS/                  # CMSIS RTOS 模板
│   └── STM32F4xx_HAL_Driver/     # STM32F4 HAL 驱动
├── Middlewares/                    # 第三方中间件
│   └── ST/
│       ├── STM32_USB_Device_Library/  # USB 设备库 (CDC 虚拟串口)
│       └── Third_Party/FatFs/         # FatFs 文件系统 (SD 卡)
├── BSP/                            # 板级支持包 (外设驱动)
│   ├── bsp_imu.c/h                # MPU6500 + IST8310 磁力计 + AHRS 姿态解算
│   ├── bsp_uart.c/h               # DBUS 遥控器接收 (USART1 + DMA)
│   ├── bsp_can.c/h                # CAN 总线滤波器 + UWB 数据解析
│   ├── bsp_oled.c/h               # OLED 显示屏驱动
│   ├── oledfont.h                 # OLED 字库
│   └── bsp_sd.c/h + sd_card.c/h   # SD 卡 BSP + FATFS 测试
├── Core/                           # 核心代码 (CubeMX 生成)
│   ├── Inc/
│   │   ├── main.h                 # 工程主头文件 (引脚宏定义)
│   │   ├── stm32f4xx_hal_conf.h   # HAL 模块配置 (使能所有外设)
│   │   ├── stm32f4xx_it.h        # 中断处理函数声明
│   │   └── *.h                    # 外设初始化头文件 (gpio/spi/usart/tim/can/adc/dma/sdio/usb_device/fatfs)
│   └── Src/
│       ├── main.c                 # 统一入口, 初始化所有外设
│       ├── stm32f4xx_hal_msp.c    # HAL MSP 初始化
│       ├── stm32f4xx_it.c        # 统一中断服务函数
│       ├── system_stm32f4xx.c     # 系统时钟初始化
│       ├── startup_stm32f427xx.s  # 启动文件
│       └── *.c                    # 外设初始化源文件
├── App/                            # 应用层模块
│   ├── app_imu.c/h                # IMU 姿态采集
│   ├── app_motor.c/h              # PWM 电机控制 (16 路, 50Hz)
│   ├── app_rc.c/h                 # 遥控器接收
│   ├── app_uwb.c/h                # UWB 定位
│   └── app_display.c/h            # OLED 显示
├── MDK-ARM/                        # Keil5 工程文件
│   ├── RMA-MotorStack.uvprojx     # Keil 工程
│   ├── RMA-MotorStack.uvoptx      # Keil 工程选项
│   ├── DebugConfig/               # 调试配置
│   └── JLinkSettings.ini          # J-Link 调试器配置
├── RMA-MotorStack.ioc              # STM32CubeMX 工程 (用于修改引脚配置)
├── Refer/                          # 参考资料
│   ├── RoboMaster 开发板A型 原理图.pdf     # 电路原理图
│   ├── RoboMaster 开发板A型 位号图.pdf     # PCB 位号图
│   └── RoboMaster 开发板A型使用说明.pdf    # 使用说明书
└── PROJECT_DOCS.md                 # 本文档
```

## 3. 功能模块与原理图对应关系

### 3.1 IMU 姿态传感器
- **文件**: `BSP/bsp_imu.c/h`, `App/app_imu.c/h`
- **原理图**: 第 3 页 - MPU6500 (U5) + IST8310 (U6) 三轴磁力计
- **接口**: SPI5 (PF7 SCK, PF8 MISO, PF9 MOSI, PF6 CS)
- **功能**: 读取加速度/陀螺仪/磁力计数据, 通过 Mahony AHRS 算法计算四元数姿态 (Roll/Pitch/Yaw), 通过 USART6 串口输出
- **说明**: 使用 CMSIS DSP 库的 invSqrt 函数

### 3.2 PWM 电机控制
- **文件**: `Core/Src/tim.c`, `App/app_motor.c/h`
- **原理图**: 第 4-5 页 - PWM 输出接口 (M1-M4 电机接口)
- **接口**:
  - TIM2 CH1-4: PA0, PA1, PA2, PA3 (PWM 组 1)
  - TIM4 CH1-4: PD12, PD13, PD14, PD15 (PWM 组 2)
  - TIM5 CH1-4: PH10, PH11, PH12, PI0 (PWM 组 3)
  - TIM8 CH1-4: PI5, PI6, PI7, PI2 (PWM 组 4)
- **功能**: 共 16 路 PWM 输出, 频率 50Hz, 分辨率 10000, 适配舵机和电调控制

### 3.3 遥控器接收 (DBUS)
- **文件**: `BSP/bsp_uart.c/h`, `App/app_rc.c/h`
- **原理图**: 第 4 页 - USART1 (DBUS 接口)
- **接口**: USART1 (PB6 TX, PB7 RX) + DMA2 Stream2
- **功能**: 接收 DJI DR16 遥控器 DBUS 协议数据 (4 通道摇杆 + 2 开关), 串口参数 100000bps/8E1

### 3.4 OLED 显示屏
- **文件**: `BSP/bsp_oled.c/h + oledfont.h`, `App/app_display.c/h`
- **原理图**: 第 4 页 - SPI1 OLED 接口
- **接口**: SPI1 (PB3 SCK, PB4 MISO, PA7 MOSI) + PE4 DC + PE5 RST
- **功能**: 128×64 OLED 显示, 支持字符/数字/字符串/LOGO 显示

### 3.5 CAN 总线 (UWB 定位 + 电机通信)
- **文件**: `BSP/bsp_can.c/h`, `App/app_uwb.c/h`
- **原理图**: 第 5 页 - CAN1 (PD0 RX, PD1 TX), CAN2 (PB12 RX, PB13 TX)
- **功能**: CAN1/CAN2 双路 CAN 总线, 波特率 1Mbps, 用于 UWB 定位模块和 M3508 电机通信

### 3.6 SD 卡 (数据存储)
- **文件**: `BSP/bsp_sd.c/h + sd_card.c/h`, `Core/Src/sdio.c + sd_diskio.c`
- **原理图**: 第 6 页 - SDIO (Micro SD 卡槽)
- **接口**: SDIO 4-bit (PC8 D0, PC9 D1, PC10 D2, PC11 D3, PC12 CK, PD2 CMD)
- **功能**: FATFS 文件系统, 支持文件创建/读写, 用于数据日志记录

### 3.7 USB 虚拟串口
- **文件**: `Core/Src/usb_device.c + usbd_cdc_if.c + usbd_conf.c + usbd_desc.c`
- **原理图**: 第 3 页 - USB OTG FS (PA11 DM, PA12 DP)
- **功能**: USB CDC 虚拟串口, PC 端识别为 COM 口进行数据传输

### 3.8 ADC 旋钮
- **文件**: `Core/Src/adc.c`
- **原理图**: 第 4 页 - ADC1_IN6 (PA6)
- **功能**: 读取开发板上的电位器旋钮值 (12 位 ADC)

### 3.9 调试串口
- **文件**: `Core/Src/usart.c`
- **接口**: USART6 (PG14 TX, PG9 RX), 115200bps/8N1
- **功能**: printf 调试输出

### 3.10 LED 指示灯
- **接口**: PE11 (红色), PF14 (绿色)
- **功能**: 程序运行状态指示

## 4. 关键引脚分配总表

| 外设 | 引脚 | 功能 | 原理图位置 |
|---|---|---|---|
| USART1 | PB6(TX), PB7(RX) | DBUS 遥控器 | 第4页 |
| USART6 | PG14(TX), PG9(RX) | 调试串口 | 第4页 |
| SPI1 | PB3(SCK), PB4(MISO), PA7(MOSI) | OLED 显示屏 | 第4页 |
| SPI5 | PF7(SCK), PF8(MISO), PF9(MOSI), PF6(CS) | MPU6500 IMU | 第3页 |
| SDIO | PC8(D0), PC9(D1), PC10(D2), PC11(D3), PC12(CK), PD2(CMD) | Micro SD 卡 | 第6页 |
| CAN1 | PD0(RX), PD1(TX) | CAN 总线 1 | 第5页 |
| CAN2 | PB12(RX), PB13(TX) | CAN 总线 2 | 第5页 |
| TIM2 | PA0-3 | PWM 组1 | 第5页 |
| TIM4 | PD12-15 | PWM 组2 | 第5页 |
| TIM5 | PH10-12, PI0 | PWM 组3 | 第5页 |
| TIM8 | PI5-7, PI2 | PWM 组4 | 第5页 |
| ADC1 | PA6 | 旋钮 | 第4页 |
| USB OTG | PA11(DM), PA12(DP) | USB CDC | 第3页 |
| LED | PE11(红), PF14(绿) | 指示灯 | 第4页 |
| OLED | PE4(DC), PE5(RST) | OLED 控制 | 第4页 |

## 5. 参考文档说明

| 文件 | 位置 | 用途 |
|---|---|---|
| 原理图 (RoboMaster 开发板A型 原理图.pdf) | `Refer/` | 电路连接参考, 引脚确认 |
| 位号图 (RoboMaster 开发板A型 位号图.pdf) | `Refer/` | PCB 元件位置 |
| 使用说明 (RoboMaster 开发板A型使用说明.pdf) | `Refer/` | 开发板功能介绍, 接口说明 |

## 6. 使用方式

1. 用 Keil MDK5 打开 `MDK-ARM/RMA-MotorStack.uvprojx`
2. 编译 (F7) 确认无报错
3. 通过 J-Link 或 ST-Link 连接开发板 SWD 接口 (CN1)
4. 下载 (F8) 到开发板
5. 如需修改外设引脚配置, 使用 STM32CubeMX 打开 `RMA-MotorStack.ioc`

## 7. 参考仓库

- 原始示例代码: https://github.com/RoboMaster/DevelopmentBoard-Examples
- 产品信息: https://store.dji.com/cn/product/rm-development-board-type-a?vid=42041
