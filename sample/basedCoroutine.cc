#include "Coroutine.h"
#include "Scheduler.h"
#include "HookSyscall.hpp"
#include <string>
#include <string.h>
#include <iostream>

void Func1(std::string str, int num)
{
    std::cout << "step2" << std::endl;
    std::cout << str << num << std::endl;
    Utility::coSleep(10000);
    std::cout << "step6" << std::endl;
}

void Func2()
{
    std::cout << "step3" << std::endl;
    Utility::coSleep(5000);
    std::cout << "step5" << std::endl;
}

int main() 
{
    auto scheduler = Utility::Scheduler::GetCurrentScheduler();
    std::cout << "step1" << std::endl;
    auto coroutine1 = std::make_shared<Utility::Coroutine>(Func1, std::string("abc"), 1);
    coroutine1->Start();
    auto coroutine2 = std::make_shared<Utility::Coroutine>(Func2);
    coroutine2->Start();
    std::cout << "step4" << std::endl;
    scheduler->Eventloop();
    std::cout << "step7" << std::endl;
};
