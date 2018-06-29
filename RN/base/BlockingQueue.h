//
// Created by renming on 6/25/18.
//

#ifndef RN_BLOCKINGQUEUE_H
#define RN_BLOCKINGQUEUE_H
#include <deque>
#include <RN/base/noncopyable.h>
#include <RN/base/Mutex.h>
#include <RN/base/Condition.h>
namespace RN {
template<typename T>
class BlockingQueue : noncopyable {
 public:
  BlockingQueue()
      : mutex_(),
        notEmpty_(mutex_),
        queue_() {

  }
  void put(const T &x) {
    MutexLockGuard lockGuard(mutex_);
    queue_.push_back(x);
    notEmpty_.notify();
  }
  void put(T &&x) {
    MutexLockGuard lockGuard(mutex_);
    queue_.push_back(std::move(x));
    notEmpty_.notify();
  }
  T take() {
    MutexLockGuard lockGuard(mutex_);
    while (queue_.empty()) {
      notEmpty_.wait();
    }
    assert(!queue_.empty());
    T front(std::move(queue_.front()));
    queue_.pop_front();
    return std::move(front);
  }
  size_t size() const {
    MutexLockGuard lockGuard(mutex_);
    return queue_.size();
  }

    bool empty() const {
        MutexLockGuard lockGuard(mutex_);
        return queue_.empty();
    }

    bool full() const {
        MutexLockGuard lockGuard(mutex_);
        return queue_.full();
    }

    size_t capacity() const {
        MutexLockGuard lockGuard(mutex_);
        return queue_.capacity();
    }

 private:
  mutable MutexLock mutex_;
  Condition notEmpty_;
  std::deque<T> queue_;
};
}
#endif //RN_BLOCKINGQUEUE_H
