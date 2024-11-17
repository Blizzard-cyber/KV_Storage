#ifndef TASKS_HPP
#define TASKS_HPP

#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>

using namespace std;

enum class TaskType {
    GET,
    PUT,
    DEL
};

struct Task {
    TaskType type;
    string key;
    string value; // Only used for PUT requests
};

class TaskQueue {
public:
    void addTask(const Task& task) {
        unique_lock<mutex> lock(mutex_);
        tasks_.push(task);
        cond_var_.notify_one();
    }

    Task getTask() {
        unique_lock<mutex> lock(mutex_);
        cond_var_.wait(lock, [this] { return !tasks_.empty(); });
        Task task = tasks_.front();
        tasks_.pop();
        return task;
    }

private:
    queue<Task> tasks_;
    mutex mutex_;
    condition_variable cond_var_;
};

#endif // TASKS_HPP