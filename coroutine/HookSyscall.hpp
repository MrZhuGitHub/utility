#ifndef _HOOK_SYSCALL_H_
#define _HOOK_SYSCALL_H_

#include "Scheduler.h"
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>

namespace Common
{
inline void coSleep(unsigned int milliseconds)
{
    TimeEvent event;
    event.next = nullptr;
    event.co = scheduler.GetCurrentCoroutine();
    timeval val;
    gettimeofday(&val, nullptr);
    event.timeout = val.tv_sec*1000 + val.tv_usec/1000;
    scheduler.AddTimeEvent(&event);
    scheduler.Resume(scheduler.GetMainCoroutine());
}

inline int coRead(int fd, char* str, unsigned int len)
{
    int flags = fcntl(fd, F_GETFL);
    flags = (flags|O_NONBLOCK);
    fcntl(fd, F_SETFL, flags);
    int ret = read(fd, str, len);
    if (0 != ret) {
        return ret;
    }
    IoEvent event;
    event.next = nullptr;
    event.co = scheduler.GetCurrentCoroutine();
    event.fd = fd;
    event.event = (EPOLLIN|EPOLLERR);
    scheduler.AddIoEvent(&event);
    scheduler.Resume(scheduler.GetMainCoroutine());
    return read(fd, str, len);
}

inline int coWrite(int fd, char* str, unsigned int len)
{
    int flags = fcntl(fd, F_GETFL);
    flags = (flags|O_NONBLOCK);
    fcntl(fd, F_SETFL, flags);
    int ret = write(fd, str, len);
    if (0 != ret) {
        return ret;
    }
    IoEvent event;
    event.next = nullptr;
    event.co = scheduler.GetCurrentCoroutine();
    event.fd = fd;
    event.event = (EPOLLOUT|EPOLLERR);
    scheduler.AddIoEvent(&event);
    scheduler.Resume(scheduler.GetMainCoroutine());
    return write(fd, str, len);
}
}

#endif
