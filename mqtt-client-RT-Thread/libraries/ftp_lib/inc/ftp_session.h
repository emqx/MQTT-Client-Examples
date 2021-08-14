#ifndef __FTP_SESSION_H
#define __FTP_SESSION_H
#include <rtthread.h>
#include <rtdevice.h>
#include <rthw.h>

enum ftp_session_state
{
    FTP_SESSION_STATE_USER = 0,
    FTP_SESSION_STATE_PASSWD,
    FTP_SESSION_STATE_PROCESS
};

struct ftp_session
{
    int fd;
    int port_pasv_fd;
    int is_anonymous;
    int offset;
    struct sockaddr_in remote;
    enum ftp_session_state state;
    char currentdir[256];
    rt_uint8_t force_quit;
    rt_tick_t tick_timeout;
    rt_slist_t slist;
};


int ftp_session_create(int fd, struct sockaddr_in *addr, socklen_t addr_len);
int ftp_session_force_quit(void);

#endif
