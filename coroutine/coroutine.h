#ifndef _COROUTINE_H_
#define _COROUTINE_H_
#include <functional>

namespace MyCoroutine {

class MyCoroutine {
public:
    MyCoroutine(void* Func);

    template <typename ...args>
    void Start()
    {

    }

    void Resume();
    ~MyCoroutine();

private:
    bool InitStack();

private:
    void* Func_;
};

}
#endif