# plugin demo

本项目旨在演示如何在STM32中使用插件功能，分为主程序、插件、引导程序三部分：
* 主程序：实现设备的主要逻辑，并为插件提供交互接口，也称为ROM；
* 插件：由主程序在不同时机调用不同入口，并调用主程序提供的接口实现扩展逻辑；
* 引导程序：固件更新的实现，基于开源openblt，可分别烧写主程序和插件。

演示程序基于STM32F103最小开发板BluePill https://stm32-base.org/boards/STM32F103C8T6-Blue-Pill.html 不过这里用的是阉割版（C6T6），配置是32k FLASH和10k RAM，可以兼容C8T6版本。

演示的功能是插件通过主程序提供的参数写入接口设置LED闪烁频率。

## 目录结构
```
|- bl                   引导程序
|   |- bin                  编译好的二进制
|   |- lib                  依赖库
|   |- obj                  编译中间文件
|   |- bl_conf.h            openblt配置
|   |- flash_layout.c       openblt的flash布局定义
|   |- hooks.c              openblt回调接口
|   |- led.c                led闪烁实现
|   |- led.h                
|   |- main.c               c程序入口
|   |- makefile             编译脚本
|   |- startup_stm32f103.s  主入口、中断向量
|   └- STM32F103C6_FLASH.ld 链接脚本，定义FLASH、RAM范围
|- main                 主程序
|   |- bin                  编译好的二进制
|   |- lib                  依赖库
|   |- obj                  编译中间文件
|   |- boot.c               固件更新请求侦测，以实现在线更新
|   |- boot.h               
|   |- led.c                led闪烁实现
|   |- led.h                
|   |- main.c               c程序入口
|   |- rom_interface.c      主程序对插件开放的接口实现
|   |- makefile             编译脚本
|   |- startup_stm32f103.s  主入口、中断、ROM和插件接口地址表
|   └- STM32F103C6_FLASH.ld 链接脚本
|- plugin               插件
|   |- bin                  编译好的二进制
|   |- obj                  编译中间文件
|   |- main.c               c程序入口
|   |- plugin.c             插件接口实现
|   |- plugin.h             ROM接口插件接口和定义，与主程序共享
|   |- makefile             编译脚本
|   |- startup_stm32f103.s  主入口、ROM和插件接口地址表
|   └- STM32F103C6_FLASH.ld 链接脚本
└- openblt              开源bootloader库openblt
   |- Host                 固件下载软件
   └- Target               bootloader实现
```

## 接口定义

### ROM接口

|接口|描述|
|--|--|
|get_tick|获取启动后运行时间，单位毫秒|
|read_reg|读参数，例如读取电机转速|
|write_reg|写参数，例如设置电机开始运动|
|send_serial|发包|

### 插件接口

|接口|描述|
|--|--|
|entry|主程序完成初始化后调用插件启动|
|loop|在主程序主循环中调用插件主循环|
|handle_serial|主程序收到包时调用|

### 调整

如需调整接口，需要修改以下文件：

|文件|位置|说明|
|--|--|--|
|plugin/plugin.h|7行|定义ROM接口|
|plugin/plugin.h|16行|定义插件接口|
|plugin/plugin.c||实现插件接口|
|plugin/startup_stm32f103.s|73行|定义插件接口地址表|
|main/rom_interface.c||实现ROM接口|
|main/startup_stm32f103.s|383行|定义ROM接口地址表|
|main/startup_stm32f103.s|388行|定义插件接口桩函数地址表，需要填入对应数量的桩函数|

分支add_interface演示了如何增加一个接口，可与主分支的比较具体修改。

## FLASH/RAM布局定义

### FLASH

|开始|结束|长度|定义|
|--|--|--|--|
|0x8000000|0x8002000|8k|引导程序|
|0x8002000|0x8005F80|16k-128|主程序|
|0x8005F80|0x8006000|128|主程序（ROM）接口地址表|
|0x8006000|0x8006080|128|插件接口地址表|
|0x8006080|0x8008000|8k-128|插件|

主程序最后128字节是ROM接口地址表，插件最前128字节是插件接口地址表。

