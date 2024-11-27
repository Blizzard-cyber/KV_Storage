#ifndef IOPOOL_HPP
#define IOPOOL_HPP

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>
#include <unordered_map>

class IOThreadPool {
public:
    IOThreadPool(size_t numThreads);
    ~IOThreadPool();

    void submit(size_t blockNumber, std::function<void()> task);

private:
    std::vector<std::thread> workers;
    std::unordered_map<size_t, std::queue<std::function<void()>>> tasks;
    std::unordered_map<size_t, std::mutex> queueMutexes;
    std::condition_variable condition;
    bool stop;

    void workerThread(size_t blockNumber);
};

IOThreadPool::IOThreadPool(size_t numThreads) : stop(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back(&IOThreadPool::workerThread, this, i);
    }
}

IOThreadPool::~IOThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutexes[0]);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers) {
        worker.join();
    }
}

void IOThreadPool::submit(size_t blockNumber, std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queueMutexes[blockNumber]);
        tasks[blockNumber].push(task);
    }
    condition.notify_one();
}

void IOThreadPool::workerThread(size_t blockNumber) {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutexes[blockNumber]);
            condition.wait(lock, [this, blockNumber] { return stop || !tasks[blockNumber].empty(); });
            if (stop && tasks[blockNumber].empty()) {
                return;
            }
            task = std::move(tasks[blockNumber].front());
            tasks[blockNumber].pop();
        }
        task();
    }
}

#endif // IOPOOL_HPP