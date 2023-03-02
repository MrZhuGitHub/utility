#include "Coroutine.h"
#include "Scheduler.h"
#include "HookSyscall.h"
#include <string>
#include <string.h>
#include <iostream>

void Func1(std::string str, int num)
{
    std::cout << str << num << std::endl;
    Common::sleep(10000);
}

void Func2()
{
    std::cout << "input nothing" << std::endl;
}

int main() 
{
    Common::Coroutine continue1((void*)&Func1, std::string("abc"), 1);
    continue1.Start();
    Common::Coroutine continue2((void*)&Func2);
    continue2.Start();
    scheduler.Eventloop();
};
