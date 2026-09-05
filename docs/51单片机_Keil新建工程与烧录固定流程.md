# 51 单片机：Keil 新建工程、编译与 STC-ISP 烧录固定流程

> 适用于你当前这块 51 单片机实验板。  
> 实际开发板芯片按 **IAP15F2K61S2** 处理；在 Keil C51 新建工程时，如果没有直接使用该器件，则使用 **AT89C52** 作为兼容工程目标。  
> 烧录时，在 **STC-ISP** 中选择实际芯片 **IAP15F2K61S2**。

---

## 一、为什么新建工程时先选 Legacy Device Database？

在 Keil 的 `Select Device for Target 'Target 1'...` 窗口顶部，会看到器件数据库选择框。

这里固定选择：

**Legacy Device Database [no RTE]**

### 原因

我们现在学习的是传统的 **8051 / 51 单片机 C51 开发方式**。

`Legacy Device Database` 中保存的是传统 8051 系列器件信息，能够直接找到类似：

- Atmel → AT89C52
- ISSI → IS89C52
- NXP → P89C52X2

这些传统 51 系列型号。

而我们现在使用的代码也是标准的 Keil C51 写法，例如：

```c
#include <reg52.h>
```

所以选择 `Legacy Device Database [no RTE]` 最直接，也最符合我们现在的学习方式。

### `no RTE` 是什么意思？

RTE 是 Run-Time Environment，也就是 Keil 新版软件中的运行环境和软件组件管理系统。

我们现在写的是最基础的裸机 51 单片机程序：

```c
P0 = 0x00;
P0 = 0xFF;
```

不需要使用 RTE 软件组件。

所以看到：

**Legacy Device Database [no RTE]**

是正常的，不需要担心。

---

# 二、Keil 每次新建工程的固定步骤

假设本次实验目录为：

```text
C:\Users\Matthew Xie\Desktop\91project\led_260904\00_ws
```

以后每个实验都可以在：

```text
C:\Users\Matthew Xie\Desktop\91project
```

下面建立自己的子文件夹。

---

## 第 1 步：新建 Keil 工程

打开 Keil uVision。

点击：

**Project → New uVision Project...**

进入当前实验文件夹，例如：

```text
C:\Users\Matthew Xie\Desktop\91project\led_260904\00_ws
```

输入工程名称，例如：

```text
led_flash_3
```

然后保存。

---

## 第 2 步：选择器件数据库

出现：

**Select Device for Target 'Target 1'...**

在窗口顶部选择：

**Legacy Device Database [no RTE]**

这是我们以后固定使用的选择。

---

## 第 3 步：选择 Keil 工程目标器件

在搜索框输入：

```text
89C52
```

然后选择：

```text
Atmel
└── AT89C52
```

点击：

**OK**

### 注意

这里选择 `AT89C52`，并不代表你的开发板真的使用 AT89C52。

你的实际实验板芯片仍然按：

**IAP15F2K61S2**

处理。

这里选择 AT89C52 的主要目的是：

**让 Keil 按照兼容的 8051 / 8052 C51 架构来编译我们的基础程序。**

---

## 第 4 步：STARTUP.A51 选择 No

如果 Keil 弹出类似：

**Copy STARTUP.A51 to Project Folder and Add File to Project?**

选择：

**No / 否**

当前基础实验不需要手动加入这个启动文件。

---

# 三、创建 main.c

## 第 5 步：在 Source Group 1 中创建 C 文件

左侧工程栏找到：

```text
Target 1
└── Source Group 1
```

右键：

**Source Group 1**

选择：

**Add New Item to Group 'Source Group 1'**

---

## 第 6 步：选择 C File (.c)

在弹出的 `Add New Item` 窗口中选择：

**C File (.c)**

不要选择：

```text
C++ File (.cpp)
```

虽然你有 C++ 基础，但 51 单片机课程当前使用的是 **Keil C51 / C语言**。

---

## 第 7 步：文件命名

Name 输入：

```text
main.c
```

Location 确认是在当前工程目录，例如：

```text
C:\Users\Matthew Xie\Desktop\91project\led_260904\00_ws
```

点击：

**Add**

然后关闭窗口。

此时左侧应该看到：

```text
Target 1
└── Source Group 1
    └── main.c
```

---

# 四、编写程序

在 `main.c` 中写代码。

例如 LED 全亮闪烁三次后全灭：

```c
#include <reg52.h>

sbit HC138_A = P2^5;
sbit HC138_B = P2^6;
sbit HC138_C = P2^7;

void delay()
{
    unsigned int i, j;

    for(i = 0; i < 500; i++)
    {
        for(j = 0; j < 120; j++);
    }
}

void main()
{
    HC138_C = 1;
    HC138_B = 0;
    HC138_A = 0;

    P0 = 0x00;
    delay();
    P0 = 0xFF;
    delay();

    P0 = 0x00;
    delay();
    P0 = 0xFF;
    delay();

    P0 = 0x00;
    delay();
    P0 = 0xFF;
    delay();

    P0 = 0xFF;

    while(1);
}
```

