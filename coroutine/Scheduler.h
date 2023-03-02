#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <map>
#include <sys/epoll.h>

#define FD_SIZE 1024
#define MAX_EVENTS 1024
#define WAIT_DURATION 1

namespace MyCoroutine
{
class Coroutine;

struct TimeEvent
{
    Coroutine* co;
    uint64_t timeout;
    TimeEvent* next;
}

struct IoEvent
{
    Coroutine* co;
    int fd;
    struct epoll_event event;
    IoEvent* next;
}

class Scheduler
{
public:
    Scheduler();

    void Eventloop();

    ~Scheduler();

    bool AddTimeEvent(const TimeEvent* const event);

    bool AddIoEvent(const IoEvent* const event);

    Coroutine* GetCurrentCoroutine()
    {
        return currentCoroutine_;
    }

    Coroutine* GetMainCoroutine()
    {
        return mainCoroutine_;
    }

    friend class Coroutine;

private:
    void Resume(Coroutine* co);

    void RemoveCoroutine(Coroutine* co);

    void SwapContext(Coroutine* current, Coroutine* next);

private:
    Coroutine* mainCoroutine_;
    Coroutine* coroutines_;
    Coroutine* currentCoroutine_;
    int epfd_;
    TimeEvent* timeEvent_;
    IoEvent* ioEvent_;
    epoll_event events_[MAX_EVENTS];
};

extern Scheduler scheduler;
}

#endif