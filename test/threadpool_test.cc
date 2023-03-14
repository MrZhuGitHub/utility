#include <gtest/gtest.h>
#include "threadpool.hpp"
#include <atomic>
#include <iostream>

class threadpoolTest : public testing::Test
{
protected:
    virtual void SetUp() override
    {
        threadpool_ = std::make_shared<Utility::threadpool>(2);
    }

    virtual void TearDown()
    {

    }

    std::shared_ptr<Utility::threadpool> threadpool_;
};

std::atomic<int> count;

int base(const int& repeat)
{
    std::cout << "thread id = " << std::this_thread::get_id() << std::endl;
    for (int i=0; i < repeat; i++)
    {
        count++;
    }
    return 0;    
}

TEST_F(threadpoolTest, base_true)
{
    count.store(0);
    int repeat = 100000;
    std::future<int> result1 = threadpool_->push(base, repeat);
    std::future<int> result2 = threadpool_->push(base, repeat);
    std::future<int> result3 = threadpool_->push(base, repeat);
    result1.wait();
    result2.wait();
    result3.wait();
    int result = count.load();
    EXPECT_EQ(result, 300000);
}

TEST_F(threadpoolTest, base_false)
{
    count.store(0);
    int repeat = 100000;
    threadpool_->push(base, repeat);
    threadpool_->push(base, repeat);
    threadpool_->push(base, repeat);
    int result = count.load();
    EXPECT_NE(result, 300000);
}

int getfuture(int& a, int& b)
{
    return (a*b);
}

TEST_F(threadpoolTest, getfuture)
{
    int a=10, b=30;
    std::future<int> result = threadpool_->push(getfuture, a, b);
    result.wait();
    EXPECT_EQ(result.get(), 300);
}

