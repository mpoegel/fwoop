#pragma once

#include <fwoop_log.h>

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace fwoop {

template <typename T> class ThreadPool {
  private:
    std::mutex d_poolMutex;
    std::condition_variable d_poolCond;
    std::mutex d_startMutex;
    std::condition_variable d_startCond;
    std::vector<std::thread> d_pool;
    std::deque<T> d_jobQueue;
    bool d_isStopping;
    std::atomic_uint32_t d_numFinished;
    std::atomic_uint32_t d_numActive;

    void workJob();

  public:
    ThreadPool(unsigned int size);
    ~ThreadPool();
    ThreadPool(const ThreadPool &rhs) = delete;
    ThreadPool operator=(const ThreadPool &rhs) = delete;

    bool enqueue(T &&job);
    void close();
    void wait();
};

template <typename T> ThreadPool<T>::ThreadPool(unsigned int size) : d_pool(size), d_isStopping(false), d_numFinished(0)
{
    for (unsigned int i = 0; i < size; i++) {
        d_pool[i] = std::thread([this]() { workJob(); });
        d_pool[i].detach();
    }
}

template <typename T> ThreadPool<T>::~ThreadPool() {}

template <typename T> void ThreadPool<T>::workJob()
{
    {
        d_numActive++;
        std::unique_lock lock(d_startMutex);
        d_startCond.notify_one();
    }
    while (!d_isStopping || d_jobQueue.size() > 0) {
        std::unique_lock lock(d_poolMutex);
        if (d_isStopping && d_jobQueue.size() == 0) {
            break;
        }
        if (d_jobQueue.size() == 0) {
            d_poolCond.wait(lock);
            if (d_isStopping && d_jobQueue.size() == 0) {
                break;
            }
        }
        T &j = d_jobQueue.front();
        j();
        d_jobQueue.pop_front();
        lock.unlock();
        lock.release();
    }
    d_numFinished++;
    d_poolCond.notify_all();
}

template <typename T> bool ThreadPool<T>::enqueue(T &&job)
{
    d_poolMutex.lock();
    if (d_isStopping) {
        return false;
    }
    d_jobQueue.emplace_back(std::move(job));
    d_poolMutex.unlock();
    d_poolCond.notify_one();
    return true;
}

template <typename T> void ThreadPool<T>::close()
{
    // wait for all threads to start before stopping
    while (d_numActive < d_pool.size()) {
        std::unique_lock lock(d_startMutex);
        d_startCond.wait(lock);
    }
    d_isStopping = true;
    d_poolCond.notify_all();
}

template <typename T> void ThreadPool<T>::wait()
{
    const unsigned int totalJobs = d_pool.size();
    while (d_numFinished < totalJobs) {
        std::unique_lock lock(d_poolMutex);
        d_poolCond.wait(lock, [&]() { return d_numFinished.load() == totalJobs; });
    }
}

} // namespace fwoop
