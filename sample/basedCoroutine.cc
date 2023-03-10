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
    std::cout << "step1" << std::endl;
    Utility::Coroutine continue1(Func1, std::string("abc"), 1);
    continue1.Start();
    Utility::Coroutine continue2(Func2);
    continue2.Start();
    std::cout << "step4" << std::endl;
    Utility::Coroutine::EventLoop();
    std::cout << "step7" << std::endl;
};
