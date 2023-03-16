#ifndef _HOOK_SYSCALL_H_
#define _HOOK_SYSCALL_H_

#include "Scheduler.h"
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <socket.h>

namespace Utility
{
inline void coSleep(unsigned int milliseconds)
{
    TimeEvent* event = new TimeEvent();
    event->next = nullptr;
    event->co = scheduler.GetCurrentCoroutine();
    timeval val;
    gettimeofday(&val, nullptr);
    event->timeout = val.tv_sec*1000 + val.tv_usec/1000 + milliseconds;
    scheduler.AddTimeEvent(event);
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
    IoEvent* event = new IoEvent();
    event->next = nullptr;
    event->co = scheduler.GetCurrentCoroutine();
    event->fd = fd;
    event->event.events = (EPOLLIN|EPOLLERR);
    scheduler.AddIoEvent(event);
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
    IoEvent* event = new IoEvent();
    event->next = nullptr;
    event->co = scheduler.GetCurrentCoroutine();
    event->fd = fd;
    event->event.events = (EPOLLOUT|EPOLLERR);
    scheduler.AddIoEvent(event);
    scheduler.Resume(scheduler.GetMainCoroutine());
    return write(fd, str, len);
}

inline int coAccept(int sockfd, sockaddr *addr, socklen_t *addrlen)
{
    int flags = fcntl(sockfd, F_GETFL);
    flags = (flags|O_NONBLOCK);
    fcntl(fd, F_SETFL, flags);
    IoEvent* event = new IoEvent();
    event->next = nullptr;
    event->fd = fd;
    event->co = scheduler.GetCurrentCoroutine();
    event->event = EPOLLIN;
    scheduler.AddIoEvent(event);
    scheduler.Resume(scheduler.GetMainCoroutine());
    return accept(sockfd, addr, addrlen);
}

inline int coConnect(int fd, sockaddr* addr, socklen_t len)
{
    int flags = fcntl(fd, F_GETFL);
    flags = (flags|O_NONBLOCK);
    fcntl(fd, F_SETFL, flags);
    while (1)
    {
        int ret = connect(fd, addr, len);
        if (0 == ret)
        {
            return ret;
        }
        if (-1 == ret)
        {
            if (EINTR == ret)
            {
                continue;
            } else if (EINPROGRESS != errno) {
                return ret;
            } else {
                IoEvent* event = new IoEvent();
                event->next = nullptr;
                event->fd = fd;
                event->co = scheduler.GetCurrentCoroutine();
                event->event = EPOLLIN;
                scheduler.AddIoEvent(event);
                scheduler.Resume(scheduler.GetMainCoroutine());
                if (event->ret & EPOLLERR)
                {
                    return -1;
                } else {
                    return 0;
                }
            }
        }
    }
}

}

#endif
