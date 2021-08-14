# 如何使用

## 注意事项

- 目前只提供了 `gcc` 的静态库
- 推荐使用 `RT-Thread Studio` 开发

## 快速上手

1. `#include "ftp.h"`
2. `ftp_init(2048, 27, 100);`
3. 默认端口 `21` ，默认用户名和密码都为 `loogg`

## API 介绍

1. `ftp_get_max_session_num`
    获取最大客户端数量

2. `ftp_set_max_session_num`
    设置最大客户端数量

3. `ftp_get_session_username`
    获取客户端用户名

4. `ftp_set_session_username`
    设置客户端用户名

5. `ftp_get_session_password`
    获取客户端密码

6. `ftp_set_session_password`
    设置客户端密码

7. `ftp_get_session_welcome_msg`
    获取客户端欢迎词

8. `ftp_set_session_welcome_msg`
    设置客户端欢迎词

9. `ftp_session_force_quit`
    强制关闭所有客户端连接(异步)

10. `ftp_force_restart`
    强制重启服务器(异步)

11. `ftp_get_port`
    获取服务器监听端口

12. `ftp_set_port`
    设置服务器监听端口

13. `ftp_init`
    初始化 ftp 服务

## 动态设置参数

- 使用 `ftp_force_restart` 和 `ftp_session_force_quit`

## 联系方式 & 感谢

- 维护：malongwei
- 主页：<https://github.com/loogg/ftp>
- 邮箱：<2544047213@qq.com>
