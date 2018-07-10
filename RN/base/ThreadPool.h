//
// Created by renming on 6/22/18.
//

#ifndef RN_THREADPOOL_H
#define RN_THREADPOOL_H
#include <RN/base/Mutex.h>
#include <RN/base/Condition.h>
#include <RN/base/Thread.h>
#include <RN/base/Logging.h>
#include <functional>
#include <vector>
#include <string>
#include <bits/unique_ptr.h>
#include <deque>
using std::string;
namespace RN {
class ThreadPool {
 public:
  typedef std::function<void()> Task;
  ThreadPool(const string &threadPoolName = string("RNThreadPool"), const int maxQueueSize = 0);
  ~ThreadPool();

  void start(size_t numThreadInPool);
  void stop();
  Task take();
  void post(Task task);
  const string &name() const {
    return name_;
  }
  void setInitCallback(Task cb) {
    threadInitCallback_ = cb;
  }
  size_t queueSize() const {
    MutexLockGuard lockGuard(mutex_);
    return taskQueue_.size();
  }
  bool isFull() const;
  void setMaxQueueSize(int maxSize) { maxQueueSize_ = maxSize; }


 private:
  void runInThread();
  mutable MutexLock mutex_;
  Condition notFull_;
  Condition notEmpty_;
  string name_;
  int maxQueueSize_;
  bool running_;
  Task threadInitCallback_;
  std::vector<std::unique_ptr<Thread>> threads_;
  std::deque<Task> taskQueue_;

};
}

#endif //RN_THREADPOOL_H
