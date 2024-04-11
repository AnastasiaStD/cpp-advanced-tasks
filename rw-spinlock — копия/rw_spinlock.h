#pragma once

#include <atomic>

struct RWSpinLock {

    std::atomic<int> state_th_ = 0;

    void LockRead() {
        int expct = 0;
        do {
            expct = state_th_.load(std::memory_order_relaxed);
            while (expct % 2 != 0) {
                expct = state_th_.load(std::memory_order_relaxed);
            }
        } while (!state_th_.compare_exchange_weak(expct, expct + 2, std::memory_order_acquire,
                                                  std::memory_order_relaxed));
    }

    void UnlockRead() {
        state_th_.fetch_sub(2, std::memory_order_release);
    }

    void LockWrite() {
        int expct = 0;
        do {
            expct = 0;
        } while (!state_th_.compare_exchange_strong(expct, 1, std::memory_order_acquire,
                                                    std::memory_order_relaxed));
    }

    void UnlockWrite() {
        state_th_.store(0, std::memory_order_release);
    }
};
