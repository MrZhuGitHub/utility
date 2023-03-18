#ifndef _HOOK_SYSCALL_H_
#define _HOOK_SYSCALL_H_

#include "Scheduler.h"
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>
#include <iostream>
#include <memory>

namespace Utility
{
inline void coSleep(unsigned int milliseconds)
{
    auto event = std::make_shared<TimeEvent>();
    auto scheduler = Scheduler::GetCurrentScheduler();
    event->suspendCoroutine = scheduler->GetCurrentCoroutine();
    timeval val;
    gettimeofday(&val, nullptr);
    event->timeout = val.tv_sec*1000 + val.tv_usec/1000 + milliseconds;
    scheduler->SwitchCoroutine(event);
}

inline int setNoBlockingIO(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    flags = (flags|O_NONBLOCK);
    return fcntl(fd, F_SETFL, flags);    
}

inline int coRead(int fd, char* str, unsigned int len)
{
    setNoBlockingIO(fd);
    auto event = std::make_shared<IoEvent>();
    auto scheduler = Scheduler::GetCurrentScheduler();
    event->suspendCoroutine = scheduler->GetCurrentCoroutine();
    event->fd = fd;
    event->event.events = (EPOLLIN|EPOLLERR);
    event->event.data.fd = fd;
    scheduler->SwitchCoroutine(event);
    return read(fd, str, len);
}

inline int coWrite(int fd, const char* str, unsigned int len)
{
    setNoBlockingIO(fd);
    auto event = std::make_shared<IoEvent>();
    auto scheduler = Scheduler::GetCurrentScheduler();
    event->suspendCoroutine = scheduler->GetCurrentCoroutine();
    event->fd = fd;
    event->event.events = (EPOLLOUT|EPOLLERR);
    event->event.data.fd = fd;
    scheduler->SwitchCoroutine(event);
    return write(fd, str, len);
}

inline int coAccept(int fd, sockaddr *addr, socklen_t *addrlen)
{
    setNoBlockingIO(fd);
    auto event = std::make_shared<IoEvent>();
    auto scheduler = Scheduler::GetCurrentScheduler();
    event->suspendCoroutine = scheduler->GetCurrentCoroutine();
    event->fd = fd;
    event->event.events = EPOLLIN;
    event->event.data.fd = fd;
    scheduler->SwitchCoroutine(event);
    return accept(fd, addr, addrlen);
}

inline int coConnect(int fd, sockaddr* addr, socklen_t len)
{
    setNoBlockingIO(fd);
    while (1)
    {
        int ret = connect(fd, addr, len);
        if (0 == ret)
        {
            return ret;
        }
        if (-1 == ret)
        {
            if (EINTR == errno)
            {
                continue;
            }
            if (EINPROGRESS != errno) {
                return ret;
            } else {
                auto event = std::make_shared<IoEvent>();
                auto scheduler = Scheduler::GetCurrentScheduler();
                event->suspendCoroutine = scheduler->GetCurrentCoroutine();
                event->fd = fd;
                event->event.events = (EPOLLIN|EPOLLOUT);
                event->event.data.fd = fd;
                scheduler->SwitchCoroutine(event);

                int32_t temperr = 0;
                socklen_t temperrlen = sizeof(temperr);
                int ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, (void*)&temperr, &temperrlen);
                if(ret >= 0)
                {
                    return 0;
                } else {
                    return -1;
                }
            }
        }
    }
}

}

#endif
