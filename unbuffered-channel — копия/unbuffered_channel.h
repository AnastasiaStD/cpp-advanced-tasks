#pragma once
#include <condition_variable>
#include <mutex>
#include <optional>

template <class T>
class UnbufferedChannel {
public:
    void Send(const T& value) {
        auto lock = std::unique_lock<std::mutex>(mutex_);
        if (closed_) {
            throw std::runtime_error("you have already closed your channel");
        }
        while (value_.has_value() || !receiver_ready_) {
            cv_send_.wait(lock);
            if (closed_) {
                throw std::runtime_error("you have already closed your channel");
            }
        }
        value_ = value;
        receiver_ready_ = false;
        cv_recv_.notify_one();
    }

    std::optional<T> Recv() {
        auto lock = std::unique_lock<std::mutex>(mutex_);
        receiver_ready_ = true;
        cv_send_.notify_one();
        while (!value_.has_value() && !closed_) {
            cv_recv_.wait(lock);
        }
        if (closed_ && !value_.has_value()) {
            return std::nullopt;
        }
        auto value = std::move(value_.value());
        value_.reset();
        return value;
    }

    void Close() {
        auto lock = std::unique_lock<std::mutex>(mutex_);
        closed_ = true;
        cv_recv_.notify_all();
        cv_send_.notify_all();
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_send_, cv_recv_;
    std::optional<T> value_;
    bool closed_ = false;
    bool receiver_ready_ = false;
};
