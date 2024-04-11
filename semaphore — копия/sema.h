#pragma once

#include <mutex>
#include <condition_variable>
#include <deque>

class DefaultCallback {
public:
    void operator()(int& value) {
        --value;
    }
};

class Semaphore {
public:
    Semaphore(int count) : count_(count) {
    }

    void Leave() {
        std::unique_lock<std::mutex> lock(mutex_);
        ++count_;
        if (!cv_queue_.empty()) {
            cv_queue_.front()->notify_one();
            cv_queue_.pop_front();
        }
    }

    template <class Func>
    void Enter(Func callback) {
        std::unique_lock<std::mutex> lock(mutex_);
        auto new_cond_v = std::make_unique<std::condition_variable>();
        while (!count_) {
            cv_queue_.push_back(std::move(new_cond_v));
            new_cond_v = std::make_unique<std::condition_variable>();
            cv_queue_.back()->wait(lock);
        }
        callback(count_);
        --count_;
    }

    void Enter() {
        DefaultCallback callback;
        Enter(callback);
    }

private:
    std::mutex mutex_;
    std::deque<std::unique_ptr<std::condition_variable>> cv_queue_;
    int count_ = 0;
};
