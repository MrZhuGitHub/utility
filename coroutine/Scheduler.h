#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <map>
#include <sys/epoll.h>

#define FD_SIZE 1024
#define MAX_EVENTS 1024
#define WAIT_DURATION 1

namespace Common
{
class Coroutine;

struct TimeEvent
{
    Coroutine* co;
    uint64_t timeout;
    TimeEvent* next;
};

struct IoEvent
{
    Coroutine* co;
    int fd;
    struct epoll_event event;
    IoEvent* next;
};

class Scheduler
{
public:
    Scheduler();

    void Eventloop();

    ~Scheduler();

    bool AddTimeEvent(TimeEvent* event);

    bool AddIoEvent(IoEvent* event);

    Coroutine* GetCurrentCoroutine()
    {
        return currentCoroutine_;
    }

    Coroutine* GetMainCoroutine()
    {
        return mainCoroutine_;
    }

    void Resume(Coroutine* co);

    friend class Coroutine;

private:
    template<typename T>
    T* RemoveItem(T* head, T* item)
    {
        if (nullptr == head || nullptr == item)
        {
            return nullptr;
        }
        if (head == item) {
            head = item->next;
            return head;
        }
        T* preItem = head;
        T* nextItem = head->next;
        while (nextItem != item && nextItem != nullptr)
        {
            preItem = preItem->next;
            nextItem = nextItem->next;
        }
        if (nextItem == item)
        {
            preItem = nextItem->next;
        }
        return head;
    }

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