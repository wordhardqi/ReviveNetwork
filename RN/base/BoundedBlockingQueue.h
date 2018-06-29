//
// Created by rmqi on 28/6/18.
//

#ifndef RN_BOUNDEDBLOCKINGQUEUE_H
#define RN_BOUNDEDBLOCKINGQUEUE_H

#include <RN/base/noncopyable.h>
#include <boost/circular_buffer.hpp>
#include <RN/base/Condition.h>
#include <RN/base/Mutex.h>
#include <assert.h>

namespace RN {
    template<typename T>
    class BoundedBlockingQueue {
    public:
        BoundedBlockingQueue(int maxQueueSize)
                : mutex_(),
                  notEmpty_(mutex_),
                  notFull_(notFull_),
                  queue_(maxQueueSize) {

        }

        T take() {
            MutexLockGuard lockGuard(mutex_);
            while (queue_.empty()) {
                notEmpty_.wait();
            }
            assert(!queue_.empty());
            T front(queue_.front());
            queue_.pop_front();
            notFull_.notify();
            return front;

        }

        void put(const T &val) {
            MutexLock lockGuard(mutex_);
            while (queue_.full()) {
                notFull_.wait();
            }
            assert(!queue_.full());
            queue_.push_back(val);
            notEmpty_.notify();


        }

    private:
        mutable MutexLock mutex_;
        Condition notEmpty_;
        Condition notFull_;
        boost::circular_buffer<T> queue_;

    };

}


#endif //RN_BOUNDEDBLOCKINGQUEUE_H
