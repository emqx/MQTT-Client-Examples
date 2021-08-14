# EasyFlash

[![GitHub release](https://img.shields.io/github/release/armink/EasyFlash.svg)](https://github.com/armink/EasyFlash/releases/latest) [![GitHub commits](https://img.shields.io/github/commits-since/armink/EasyFlash/4.1.0.svg)](https://github.com/armink/EasyFlash/compare/4.1.0...master) [![MIT licensed](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/armink/EasyFlash/master/LICENSE)

## 1、介绍

[EasyFlash](https://github.com/armink/EasyFlash)是一款开源的轻量级嵌入式Flash存储器库，方便开发者更加轻松的实现基于Flash存储器的常见应用开发。非常适合智能家居、可穿戴、工控、医疗、物联网等需要断电存储功能的产品，资源占用极低，支持各种 MCU 片上存储器。该库主要包括 **三大实用功能** ：

- **ENV** 快速保存产品参数，支持 **写平衡（磨损平衡）** 及 **掉电保护** 功能

EasyFlash不仅能够实现对产品的 **设定参数** 或 **运行日志** 等信息的掉电保存功能，还封装了简洁的 **增加、删除、修改及查询** 方法， 降低了开发者对产品参数的处理难度，也保证了产品在后期升级时拥有更好的扩展性。让Flash变为NoSQL（非关系型数据库）模型的小型键值（Key-Value）存储数据库。

- **IAP** 在线升级再也不是难事儿

该库封装了IAP(In-Application Programming)功能常用的接口，支持CRC32校验，同时支持Bootloader及Application的升级。

- **Log** 无需文件系统，日志可直接存储在Flash上

非常适合应用在小型的不带文件系统的产品中，方便开发人员快速定位、查找系统发生崩溃或死机的原因。同时配合[EasyLogger](https://github.com/armink/EasyLogger)(我开源的超轻量级、高性能C日志库，它提供与EasyFlash的无缝接口)一起使用，轻松实现C日志的Flash存储功能。

### 1.1、V4.0 NG 模式

自 2019 年春节后，EasyFlash 经过 4 年多的迭代，结合众多开发者的需求及建议，终于发布了 V4.0 版本，该版本中的 ENV 功能被命名为 **NG** (Next Generation) 模式，这是一个完全重构的新版本，具有以下新特性：

- 更小的资源占用，内存占用 **几乎为 0** ；（V4.0 以前版本会使用额外的 RAM 空间进行缓存）
- ENV 的值类型支持 **任意类型** 、任意长度，相当于直接 memcpy 变量至 flash ；（V4.0 之前只支持存储字符串）
- ENV 操作效率比以前的模式高，充分利用剩余空闲区域，擦除次数及操作时间显著降低；
- **原生支持** 磨损平衡、掉电保护功能 （V4.0 之前需要占用额外的 Flash 扇区）；
- ENV 支持 **增量升级** ，固件升级后 ENV 也支持升级；
- 支持大数据存储模式，**长度无限制**，数据可在多个 Flash 扇区上顺序存储。像脚本程序、音频等占用 Flash 超过 1 个扇区的资源也都可以存入 ENV（即将在 V4.2 支持）；
- 支持 **数据加密** ，提升存储的安全性，物联网时代的必备功能（即将在 V4.3 支持）；
- 支持 **数据压缩** ，减低 Flash 占用（即将在 V4.4 支持）；

V4.0 设计及内部原理，V4.0 迁移指南等更多内容请继续阅读下面的 [文档章节](#3文档) 

### 1.2、资源占用

```
最低要求： ROM: 6K bytes     RAM: 0.1K bytes
```

## 2、文档

- 基于 RT-Thread 的移植说明文档：[`ports/README.md`](ports/README.md)
- API 说明文档：[ `docs/zh/api.md`](docs/zh/api.md)
- 通用移植文档：[ `docs/zh/port.md`](docs/zh/port.md)
- V4.0 迁移指南：[`\docs\zh\v4_migrate.md`](/docs/zh/v4_migrate.md)
- V4.0 ENV 功能设计与实现：[`\docs\zh\design.md`](/docs/zh/design.md)

务必保证在 **阅读文档** 后再移植使用。

## 3、配置说明

```shell
[*]   ENV: Environment variables                                                     
[*]     Auto update ENV to latest default when current ENV version number is changed.
(0)       Setting current ENV version number                                         
[*]   LOG: Save logs on flash                                                        
(262144) Saved log area size. MUST be aligned by erase minimum granularity           
[*]   IAP: In Application Programming                                                
(4096) Erase minimum granularity                                                     
      Write minimum granularity (1bit   such as Nor Flash)  --->                     
(0)   Start addr on flash or partition                                               
[*]   Enable debug log output     
```

- `ENV: Environment variables`： 是否使能环境变量功能
  - `Auto update ENV to latest default when current ENV version number is changed.`：是否启用环境变量自动更新功能。启动这个功能后，环境变量将在其版本号发生变化时自动更新。
    - `Setting current ENV version number` : 当前环境变量版本号
- `LOG: Save logs on flash`：日志功能，可以将日志顺序保存至 Flash 中。还可以配合 EasyLogger 完成产品日志的掉电存储。
- `IAP: In Application Programming`：IAP 在线升级功能，开启后将提供一些 IAP 功能里常用的 API 。
- `Erase minimum granularity`：擦除的最小粒度，一般 SPI Flash 通常为 4KB，STM32F4 片内 Flash 通常为 128KB。
- `Write minimum granularity`：写数据的最小粒度，一般 SPI Flash 通常为 1bit，STM32F4 片内 Flash 通常为 8bit，详见具体选项。
- `Start addr on flash or partition`：EasyFlash 的整个存储区相对于 Flash 或者 分区 的偏移地址，视移植代码而定。
- `Enable debug log output`：是否使能调试日志输出。开启后将会看到更多调试日志信息。

## 4、支持

 ![support](/docs/zh/images/wechat_support.png)

如果 EasyFlash 解决了你的问题，不妨扫描上面二维码请我 **喝杯咖啡**~ 