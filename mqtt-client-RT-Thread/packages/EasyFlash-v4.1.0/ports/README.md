# 基于 RT-Thread 的 EasyFlash 移植参考示例

## 1、简介

本目录下主要存放了基于 RT-Thread 的移植参考示例。主要基于以下两种底层 Flash 驱动

- [fal : Flash 抽象层](https://github.com/RT-Thread-packages/fal)
- [SFUD : 万能 SPI Flash 驱动库](https://github.com/RT-Thread/rt-thread/tree/master/components/drivers/spi/sfud)

如果你的 Flash 驱动使用的是上面中的一种，那么移植 EasyFlash 将会非常简单，具体参考下面的文档。如果没有使用上面的驱动，请参考 EasyFlash 的 [移植文档](https://github.com/armink/EasyFlash/blob/master/docs/zh/port.md) 进行移植。在 EasyFlash [官方仓库](https://github.com/armink/EasyFlash) 下有很多 demo，也可以参考。

## 2、基于 fal : Flash 抽象层

移植参考文件位于 [/ports/ef_fal_port.c](ef_fal_port.c) 。先将该文件拷贝至项目中，在文件顶部有如下宏定义

```c
/* EasyFlash partition name on FAL partition table */
#define FAL_EF_PART_NAME               "ef"
```

再修改该宏定义为 EasyFlash 备份区位于 fal 分区表中的分区名即可，默认是 "ef"。

> 注意：在使用 fal 时，由于上面指定好了会使用整个 `"ef"` 分区给 EasyFlash，同时 `ef_fal_port.c` 中的移植也是基于分区读写，所以配置的 EasyFlash 起始地址应从 **0** 地址开始，即`PKG_EASYFLASH_START_ADDR` 配置应为 0。

## 3、基于 SFUD : 万能 SPI Flash 驱动库

移植参考文件位于 [/ports/ef_sfud_port.c](ef_sfud_port.c) 。先将该文件拷贝至项目中，在文件中的 `ef_port_init` 函数中有关于 SFUD Flash 设备的获取的相关的代码，如下所示

```c
static const sfud_flash *flash;

EfErrCode ef_port_init(ef_env const **default_env, size_t *default_env_size) {
    EfErrCode result = EF_NO_ERR;

    ...

    /* 从 RT-Thread 的 SPI Flash 设备中获取 SFUD Flash 设备对象 */
    extern rt_spi_flash_device_t w25q64;
    flash = (sfud_flash_t)(w25q64->user_data);

    return result;
}
```

上述代码中的 `w25q64` ，是在 Flash 设备初始化时，执行 `w25q64 = rt_sfud_flash_probe("w25q64", "spi10");` 成功的返回值。该对象是 RT-Thread 的 SPI Flash 对象，其内部元素 `user_data` 里存放了 SFUD Flash 对象，将其赋值给静态的 `flash` 变量即可。