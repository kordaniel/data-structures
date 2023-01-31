#include "ThreadPool.hpp"


ThreadPool::ThreadPool(size_t numThreads, bool startThreads)
{
    if (s_objectInstantiated) {
        throw std::runtime_error("Attempted to instantiate a second ThreadPool object");
    }
    s_objectInstantiated = true;

    if (startThreads && !ThreadPool::IsStarted()) {
        ThreadPool::Start(numThreads);
    }
}

ThreadPool::~ThreadPool()
{
    ThreadPool::Stop();
    s_objectInstantiated = false;
}

void
ThreadPool::Start(size_t numThreads)
{ // static function
    {
        std::unique_lock<std::mutex> lock(s_queueMutex);
        if (!s_terminate) {
            throw std::runtime_error("Attempted to start an already started ThreadPool");
        }
        s_terminate = false;
        s_threadsCount = numThreads <= std::thread::hardware_concurrency()
                       ? numThreads
                       : std::thread::hardware_concurrency();
    }

    s_threads.reserve(s_threadsCount);
    s_threadsWaitingCount = 0;

    for (size_t i = 0; i < s_threadsCount; ++i) {
        s_threads.emplace_back(&ThreadPool::threadLoop);
    }
}

void
ThreadPool::Stop()
{ // static function
    {
        std::unique_lock<std::mutex> lock(s_queueMutex);
        s_terminate = true;
        s_threadsCount = 0;
    }
    s_queueMutexCondition.notify_all();

    for (auto& t : s_threads) {
        t.join();
    }

    s_threads.clear();
}

bool
ThreadPool::HasTasksQueued()
{ // static function
    bool jobsWaiting;
    {
        std::unique_lock<std::mutex> lock(s_queueMutex);
        jobsWaiting = !s_jobs.empty();
    }
    return jobsWaiting;
}

bool
ThreadPool::IsIdle()
{ // static function
    bool done;
    {
        std::unique_lock<std::mutex> lock(s_queueMutex);
        done = s_threadsWaitingCount == s_threadsCount && s_jobs.empty();
    }
    return done;
}

bool
ThreadPool::IsStarted()
{ // static function
    std::unique_lock<std::mutex> lock(s_queueMutex);
    return !s_terminate;
}

size_t
ThreadPool::GetThreadsCount()
{ // static function
    std::unique_lock<std::mutex> lock(s_queueMutex);
    return s_threadsCount;
}


/****************************************
 * ThreadPool Private methods
 ****************************************/
void
ThreadPool::threadLoop()
{ // static function
    while (true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(s_queueMutex);

            s_threadsWaitingCount++;

            s_queueMutexCondition.wait(lock, []() {
                // lock is released while waiting, automatically reaquired when returning from here
                return !s_jobs.empty() || s_terminate;
            });

            s_threadsWaitingCount--;

            if (s_terminate) {
                return;
            }

            task = s_jobs.front();
            s_jobs.pop();
        }

        task();
    }
}
