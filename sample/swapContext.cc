/**********************************************************************
 * regs[0]   %rax	返回值
 * regs[1]   %rbx	被调用者保存
 * regs[2]   %rcx	第4个参数
 * regs[3]   %rdx	第3个参数
 * regs[4]   %rsi	第2个参数
 * regs[5]   %rdi	第1个参数
 * regs[6]   %rbp	被调用者保存
 * regs[7]   %rsp	栈指针
 * regs[8]   %r8	第5个参数
 * regs[9]   %r9	第6个参数
 * regs[10]  %r10	调用者保存
 * regs[11]  %r11	调用者保存
 * regs[12]  %r12	被调用者保存
 * regs[13]  %r13	被调用者保存
 * regs[14]  %r14	被调用者保存
 * regs[15]  %r15	被调用者保存
 * 
**********************************************************************/
#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>

struct coroutineState {
    char* regs[16];
    char* stack;
    void* func;
    void resume(coroutineState* co) {
        // std::cout << "1" << std::endl;
        __asm__ __volatile__ (
            "movq %rax,(%rdi)\n"
            "movq %rbx,8(%rdi)\n"
            "movq %rcx,16(%rdi)\n"
            "movq %rdx,24(%rdi)\n"
            "movq %rsi,32(%rdi)\n"
            "movq %rbp,48(%rdi)\n"
            "movq %rsp,56(%rdi)\n"
            "movq %r8,64(%rdi)\n"
            "movq %r9,72(%rdi)\n"
            "movq %r10,80(%rdi)\n"
            "movq %r11,88(%rdi)\n"
            "movq %r12,96(%rdi)\n"
            "movq %r13,104(%rdi)\n"
            "movq %r14,112(%rdi)\n"
            "movq %r15,120(%rdi)\n"
            "movq %rdi,40(%rdi)\n"
        );
        // std::cout << "2" << std::endl;
        __asm__ __volatile__ (
            "movq (%rsi),%rax\n"
            "movq 8(%rsi),%rbx\n"
            "movq 16(%rsi),%rcx\n"
            "movq 24(%rsi),%rdx\n"
            "movq 40(%rsi),%rdi\n"
            "movq 48(%rsi),%rbp\n"
            "movq 56(%rsi),%rsp\n"
            "movq 64(%rsi),%r8\n"
            "movq 72(%rsi),%r9\n"
            "movq 80(%rsi),%r10\n"
            "movq 88(%rsi),%r11\n"
            "movq 96(%rsi),%r12\n"
            "movq 104(%rsi),%r13\n"
            "movq 112(%rsi),%r14\n"
            "movq 120(%rsi),%r15\n"
            "movq 32(%rsi),%rsi\n"
        );
        // std::cout << "3" << std::endl;
    }
    void InitCoroutineState() {
        stack = (char*)malloc(1024);
        stack = stack + 1000;
        void** eip = (void**)(stack - 24);
        (*eip) = func;
        regs[6] = (char*)(stack - 32);
        regs[7] = (char*)(stack - 32);
        char** ebp = (char**)(stack - 32);
        (*ebp) = stack;
    }
};

coroutineState mainContext;
coroutineState func1Context1, func1Context2;

void func1(std::string str) {
    std::cout << "step2" << std::endl;
    func1Context1.resume(&mainContext);
    std::cout << "step5" << std::endl;
    func1Context1.resume(&mainContext);
}

void func2(std::string str) {
    std::cout << "step4" << std::endl;
    func1Context2.resume(&func1Context1);
    std::cout << "step7" << std::endl;
    func1Context2.resume(&mainContext);
}

int main() {
    func1Context1.func = (void*)&func1;
    func1Context2.func = (void*)&func2;
    func1Context1.InitCoroutineState();
    func1Context2.InitCoroutineState();
    std::cout << "step1" << std::endl;
    mainContext.resume(&func1Context1);
    std::cout << "step3" << std::endl;
    mainContext.resume(&func1Context2);
    std::cout << "step6" << std::endl;
    mainContext.resume(&func1Context2);
    std::cout << "step8" << std::endl;
    return 0;
}