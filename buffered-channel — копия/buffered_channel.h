#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

template <class T>
class BufferedChannel {
public:
    explicit BufferedChannel(size_t chan_size) : channel_size_(chan_size), closed_(false) {
    }

    void Send(const T& value) {
        auto lock = std::unique_lock<std::mutex>(mutex_);
        while (buffer_.size() == channel_size_ && !closed_) {
            not_full_.wait(lock);
        }
        if (closed_) {
            throw std::runtime_error("поздно отправлять, лавочка закрыта");
        }
        buffer_.push(value);
        not_empty_.notify_one();
    }

    std::optional<T> Recv() {
        auto lock = std::unique_lock<std::mutex>(mutex_);
        while (buffer_.empty() && !closed_) {
            not_empty_.wait(lock);
        }
        if (buffer_.empty()) {
            return std::nullopt;
        }
        auto value = buffer_.front();
        buffer_.pop();
        not_full_.notify_one();
        return value;
    }

    void Close() {
        auto lock = std::lock_guard<std::mutex>(mutex_);
        closed_ = true;
        not_full_.notify_all();
        not_empty_.notify_all();
    }

private:
    size_t channel_size_;
    bool closed_;
    std::queue<T> buffer_;
    std::mutex mutex_;
    std::condition_variable not_full_, not_empty_;
};
