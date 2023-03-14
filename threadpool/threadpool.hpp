#include <thread>
#include <functional>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <vector>
#include <future>

namespace Utility
{
class threadpool
{
public:
    threadpool(int threadsize=10)
        : size_(threadsize)
        , state_(true)
    {
        size_ = threadsize;
        for (int i=0; i<size_; i++)
        {
            auto ptr = std::make_shared<std::thread>(&threadpool::loop, this);
            threads_.push_back(ptr);
        }
    }

    template <typename Func, typename... Args>
    auto push(Func&& func, Args&&... args) -> std::future<decltype(func(args...))>
    {
        using retType = decltype(func(args...));        
        auto task = std::make_shared<std::packaged_task<retType(void)>>(std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
        std::function<void()> packagedFunc = [=]()->void{ (*task)(); };
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(packagedFunc);
        condition_.notify_one();
        return task->get_future();
    }

    ~threadpool()
    {
        state_=false;
        condition_.notify_all();
        for (auto& it : threads_)
        {
            it->join();
        }
    }

private:

    void loop()
    {
        while(1)
        {
            std::unique_lock<std::mutex> lock(mutex_);
            while(queue_.empty() && state_)
            {
                condition_.wait(lock);
            }
            if (!state_) {
                break;
            }
            auto task = queue_.front();
            queue_.pop();
            task();
        }
    }


private:
    int size_;
    bool state_;
    std::mutex mutex_;
    std::condition_variable condition_;
    std::vector<std::shared_ptr<std::thread>> threads_;
    std::queue<std::function<void()>> queue_;
};
}