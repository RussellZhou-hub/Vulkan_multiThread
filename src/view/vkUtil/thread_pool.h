
#pragma once
#include "../../config.h"
#include"../logging.h"
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

class ThreadPool {
public:
    ThreadPool(size_t threads);
    ~ThreadPool();

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
    void wait();

private:
    // need to keep track of threads so we can join them
    std::vector<std::thread> workers;
    // the task queue
    std::queue<std::function<void()>> tasks;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

template <class F, class... Args> // F is function , Args is func parameters
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type> {  //stad::future asyn return encapsulator
    using return_type = typename std::result_of<F(Args...)>::type;
    auto task = std::make_shared<std::packaged_task<return_type()> >(  // std::packaged_task is a func object , encapsulate a asyn operation and return a std::future
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");
        tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return res;
}

/*
ThreadPool pool(4);

std::vector<std::future<void>> futures;
    for (int i = 0; i < 10; ++i)
        futures.emplace_back(pool.enqueue(doSomething, i));

for (auto &&future : futures)
        future.wait();  // wait for asyn operation to finish
*/