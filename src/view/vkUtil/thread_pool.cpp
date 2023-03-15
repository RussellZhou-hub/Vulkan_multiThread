#include "thread_pool.h"

ThreadPool::ThreadPool(size_t threads) : stop(false) {
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back(
            [=] {

                // 设置线程的调度策略为SCHED_FIFO或SCHED_RR
                /*
                struct sched_param param;
                param.sched_priority = 1;
                if (sched_setscheduler(pthread_self(), SCHED_FIFO, &param) != 0) {
                    std::cerr << "Error setting scheduler policy: " << errno << "\n";
                    return;
                }
                */
                
                // 将线程绑定到特定的CPU核心上
                cpu_set_t cpu_set;
                CPU_ZERO(&cpu_set);
                CPU_SET(i % 8, &cpu_set);
                pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu_set);


                for (;;) {
                    std::function<void()> task; // function object encapsulator
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex); // only one thread can access task queue
                        this->condition.wait(lock,                            // release this lock  when blocked
                                             [this] { return this->stop || !this->tasks.empty(); }); // re lock this lock when continue process
                        if (this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    task();
                }
            }
        );
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers)
        worker.join();
}

void ThreadPool::wait(){
    for (std::thread &worker : workers)
        worker.join();
}