写完以后：

**Ctrl + S 保存**

---

# 五、让 Keil 生成 HEX 文件

STC-ISP 烧录需要的是 `.hex` 文件。

## 第 8 步：打开 Target 设置

点击：

**Project → Options for Target 'Target 1'**

或者点击工具栏上的：

**Options for Target**

---

## 第 9 步：开启 Create HEX File

进入：

**Output**

勾选：

**Create HEX File**

然后：

**OK**

这个步骤非常重要。

如果没有勾选，Keil 即使编译成功，也可能没有生成我们烧录需要的 `.hex` 文件。

---

# 六、编译程序

## 第 10 步：Build

点击：

**Project → Build Target**

或者使用工具栏的 Build 按钮。

观察下面的 Build Output。

理想结果：

```text
0 Error(s), 0 Warning(s)
```

最重要的是：

```text
0 Error(s)
```

如果存在 Error，不要烧录，先解决编译错误。

---

## 第 11 步：找到 HEX 文件

编译成功以后，会生成类似：

```text
工程名.hex
```

通常在工程目录下面的：

```text
Objects
```

文件夹中。

例如：

```text
C:\Users\Matthew Xie\Desktop\91project\led_260904\00_ws\Objects\led_flash_3.hex
```

具体名称取决于你的工程名。

---

# 七、使用 STC-ISP 烧录

## 第 12 步：连接开发板

使用 USB 将开发板连接电脑。

打开：

**STC-ISP**

---

## 第 13 步：选择实际芯片

在 STC-ISP 中选择实际开发板芯片：

**IAP15F2K61S2**

注意这里和 Keil 不一样。

### Keil 中

```text
AT89C52
```

是为了方便编译。

### STC-ISP 中

```text
IAP15F2K61S2
```

必须选择开发板上的实际芯片型号。

可以简单记成：

```text
Keil：选兼容型号，负责“编译”
STC-ISP：选真实型号，负责“烧录”
```

---

## 第 14 步：选择串口

在 STC-ISP 中选择开发板对应的：

```text
COMx
```

例如：

```text
COM3
COM5
COM7
```

具体数字取决于电脑当前识别到的串口。

---

## 第 15 步：打开 HEX 文件

点击：

**打开程序文件**

选择刚才 Keil 编译生成的：

```text
.hex
```

文件。

---

## 第 16 步：点击 下载/编程

点击：

**下载/编程**

此时 STC-ISP 通常会等待单片机重新上电。

---

## 第 17 步：开发板重新上电

如果一直显示等待：

1. 关闭开发板电源；
2. 再打开开发板电源。

STC-ISP 检测到芯片重新上电后，就会开始下载程序。

出现下载成功提示以后，说明程序已经烧录进单片机。

---

# 八、以后每次直接照着这个清单做

## Keil 部分

1. 建立实验文件夹。
2. `Project → New uVision Project`
3. 选择 `Legacy Device Database [no RTE]`
4. 搜索 `89C52`
5. 选择 `Atmel → AT89C52`
6. STARTUP.A51 选择 `No`
7. `Source Group 1 → Add New Item`
8. 选择 `C File (.c)`
9. 创建 `main.c`
10. 写代码并保存
11. `Options for Target → Output`
12. 勾选 `Create HEX File`
13. `Build Target`
14. 确认 `0 Error(s)`
15. 找到生成的 `.hex`

## STC-ISP 部分

1. USB 连接开发板
2. 打开 STC-ISP
3. 选择真实芯片 `IAP15F2K61S2`
4. 选择正确 COM 串口
5. 打开 Keil 生成的 `.hex`
6. 点击 `下载/编程`
7. 开发板关闭电源再重新打开
8. 等待下载成功
9. 观察开发板运行结果

---

# 九、最容易混淆的三个地方

### 1. 为什么 Keil 选 AT89C52，STC-ISP 却选 IAP15F2K61S2？

因为它们承担的任务不同：

```text
Keil
↓
编译代码
↓
使用兼容的 AT89C52 工程配置
```

而：

```text
STC-ISP
↓
真正与开发板芯片通信
↓
必须选择 IAP15F2K61S2
```

---

### 2. 为什么选择 Legacy Device Database？

因为我们现在学习的是传统 8051 C51 裸机开发。

这里可以直接找到 AT89C52 等传统 51 器件，而且不需要使用 Keil 的 RTE 软件组件。

所以以后看到器件选择窗口：

**先选 Legacy Device Database [no RTE]。**

---

### 3. 为什么文件选 C File 而不是 C++ File？

因为当前课程和代码使用的是：

**Keil C51 的 C 语言**

典型代码：

```c
#include <reg52.h>

sbit LED = P0^0;

void main()
{
}
```

所以源文件统一使用：

```text
main.c
```

而不是：

```text
main.cpp
```

---

# 十、一句话记忆

以后每次写 51 单片机程序：

**Legacy → AT89C52 → main.c → 写程序 → Create HEX File → Build → STC-ISP 选 IAP15F2K61S2 → 打开 HEX → 下载/编程 → 重新上电。**
