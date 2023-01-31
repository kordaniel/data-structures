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


/// @brief A class that holds worker threads and provides static functions for
///        using it. Also provides a constructor so that one and only one object
///        can be created, so that the class get stopped by the RAII principle.
///        The optional object should only be created in the main thread.
class ThreadPool
{
public:
    using Task = std::function<void()>;

    ThreadPool() = delete;
    ThreadPool(const ThreadPool& other) = delete;
    ThreadPool& operator=(const ThreadPool& other) = delete;
    ThreadPool(ThreadPool&& other) = delete;

    ThreadPool(size_t numWThreads, bool startThreads = true);
    ~ThreadPool();

    static void Start(size_t numThreads);
    static void Stop();

    template<typename T> inline static auto
    QueueTask(T task) -> std::future<decltype(task())>
    {
        auto wrapper = std::make_shared<std::packaged_task<decltype(task()) ()>>(std::move(task));
        bool isStarted;
        {
            std::unique_lock<std::mutex> lock(s_queueMutex);
            isStarted = !s_terminate;
            s_jobs.emplace([=] { (*wrapper)(); });
        }

        if (!isStarted) {
            throw std::runtime_error("ThreadPool that is not started received jobs");
        }

        s_queueMutexCondition.notify_one();

        return wrapper->get_future();
    }

    static bool   HasTasksQueued();
    static bool   IsIdle();
    static bool   IsStarted();
    static size_t GetThreadsCount() ;

private:
    static void threadLoop();

    inline static bool                     s_objectInstantiated = false;
    inline static bool                     s_terminate = true;
    inline static size_t                   s_threadsCount = 0;
    inline static size_t                   s_threadsWaitingCount = 0;
    inline static std::vector<std::thread> s_threads;
    inline static std::mutex               s_queueMutex;
    inline static std::condition_variable  s_queueMutexCondition;
    inline static std::queue<Task>         s_jobs;

};

#endif // THREADPOOL_HPP
