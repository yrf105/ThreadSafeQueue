#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>

template<typename T>
class ThreadSafeQueue {
public:
    explicit ThreadSafeQueue(size_t capacity) : capacity_(capacity) {}

    size_t capacity() const {
        return capacity_;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return q_.size();
    }

    void put(const T& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        notFull_.wait(lock, [this]{ return q_.size() < capacity_; });
        q_.push(item);
        notEmpty_.notify_one();
    }

    T take() {
        std::unique_lock<std::mutex> lock(mutex_);
        notEmpty_.wait(lock, [this]{ return !q_.empty(); });
        T ret = std::move(q_.front());
        q_.pop();
        notFull_.notify_one();
        return ret;
    }

    bool take(uint64_t timeOutMs, T* ret = nullptr) {
        std::unique_lock<std::mutex> lock(mutex_);
        bool status = notEmpty_.wait_for(lock, std::chrono::milliseconds(timeOutMs),
                          [this]{ return !q_.empty(); });
        if (status) { // 未超时
            if (ret) {
                *ret = std::move(q_.front());
            }
            q_.pop();
            notFull_.notify_one();
            return true;
        }
        return false;
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable notFull_;
    std::condition_variable notEmpty_;
    size_t capacity_;
    std::queue<T> q_;
};

#endif // THREADSAFEQUEUE_H