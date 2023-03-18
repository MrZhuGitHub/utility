#include "Scheduler.h"
#include "Coroutine.h"
#include <sys/time.h>
#include <iostream>

namespace Utility
{

std::unordered_map<decltype(std::this_thread::get_id()), std::shared_ptr<Scheduler>> Scheduler::kSchedulers;

std::shared_ptr<Scheduler> Scheduler::GetCurrentScheduler()
{
    std::thread::id currentThreadId = std::this_thread::get_id();
    auto it = kSchedulers.find(currentThreadId);
    if (it == kSchedulers.end())
    {
        std::shared_ptr<Scheduler> ptr =std::make_shared<Scheduler>();
        kSchedulers.insert(std::pair<std::thread::id, std::shared_ptr<Scheduler>>(currentThreadId, ptr));
        return ptr;
    } else {
        return it->second;
    }
}

Scheduler::Scheduler()
    : mainCoroutine_(std::make_shared<Coroutine>()),
      currentCoroutine_(mainCoroutine_)
{
    epfd_ = epoll_create(FD_SIZE);
}

void Scheduler::SwitchCoroutine(std::shared_ptr<Coroutine> co)
{
    char* currentStackInfo = (char*)(&currentCoroutine_->regs_);
    std::string output1;
    for (uint64_t i = 0; i < currentCoroutine_->getCoroutineId(); i++)
    {
        output1.append("    ");
    }
    std::cout << output1 << currentCoroutine_->getCoroutineId() << std::endl;

    char* nextStackInfo = (char*)(&co->regs_);
    std::string output2;
    for (uint64_t i = 0; i < co->getCoroutineId(); i++)
    {
        output2.append("    ");
    }
    std::cout << output2 << co->getCoroutineId() << std::endl;

    currentCoroutine_ = co;
    SwapContext(currentStackInfo, nextStackInfo);
}

void Scheduler::Eventloop()
{
    while(true) {

        doIoEvents_.insert(addIoEvents_.begin(), addIoEvents_.end());
        for (auto& it : addIoEvents_)
        {
            epoll_ctl(epfd_, EPOLL_CTL_ADD, it.second->fd, &it.second->event);
        }
        addIoEvents_.clear();

        doTimeEvents_.insert(doTimeEvents_.end(), addTimeEvents_.begin(), addTimeEvents_.end());
        addTimeEvents_.clear();

        doSwitchCoroutines_.insert(doSwitchCoroutines_.begin(), addSwitchCoroutines_.begin(), addSwitchCoroutines_.end());
        addSwitchCoroutines_.clear();

        int eventCount = epoll_wait(epfd_, events_, MAX_EVENTS, WAIT_DURATION);
        for (auto it = doTimeEvents_.begin(); it != doTimeEvents_.end();)
        {
            timeval val;
            gettimeofday(&val, nullptr);
            uint64_t timeout = val.tv_sec*1000 + val.tv_usec/1000;
            if (timeout >= (*it)->timeout)
            {
                this->SwitchCoroutine((*it)->suspendCoroutine);
                it = doTimeEvents_.erase(it);
            } else {
                it++;
            }
        }

        for (auto it = doSwitchCoroutines_.begin(); it != doSwitchCoroutines_.end();)
        {
            this->SwitchCoroutine((*it)->suspendCoroutine);
            it = doSwitchCoroutines_.erase(it);
        }

        for (int i = 0; i < eventCount; i++)
        {
            int fd = events_[i].data.fd;
            std::unordered_map<int, std::shared_ptr<IoEvent>>::iterator it = doIoEvents_.find(fd);
            if (it != doIoEvents_.end())
            {
                this->SwitchCoroutine(it->second->suspendCoroutine);
                epoll_ctl(epfd_, EPOLL_CTL_DEL, it->second->fd, &it->second->event);
                doIoEvents_.erase(it);
            }
        }
    }
}

void Scheduler::SwapContext(char* currentStackInfo, char* nextStackInfo)
{
#ifdef __x86_64__
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
#endif
}

Scheduler::~Scheduler()
{
    std::thread::id currentThreadId = std::this_thread::get_id();
    auto it = kSchedulers.find(currentThreadId);
    if (it != kSchedulers.end())
    {
        kSchedulers.erase(it);
    }
}
}