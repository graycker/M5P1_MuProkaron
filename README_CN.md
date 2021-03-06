# MuProkaron

![Build](https://travis-ci.org/EDI-Systems/M5P1_MuProkaron.svg?branch=master) 
[![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/1684/badge)](https://bestpractices.coreinfrastructure.org/projects/1684) 
[![Join the chat at https://gitter.im/M5P1_MuProkaron/Lobby](https://badges.gitter.im/M5P1_MuProkaron/Lobby.svg)](https://gitter.im/M5P1_MuProkaron/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

**RMP** 是一个专注于形式化验证和简易可用的小型实时系统。它通过采取形式化方法来确保系统的可靠性（当前验证工作还未完成）。所有的实时操作系统必备的功能它都具备，但是并不在此基础上提供更多可选组件以确保内核的精炼性。这样，得到的内核就是一个最小化的内核，可以很方便地对它进行形式化验证。同时，它还可以作为客户操作系统运行在虚拟机监视器上。本系统比以_FreeRTOS_和_RT-Thread_为代表的全功能系统相比要**小**得多，而且理解起来应该也相对容易得多。

如果想要参与开发，请阅读 **[参与](CONTRIBUTING.md)** 和 **[规范](CODE_OF_CONDUCT.md)** 两个指导文档。如果要提交拉取请求，请使用 **[拉取请求模板](PULL_REQUEST_TEMPLATE.md)** 。
本软件采用 **三种不同的授权** ：你可以选择 **[LGPL v3](LICENSE.md)** ，也可以使用 **[经修改的MIT协议](MODMIT.md)** 。 如果有特殊需求， **商业授权** 也是可以的。

对于那些由微控制器厂商提供的硬件抽象层软件包，请到 **[M0P0_Library](https://github.com/EDI-Systems/M0P0_Library)** 软件仓库自行下载。

## 快速演示
### 可在Linux上直接运行的Demo
从 **[这里](Project/ECLIPSE-GCC-LINUX/RMP/Debug/RMP)** 下载预编译的x86-32位Linux系统上可运行的二进制，观察性能测试的运行结果！

### 基础线程操作
**创建一个线程**
```C
    RMP_Thd_Crt(&Thd_1            /* 线程控制块 */, 
                Func_1            /* 线程入口 */,
                &Stack_1[238]     /* 线程栈地址 */,
                (void*)0x12345678 /* 参数 */,
                1                 /* 优先级 */, 
                5                 /* 时间片 */);
```
**删除一个线程**
```C
    RMP_Thd_Del(&Thd_1            /* 线程控制块 */);
```
**悬起一个线程**
```C
    RMP_Thd_Suspend(&Thd_1        /* 线程控制块 */);
```
**解除线程悬起**
```C
    RMP_Thd_Resume(&Thd_1         /* 线程控制块 */);
```

### 线程延时
![Delay](https://raw.githubusercontent.com/EDI-Systems/M5P1_MuProkaron/master/Documents/Demo/Delay.gif)
```C
    void Func_1(void* Param)
    {
        RMP_PRINTK_S("Parameter passed is ");
        RMP_PRINTK_U((ptr_t)Param);
        RMP_PRINTK_S("\r\n");
        while(1)
        {
            RMP_Thd_Delay(30000);
            RMP_PRINTK_S("Delayed 30000 cycles\r\n\r\n");
        };
    }

    void RMP_Init_Hook(void)
    {
        RMP_Thd_Crt(&Thd_1, Func_1, &Stack_1[238], (void*)0x12345678, 1, 5);
    }
```
### 从一个线程向另一个线程的邮箱发送
![Send](https://raw.githubusercontent.com/EDI-Systems/M5P1_MuProkaron/master/Documents/Demo/Send.gif)
```C
    void Func_1(void* Param)
    {
        ptr_t Time=0;
        while(1)
        {
            RMP_Thd_Delay(30000);
            RMP_Thd_Snd(&Thd_2, Time, RMP_MAX_SLICES);
            Time++;
        };
    }

    void Func_2(void* Param)
    {
        ptr_t Data;
        while(1)
        {
            RMP_Thd_Rcv(&Data, RMP_MAX_SLICES);
            RMP_PRINTK_S("Received ");
            RMP_PRINTK_I(Data);
            RMP_PRINTK_S("\n");
        };
    }

    void RMP_Init_Hook(void)
    {
        RMP_Thd_Crt(&Thd_1, Func_1, &Stack_1[238], (void*)0x12345678, 1, 5);
        RMP_Thd_Crt(&Thd_2, Func_2, &Stack_2[238], (void*)0x87654321, 1, 5);
    }
```

### 计数信号量
![Semaphore](https://raw.githubusercontent.com/EDI-Systems/M5P1_MuProkaron/master/Documents/Demo/Semaphore.gif)
```C
    void Func_1(void* Param)
    {
        while(1)
        {
            RMP_Thd_Delay(30000);
            RMP_Sem_Post(&Sem_1, 1);
        };
    }

    void Func_2(void* Param)
    {
        ptr_t Data;
        while(1)
        {
            RMP_Sem_Pend(&Sem_1, RMP_MAX_SLICES);
            RMP_PRINTK_S("Semaphore successfully acquired!\r\n\r\n");
        };
    }

    void RMP_Init_Hook(void)
    {
        RMP_Sem_Crt(&Sem_1,0);
        RMP_Thd_Crt(&Thd_1, Func_1, &Stack_1[238], (void*)0x12345678, 1, 5);
        RMP_Thd_Crt(&Thd_2, Func_2, &Stack_2[238], (void*)0x87654321, 1, 5);
    }
```

### 所有被支持架构上的典型性能数据
|Machine      |Toolchain     |Flash|SRAM|Yield|Mailbox|Semaphore|Mailbox/Int|Semaphore/Int|
|:-----------:|:------------:|:---:|:--:|:---:|:-----:|:-------:|:---------:|:-----------:|
|MSP430       |TI CCS6       |2.90 |0.64|1254 |2386   |2281     |2378       |2245         |
|MSP430       |GCC           |TBT  |TBT |TBT  |TBT    |TBT      |TBT        |TBT          |
|Cortex-M0    |Keil uVision 5|4.94 |1.65|374  |663    |616      |659        |617          |
|Cortex-M0+   |Keil uVision 5|6.25 |1.65|334  |607    |544      |588        |552          |
|Cortex-M3    |Keil uVision 5|2.60 |1.65|246  |456    |422      |443        |409          |
|Cortex-M3    |GCC           |TBT  |TBT |TBT  |TBT    |TBT      |TBT        |TBT          |
|Cortex-M4    |Keil uVision 5|2.70 |1.66|184  |339    |325      |374        |361          |
|Cortex-M4    |GCC           |TBT  |TBT |TBT  |TBT    |TBT      |TBT        |TBT          |
|Cortex-M7    |Keil uVision 5|6.66 |1.65|170  |256    |230      |274        |268          |
|Cortex-M7    |GCC           |TBT  |TBT |TBT  |TBT    |TBT      |TBT        |TBT          |
|Cortex-R4    |Keil uVision 5|TBT  |TBT |TBT  |TBT    |TBT      |TBT        |TBT          |
|Cortex-R4    |GCC           |TBT  |TBT |TBT  |TBT    |TBT      |TBT        |TBT          |
|Cortex-R5    |Keil uVision 5|TBT  |TBT |TBT  |TBT    |TBT      |TBT        |TBT          |
|Cortex-R5    |GCC           |TBT  |TBT |TBT  |TBT    |TBT      |TBT        |TBT          |
|MIPS M14k    |XC32-GCC      |17.2 |2.46|264  |358    |340      |421        |415          |
|X86-LINUX    |GCC           |N/A  |N/A |33000|35000  |33000    |35000      |33000        |

**Flash和SRAM消耗以kB计，其他数据以CPU指令周期计。这里列出的所有值都是典型（有意义的系统配置）值而非绝对意义上的最小值，因为纯技术层面的最小配置在实际工程中很少是真正有用的。**

- MSP430平台使用MSP430FR5994进行评估.
- Cortex-M0平台使用STM32F030F4P6进行评估.
- Cortex-M0+平台使用STM32L053C8T6进行评估.
- Cortex-M3平台使用STM32F103RET6进行评估.
- Cortex-M4平台使用STM32F405RGT6进行评估.
- Cortex-M7平台使用STM32F767IGT6进行评估.
- MIPS M14k平台使用PIC32MZ2048EFM100进行评估.
- X86 Linux平台使用Ubuntu 16.04和i7-4820k @ 3.7GHz进行评估.

All compiler options are the highest optimization (usually -O3) and optimized for time. 
- Yield: The time to yield between different threads.  
- Mailbox: The mailbox communication time between two threads.  
- Semaphore: The semaphore communication time between two threads.  
- Mailbox/Int: The time to send to a mailbox from interrupt.  
- Semaphore/Int: The time to post to a semaphore from interrupt.  

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

You need **_Cortex-M or Cortex-R or MIPS or MSP430_** microcontroller development kits to run the tests. This RTOS focuses on value-line MCUs and do not concentrate on high-end MCUs or MPUs. Do not use QEMU simulator to test the projects because they do not behave correctly in many scenarios.  
If you don't have a development board, a **_x86-based Linux port_** of RMP is also available. However, running RMP on top of linux uses the [ptrace](https://en.wikipedia.org/wiki/Ptrace) system call and [signal](https://en.wikipedia.org/wiki/Signal_(IPC)) system, thus it is not particularly fast. Just run the example and observe benchmark output.
Other platform supports should be simple to implement, however they are not scheduled yet. For Cortex-A and other CPUs with a memory management unit ([MMU](https://en.wikipedia.org/wiki/Memory_management_unit)), go [M7M1_MuEukaron](https://github.com/EDI-Systems/M7M1_MuEukaron) _Real-Time Multi-Core Microkernel_ instead; M7M1 supports some Cortex-Ms and Cortex-Rs as well.

### Compilation

The **Vendor Toolchain** or **Eclipse** projects for various microcontrollers are available in the **_Project_** folder. Refer to the readme files in each folder for specific instructions about how to run them. However, keep in mind that some examples may need vendor-specific libraries such as the STMicroelectronics HAL. Some additional drivers may be required too. These can be found in **[M0P0_Library](https://github.com/EDI-Systems/M0P0_Library)** repo.

## Running the tests

To run the sample programs, simply download them into the development board and start step-by-step debugging. Some examples will use one or two LEDs to indicate the system status. In that case, it is necessary to fill the LED blinking wrapper functions.

## Deployment

When deploying this into a production system, it is recommended that you read the manual in the **_Documents_** folder carefully to configure all macros correctly.

## Built With

- Keil uVision 5 (armcc)
- Code composer studio
- gcc/clang-llvm

Other toolchains are not recommended nor supported at this point, though it might be possible to support them later on.

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## License

This project is licensed under the GPLv3 License - see the [LICENSE.md](LICENSE.md) file for details. However, commercial licenses are also available.

## EDI Project Information
Mutate - Protero - Prokaron (M5P1 R4T1)
