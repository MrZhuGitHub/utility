#include "Coroutine.h"
#include "Scheduler.h"
#include "HookSyscall.hpp"
#include <string>
#include <string.h>
#include <iostream>

void Func1(std::string str, int num)
{
    std::cout << str << num << std::endl;
    Common::coSleep(10000);
    std::cout << "111" << std::endl;
}

void Func2()
{
    std::cout << "input nothing" << std::endl;
}

int main() 
{
    Common::Coroutine continue1(Func1, std::string("abc"), 1);
    continue1.Start();
    Common::Coroutine continue2(Func2);
    continue2.Start();
    Common::Coroutine::EventLoop();
};
