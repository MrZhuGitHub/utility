#ifndef _RING_BUFFER_
#define _RING_BUFFER_

namespace Common {

template<typename T>
class Channel
{
public:
    Channel()
    {

    }

    ~Channel()
    {

    }

    bool Pop(T& item)
    {

    }

    bool Push(T& item)
    {

    }

private:
    T* head_;
};

}

#endif