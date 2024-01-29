# plugin demo

本项目旨在演示如何在STM32中使用插件功能，分为主程序、插件、引导程序三部分：
* 主程序：实现设备的主要逻辑，并为插件提供交互接口，也称为ROM；
* 插件：由主程序在不同时机调用不同入口（实际上是经引导程序中转调用），并调用主程序提供的接口实现扩展逻辑；
* 引导程序：基于开源openblt，专用于更新插件。引导程序实现了插件的兼容接口，是对插件进行包装，对于主程序而言引导程序就是插件。

演示程序基于STM32F407VET6双CAN开发板(TB_626070401517)，配置是168MHz主频、512k FLASH和128k RAM。

演示的功能是插件通过主程序提供的CAN收发接口与主程序通信，设置主程序的LED闪烁频率，同时插件与外部CAN设备通信，响应收到的ping。

## 目录结构
```
|- bl                         引导程序
|   |- bin                       编译好的二进制
|   |- lib                       依赖库
|   |- obj                       编译中间文件
|   |- bl_conf.h                 openblt配置
|   |- flash_layout.c            openblt的flash布局定义
|   |- hooks.c                   openblt回调接口
|   |- led.c                     led闪烁实现
|   |- led.h                
|   |- bl_interface.c            bl的接口实现、bl接口地址表，插件接口桩函数
|   |- startup_stm32f4.s         主入口、ROM和插件接口地址
|   |- makefile                  编译脚本
|   └- STM32F4_FLASH.ld          链接脚本，定义FLASH、RAM范围
|- main                       主程序
|   |- build                     中间文件和编译好的二进制
|   |- Drivers                   依赖库
|   |- Inc                       
|   |- Src                       
|   |   |- main.c                c程序入口
|   |   └- rom_interface.c       主程序对插件开放的接口实现
|   |- main_stm32f407.ioc        stm32cubemx项目文件
|   |- startup_stm32f407xx.s     主入口、中断向量
|   |- makefile                  编译脚本（警告：CUBE重新生成将会覆盖修改）
|   └- STM32F407VETx_FLASH.ld    链接脚本（警告：CUBE重新生成将会覆盖修改）
|- plugin                     插件
|   |- bin                       编译好的二进制
|   |- obj                       编译中间文件
|   |- plugin.c                  插件接口实现
|   |- plugin.h                  ROM接口插件接口和定义，与主程序共享
|   |- startup_stm32f.s          主入口、ROM和插件接口地址
|   |- STM32F4_FLASH.ld          链接脚本
|   └- makefile                  编译脚本
└- openblt                    开源bootloader库openblt
   |- Host                       固件下载软件
   └- Target                     bootloader实现
```

## 接口定义

### ROM接口

|接口|描述|
|--|--|
|get_tick|获取启动后运行时间，单位毫秒|
|send_can|CAN发包|
|send_uart|串口发送|

### 插件接口

|接口|描述|
|--|--|
|entry|主程序完成初始化后调用插件启动|
|loop|在主程序主循环中调用插件主循环|
|handle_can|主程序收到CAN包时调用|
|handle_uart|主程序收到串口数据时调用|

### 调整

如需调整接口，需要修改以下文件：

|文件|位置|说明|
|--|--|--|
|plugin/plugin.h|5-7行|定义ROM接口|
|plugin/plugin.h|14-17行|定义插件接口|
|plugin/plugin.c||实现插件接口,定义插件接口地址表|
|bl/bl_interface.c||实现BL兼容插件接口,定义插件接口地址表|
|main/rom_interface.c||实现ROM接口,定义ROM接口地址表|

## FLASH/RAM布局定义

### FLASH

|开始|结束|长度|定义|
|--|--|--|--|
|0x8000000|0x805FF80|384k-128|主程序|
|0x805FF80|0x8060000|128|主程序（ROM）接口地址表|
|0x8060000|0x8060080|128|引导程序的插件兼容接口地址表|
|0x8060080|0x8062000|8k-128|引导程序|
|0x8006000|0x8062080|128|插件接口地址表|
|0x8062080|0x8080000|120k-128|插件|

主程序最后128字节是ROM接口地址表，引导程序和插件最前128字节是插件接口地址表。

也就是；前面384k主程序，中间8k引导程序，最后120k插件。

引导程序、主程序和插件的代码空间，各部分的FLASH地址不允许重叠，但插件接口地址表例外，它在主程序的链接脚本中也有定义，下载主程序时也会写入，只不过写入的是桩函数地址，目的是在只下载了主程序没有下载插件时主程序调用插件接口也不会出错，引导程序也同理。

### RAM

|开始|结束|长度|定义|
|--|--|--|--|
|0x20000000|0x2001C000|112k|主程序|
|0x2001C000|0x2001D000|4k|引导程序|
|0x2001D000|0x20020000|12k|插件|

也就是；前面112k主程序，中间4k引导程序，最后12k插件。
在更新插件的过程中，插件的这12k也会被引导程序暂时占用。

注意：引导程序和插件接口是在主程序上下文中调用的，它没有独立的栈空间，而是共享主程序的栈空间

### 调整

如需调整布局，需要修改以下文件：

