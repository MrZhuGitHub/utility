#include "Scheduler.h"
#include "Coroutine.h"
#include <sys/time.h>

namespace Common
{

Scheduler scheduler;

Scheduler::Scheduler()
    : mainCoroutine_(new Coroutine(nullptr)),
      coroutines_(mainCoroutine_),
      currentCoroutine_(mainCoroutine_),
      timeEvent_(nullptr),
      ioEvent_(nullptr)
{
    epfd_ = epoll_create(FD_SIZE);
}

void Scheduler::Resume(Coroutine* co)
{
    Coroutine* next = co;
    Coroutine* current = currentCoroutine_;
    Coroutine* ptr = coroutines_;
    while (ptr != nullptr)
    {
        if (ptr == co) {
            break;
        } else {
            ptr = ptr->next;
        }
    }
    if (ptr == nullptr) {
        coroutines_ = co;
    }
    currentCoroutine_ = co;
    SwapContext(current, next);
}

void Scheduler::RemoveCoroutine(Coroutine* co)
{
    coroutines_ = RemoveItem<Coroutine>(coroutines_, co);
}

bool Scheduler::AddTimeEvent(const TimeEvent* const event)
{
    if (nullptr == timeEvent_)
    {
        timeEvent_ = event;
    } else {
        while(nullptr != timeEvent_->next)
        {
            timeEvent_ = timeEvent_ ->next;
        }
        timeEvent_->next = event;
    }
    return true;
}

bool Scheduler::AddIoEvent(const IoEvent* const event)
{
    if (nullptr == ioEvent_)
    {
        ioEvent_ = event;
    } else {
        while(nullptr != ioEvent_->next)
        {
            ioEvent_ = ioEvent_ ->next;
        }
        ioEvent_->next = event;
    }
    int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, event->fd, event->event);
    if (0 != ret) {
        return false;
    }
    return true;
}

void Scheduler::Eventloop()
{
    while(1) {
        int eventCount = epoll_wait(epfd_, events_, MAX_EVENTS, WAIT_DURATION);
        TimeEvent* timeEvents = timeEvent_;
        for (nullptr != timeEvents)
        {
            timeval val;
            gettimeofday(&val, nullptr);
            uint64_t timeout = val.tv_sec*1000 + val.tv_usec/1000;
            if (timeout >= timeEvents->timeout)
            {
                Resume(timeEvents->co);
                timeEvent_ = RemoveItem<TimeEvent>(timeEvent_, timeEvents);
            } 
            timeEvents = timeEvents->next;
        }
        for (int i = 0; i < eventCount; i++)
        {
            int fd = events_[i].data.fd;
            IoEvent* ioEvents = ioEvent_;
            while(nullptr != ioEvents)
            {
                if (ioEvents->fd == fd)
                {
                    Resume(ioEvents->co);
                    ioEvent_ = RemoveItem<IoEvent>(ioEvent_, ioEvents);
                    break;
                }
            }
        }
    }
}

void Scheduler::SwapContext(Coroutine* current, Coroutine* next)
{
    __asm__ __volatile__ (
        "movq %rax,(%rsi)\n"
        "movq %rbx,8(%rsi)\n"
        "movq %rcx,16(%rsi)\n"
        "movq %rdx,24(%rsi)\n"
        "movq %rdi,40(%rsi)\n"
        "movq %rbp,48(%rsi)\n"
        "movq %rsp,56(%rsi)\n"
        "movq %r8,64(%rsi)\n"
        "movq %r9,72(%rsi)\n"
        "movq %r10,80(%rsi)\n"
        "movq %r11,88(%rsi)\n"
        "movq %r12,96(%rsi)\n"
        "movq %r13,104(%rsi)\n"
        "movq %r14,112(%rsi)\n"
        "movq %r15,120(%rsi)\n"
        "movq %rsi,32(%rsi)\n"
    );
    __asm__ __volatile__ (
        "movq (%rdx),%rax\n"
        "movq 8(%rdx),%rbx\n"
        "movq 16(%rdx),%rcx\n"
        "movq 32(%rdx),%rsi\n"
        "movq 40(%rdx),%rdi\n"
        "movq 48(%rdx),%rbp\n"
        "movq 56(%rdx),%rsp\n"
        "movq 64(%rdx),%r8\n"
        "movq 72(%rdx),%r9\n"
        "movq 80(%rdx),%r10\n"
        "movq 88(%rdx),%r11\n"
        "movq 96(%rdx),%r12\n"
        "movq 104(%rdx),%r13\n"
        "movq 112(%rdx),%r14\n"
        "movq 120(%rdx),%r15\n"
        "movq 24(%rdx),%rdx\n"
    );
}

Scheduler::~Scheduler()
{
    if (nullptr != mainCoroutine_)
    {
        delete mainCoroutine_;
    }
}
}