也就是；前面8k引导程序，中间16k主程序，最后8k插件。

引导程序、主程序和插件的代码空间，各部分的FLASH地址不允许重叠，但插件接口地址表例外，它在主程序的链接脚本中也有定义，下载主程序时也会写入，只不过写入的是桩函数地址，目的是在只下载了主程序没有下载插件时主程序调用插件接口也不会出错。

### RAM

|开始|结束|长度|定义|
|--|--|--|--|
|0x20000000|0x20001800|6k|主程序|
|0x20001800|0x20002800|4k|插件|

也就是；前面6k主程序，最后4k插件。这里没有列出引导程序，它使用了全部RAM

主程序最后128字节是ROM接口地址表，插件最前128字节是插件接口地址表。

注意：插件接口是在主程序上下文中调用的，它没有独立的栈空间，而是共享主程序的栈空间

### 调整

如需调整布局，需要修改以下文件：

|文件|位置|说明|
|--|--|--|
|bl/flash_layout.c|flashLayout数组|定义所有可以通过bootloader更新的区块，包含主程序和插件的全部FLASH，但不包括bootloader自身|
|bl/STM32F103C6_FLASH.ld|44行|定义bootloader使用的FLASH，一般第一个区块|
|bl/STM32F103C6_FLASH.ld|35、43行|定义bootloader使用的RAM和栈，一般为整个RAM|
|main/STM32F103C6_FLASH.ld|44行|定义主程序使用的FLASH，一般在bootloader后|
|main/STM32F103C6_FLASH.ld|35、43行|定义主程序使用的RAM和栈，一般为RAM前面一大部分|
|plugin/STM32F103C6_FLASH.ld|42行|定义插件使用的FLASH|
|plugin/STM32F103C6_FLASH.ld|41行|定义插件使用的RAM和栈，一般为RAM后面一小部分|
|plugin/plugin.h|4、5行|定义ROM接口地址表和插件接口地址表的存放位置，此文件主程序和插件共用|


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

### 烧写引导程序

连接JLINK或者CMSIS-DAP，针脚对应接：SWDIO、SWCLK、GND、VCC

烧写 bl/bin/bl_stm32f103.elf，例如使用pyocd：
``` bash
pyocd flash -t stm32f103c6 bl/bin/bl_stm32f103.elf
```

bootloader烧写后，在下载主程序前，正常可以看到LED快速闪烁（约4Hz），表示等待下载。

### 连接串口

用USB转串口工具连接开发板

|开发板针脚|USB转串口模块针脚|含义|
|--|--|--|
|PA9|RX|开发板TX接USB转串口RX|
|PA10|TX|开发板RX接USB转串口TX|
|GND|GND|共地|
|3.3V|3.3V|仅用USB转串口模块给开发板供电时接，注意和JLINK供电只能2选1|

### 下载主程序

#### Windows
1. 运行openblt/Host/MicroBoot.exe
2. 点击Settings...
3. 接口选择XCP on RS232
4. 设备选择对应串口
5. 波特率选择115200 （非默认）
6. 点击Browse...
7. 选择main/bin/main_stm32f103.srec，注意只支持srec格式
8. 等待进度条满下载完成后程序自动退出

如果选择了文件进度条不动，可能的原因：
* 连接问题，检查TX/RX是否接错或者接反
* 串口设备和波特率选择不对
* 串口驱动是否正常，检查设备管理器
* bootloader未烧写成功，重新烧写
* 主程序或插件死机，可尝试在选择文件后按开发板reset按钮，bootloader启动有短暂的等待时间

主程序烧写后，正常可以看到LED慢速闪烁（约0.5Hz）。

#### Linux/Mac
使用BootCommander下载，需要先编译，具体参考openblt官方文档 https://www.feaser.com/openblt/doku.php?id=manual:bootcommander

### 下载插件

先下载主程序再下载插件。与下载主程序类似，只是选择的文件是plugin/bin/plugin_stm32f103.srec

下载插件后，每次重启开发板，前5秒LED慢速闪烁（0.5Hz），5秒后变快（2Hz），表示插件正常运行，调用主程序接口设置了参数。