|文件|位置|说明|
|--|--|--|
|bl/flash_layout.c|flashLayout数组|定义所有可以通过bootloader更新的区块，包含插件的全部FLASH|
|bl/STM32F4_FLASH.ld|67-68行|定义bootloader使用RAM|
|bl/STM32F4_FLASH.ld|69-73行|定义bootloader及接口地址表使用的FLASH|
|main/STM32F407VETx_FLASH.ld|64～65行|定义主程序使用RAM|
|main/STM32F407VETx_FLASH.ld|66～68行|定义主程序及接口地址表使用的FLASH|
|plugin/STM32F4_FLASH.ld|63行|定义插件使用RAM|
|plugin/STM32F4_FLASH.ld|64～66行|定义插件及接口地址表使用的FLASH|


## 编译

拉取代码
``` bash
git clone https://github.com/ntdll1/plugin_demo.git
cd plugin_demo
git submodule update --init
```

工具链可以从 https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads/ 下载，或者用其他IDE自带的

### Windows
``` bat
REM 编译引导程序
cd bl
make TOOL_PATH=d:\toolchain-gccarmnoneeabi\bin
cd ..

REM 编译主程序
cd main
make TOOL_PATH=d:\toolchain-gccarmnoneeabi\bin
cd ..

REM 编译插件
cd plugin
make TOOL_PATH=d:\toolchain-gccarmnoneeabi\bin
cd ..
```

### Linux/Mac
``` bash
# 编译引导程序
cd bl
make TOOL_PATH=$HOME/toolchain-gccarmnoneeabi/bin/
cd ..

# 编译主程序
cd main
make TOOL_PATH=$HOME/toolchain-gccarmnoneeabi/bin/
cd ..

# 编译插件
cd plugin
make TOOL_PATH=$HOME/toolchain-gccarmnoneeabi/bin/
cd ..
```

## 测试

### 烧写主程序和引导程序

连接JLINK或者CMSIS-DAP，针脚对应接：SWDIO、SWCLK、GND、VCC

先后烧写主程序 main/build/main_stm32f407.elf 和引导程序 bl/bin/bl_stm32f407.elf，例如使用pyocd：
``` bash
pyocd flash -t stm32f407ve main/build/main_stm32f407.elf

pyocd flash -t stm32f407ve bl/bin/bl_stm32f407.elf
```

也可以使用STM32CubeProgrammer基于ST芯片内置的usb bootloader下载，烧写时需要通过跳线帽设置BOOT0=1，烧写完恢复BOOT0=0，具体操作参考 https://www.st.com/resource/en/user_manual/um2237-stm32cubeprogrammer-software-description-stmicroelectronics.pdf 及 https://www.st.com/resource/en/application_note/an2606-stm32-microcontroller-system-memory-boot-mode-stmicroelectronics.pdf 。

主程序烧写后，在烧写bootloader前，正常可以看到LED1慢速闪烁（约0.5Hz）。
bootloader烧写后，在下载插件，正常可以看到LED2快速闪烁（约4Hz），表示等待下载，此时主程序和引导程序是一起执行的。

### 下载插件

由于插件与bootloader共用了一个flash sector，通过前面的方式烧写插件会把bootloader擦除，虽然也可以在烧写bootloader后不擦除直接烧写插件，但不是推荐做法，这里演示的是基于我们的bootloader通过串口更新插件。

#### 连接插件下载接口（CAN/RS232）

##### CAN
用USB转CAN工具连接开发板，建议用PEAK-CAN或兼容版本（如https://github.com/moonglow/pcan_cantact）
CANH-CANH、CANL-CANL对应连接。

##### RS232
用USB转串口工具连接开发板

|开发板针脚|USB转串口模块针脚|含义|
|--|--|--|
|PA9|RX|开发板TX接USB转串口RX|
|PA10|TX|开发板RX接USB转串口TX|
|GND|GND|共地|
|3.3V|3.3V|仅用USB转串口模块给开发板供电时接，注意和JLINK供电只能2选1|

#### Windows
1. 运行openblt/Host/MicroBoot.exe
2. 点击Settings...
3. 接口根据实际选择XCP on RS232 或 XCP on CAN
4. 设备选择对应串口或CAN接口
5. 波特率选择115200(RS232)或500Kbps(CAN)
6. 点击Browse...
7. 选择plugin/bin/plugin_stm32f407.srec，注意只支持srec格式
8. 等待进度条满下载完成后程序自动退出

如果选择了文件进度条不动，可能的原因：
* 连接问题，检查TX/RX或CANH/CANL是否接错或者接反
* 串口/CAN设备和波特率选择不对
* 串口/CAN驱动是否正常，检查设备管理器
* bootloader未烧写成功，重新烧写
* 主程序或Bootloader死机，此时应该能看到LED不再闪烁，需要调试分析具体原因

下载插件后，每次重启开发板，前5秒LED1慢速闪烁（4Hz），5秒后变快（10Hz），表示插件正常运行，调用主程序通信接口设置了参数。

如果出现插件死机，可能因烧写不完整等原因导致，可以按住KEY2不松再按一下RESET，等待bootloader启动后再松开KEY2，BL会跳过插件执行，此时可以重新下载正确的插件。

#### Linux/Mac
使用BootCommander下载，需要先编译，具体参考openblt官方文档 https://www.feaser.com/openblt/doku.php?id=manual:bootcommander
