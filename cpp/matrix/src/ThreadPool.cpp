#include "ThreadPool.hpp"


ThreadPool::ThreadPool(size_t numThreads, bool startThreads)
    : _terminate(true)
    , _threadsCount(numThreads <= std::thread::hardware_concurrency()
        ? numThreads
        : std::thread::hardware_concurrency())
    , _threadsWaitingCount(0)
    , _threads()
    , _queueMutex()
    , _queueMutexCondition()
    , _jobs()
{
    if (startThreads) {
        Start();
    }
}

ThreadPool::~ThreadPool()
{
    Stop();
}

void
ThreadPool::Start()
{
    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        if (!_terminate) {
            throw std::runtime_error("Attempted to start an already started ThreadPool");
        }
    }

    _terminate = false;
    _threads.resize(_threadsCount);
    _threadsWaitingCount = 0;

    for (size_t i = 0; i < _threadsCount; ++i) {
        _threads.at(i) = std::thread(&ThreadPool::threadLoop, this);
    }
}

void
ThreadPool::Stop()
{
    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        _terminate = true;
    }
    _queueMutexCondition.notify_all();

    for (auto& t : _threads) {
        t.join();
    }

    _threads.clear();
}

bool
ThreadPool::HasTasksQueued() const
{
    bool jobsWaiting;
    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        jobsWaiting = !_jobs.empty();
    }
    return jobsWaiting;
}

bool
ThreadPool::IsIdle() const
{
    bool done;
    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        done = _threadsWaitingCount == _threadsCount && _jobs.empty();
    }
    return done;
}

size_t
ThreadPool::GetThreadsCount() const
{ return _threadsCount; }


/****************************************
 * ThreadPool Private methods
 ****************************************/
void
ThreadPool::threadLoop()
{
    while (true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(_queueMutex);

            _threadsWaitingCount++;

            _queueMutexCondition.wait(lock, [this]() {
                // lock is released while waiting, automatically reaquired when returning from here
                return !_jobs.empty() || _terminate;
            });

            _threadsWaitingCount--;

            if (_terminate) {
                return;
            }

            task = _jobs.front();
            _jobs.pop();
        }

        task();
    }
}
