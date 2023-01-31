#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>
#include <queue>


class ThreadPool
{
public:
    using Task = std::function<void()>;

    ThreadPool(size_t numWThreads, bool startThreads = true);
    ~ThreadPool();

    void Start();
    void Stop();

    template<typename T> auto
    QueueTask(T task) const -> std::future<decltype(task())>
    {
        auto wrapper = std::make_shared<std::packaged_task<decltype(task()) ()>>(std::move(task));
        bool isStarted;
        {
            std::unique_lock<std::mutex> lock(_queueMutex);
            isStarted = !_terminate;
            _jobs.emplace([=] { (*wrapper)(); });
        }

        if (!isStarted) {
            throw std::runtime_error("ThreadPool that is not started received jobs");
        }

        _queueMutexCondition.notify_one();

        return wrapper->get_future();
    }

    bool HasTasksQueued()    const;
    bool IsIdle()            const;
    size_t GetThreadsCount() const;

private:
    void threadLoop();

    bool                            _terminate;
    const size_t                    _threadsCount;
    size_t                          _threadsWaitingCount;
    std::vector<std::thread>        _threads;
    mutable std::mutex              _queueMutex;
    mutable std::condition_variable _queueMutexCondition;
    mutable std::queue<Task>        _jobs;

};

#endif // THREADPOOL_HPP
