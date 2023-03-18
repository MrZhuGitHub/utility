#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <map>
#include <sys/epoll.h>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <thread>
#include <memory>

#define FD_SIZE 1024
#define MAX_EVENTS 1024
#define WAIT_DURATION 1

namespace Utility
{
class Coroutine;

struct SwitchEvent
{
    std::shared_ptr<Coroutine> suspendCoroutine;
    std::shared_ptr<Coroutine> resumeCoroutine;
};

struct TimeEvent
{
    std::shared_ptr<Coroutine> suspendCoroutine;
    uint64_t timeout;
};

struct IoEvent
{
    std::shared_ptr<Coroutine> suspendCoroutine;
    int fd;
    struct epoll_event event;
    uint32_t ret;
};

class Scheduler
{
public:
    Scheduler();

    void Eventloop();

    ~Scheduler();

    void SwitchCoroutine(std::shared_ptr<TimeEvent> event)
    {
        if (!event)
        {
            std::cout << "Error: time event is nullptr" << std::endl;
            return;
        }
        addTimeEvents_.push_back(event);
        this->SwitchCoroutine(mainCoroutine_);
    }

    void SwitchCoroutine(std::shared_ptr<IoEvent> event)
    {
        if (!event)
        {
            std::cout << "Error: io event is nullptr" << std::endl;
            return;
        }
        addIoEvents_.insert(std::pair<int, std::shared_ptr<IoEvent>>(event->fd, event));
        this->SwitchCoroutine(mainCoroutine_);
    }

    void SwitchCoroutine(std::shared_ptr<SwitchEvent> event)
    {
        if (!event)
        {
            std::cout << "Error: switch event is nullptr" << std::endl;
            return;
        }
        if (event->suspendCoroutine != mainCoroutine_ && event->resumeCoroutine != mainCoroutine_)
        {
            // std::cout << "Info : from coroutine = " << event->suspendCoroutine->getCoroutineId();
            // std::cout << "start coroutine = " << event->resumeCoroutine->getCoroutineId() << std::endl;
            addSwitchCoroutines_.push_back(event);
        } else if (event->resumeCoroutine == mainCoroutine_) {
            // std::cout << "Info : end coroutine = " << event->suspendCoroutine->getCoroutineId() << std::endl;
        } else {
            // std::cout << "Info : from coroutine = " << event->suspendCoroutine->getCoroutineId();
            // std::cout << "start coroutine = " << event->resumeCoroutine->getCoroutineId() << std::endl;
        }
        this->SwitchCoroutine(event->resumeCoroutine);
    }

    static std::shared_ptr<Scheduler> GetCurrentScheduler();

    std::shared_ptr<Coroutine> GetCurrentCoroutine()
    {
        return currentCoroutine_;
    }

    std::shared_ptr<Coroutine> GetMainCoroutine()
    {
        return mainCoroutine_;
    }

private:
    void SwitchCoroutine(std::shared_ptr<Coroutine> co);

    void SwapContext(char* currentStackInfo, char* nextStackInfo);

private:
    static std::unordered_map<decltype(std::this_thread::get_id()), std::shared_ptr<Scheduler>> kSchedulers;

private:
    int epfd_;
    epoll_event events_[MAX_EVENTS];
    std::shared_ptr<Coroutine> mainCoroutine_;
    std::shared_ptr<Coroutine> currentCoroutine_;
    std::vector<std::shared_ptr<SwitchEvent>> doSwitchCoroutines_;
    std::vector<std::shared_ptr<TimeEvent>> doTimeEvents_;
    std::unordered_map<int, std::shared_ptr<IoEvent>> doIoEvents_;
    std::vector<std::shared_ptr<SwitchEvent>> addSwitchCoroutines_;    
    std::vector<std::shared_ptr<TimeEvent>> addTimeEvents_;    
    std::unordered_map<int, std::shared_ptr<IoEvent>> addIoEvents_;
};

}

#endif