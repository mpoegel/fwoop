#include <fwoop_threadpool.h>

#include <atomic>
#include <gtest/gtest.h>
#include <thread>

using namespace std::chrono_literals;

TEST(ThreadPool, EnqueueAndWait)
{
    // GIVEN
    std::atomic_uint16_t count = 0;
    struct Job {
        std::atomic_uint16_t &c;
        void operator()()
        {
            fwoop::Log::Debug("job in progress");
            c++;
        }
    };
    fwoop::ThreadPool<Job> pool(2);
    Job j{count};

    // WHEN
    EXPECT_TRUE(pool.enqueue(std::move(j)));
    EXPECT_TRUE(pool.enqueue(std::move(j)));
    EXPECT_TRUE(pool.enqueue(std::move(j)));

    pool.close();
    pool.wait();

    // THEN
    EXPECT_EQ(count.load(), 3);
}

TEST(ThreadPool, EnqueueAndWaitLambda)
{
    // GIVEN
    std::atomic_uint16_t count = 0;
    auto job = [&count]() {
        std::cout << "lambda job called" << std::endl;
        count++;
    };
    fwoop::ThreadPool<decltype(job)> pool(2);

    // WHEN
    EXPECT_TRUE(pool.enqueue(std::move(job)));
    EXPECT_TRUE(pool.enqueue(std::move(job)));
    EXPECT_TRUE(pool.enqueue(std::move(job)));
    EXPECT_TRUE(pool.enqueue(std::move(job)));

    pool.close();
    pool.wait();

    // THEN
    EXPECT_EQ(count.load(), 4);
}

TEST(ThreadPool, EnqueueLongJob)
{
    // GIVEN
    std::atomic_uint16_t count = 0;
    auto job = [&count]() {
        std::cout << "lambda job called" << std::endl;
        std::this_thread::sleep_for(1s);
        count++;
    };
    fwoop::ThreadPool<decltype(job)> pool(2);

    // WHEN
    EXPECT_TRUE(pool.enqueue(std::move(job)));
    EXPECT_TRUE(pool.enqueue(std::move(job)));
    EXPECT_TRUE(pool.enqueue(std::move(job)));
    EXPECT_TRUE(pool.enqueue(std::move(job)));

    pool.close();
    pool.wait();

    // THEN
    EXPECT_EQ(count.load(), 4);
}
