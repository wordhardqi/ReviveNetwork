//
// Created by renming on 6/22/18.
//

#ifndef RN_THREADPOOL_H
#define RN_THREADPOOL_H
#include <RN/base/Mutex.h>
#include <RN/base/Condition.h>
#include <RN/base/Thread.h>

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
  ThreadPool(const string &threadPoolName = string("RNThreadPool"), const int maxQueueSize = 0)
      : mutex_(),
        notFull_(mutex_),
        notEmpty_(mutex_),
        name_(threadPoolName),
        maxQueueSize_(maxQueueSize),
        running_(false) {
    //empty
    //consider whether to reserve sapce for threads in this.

  }
  ~ThreadPool();

  void start();
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
    return maxQueueSize_;
  }

 private:
  void runInThread();
  MutexLock mutex_;
  Condition notFull_;
  Condition notEmpty_;
  string name_;
  size_t maxQueueSize_;
  bool running_;
  Task threadInitCallback_;
  std::vector<std::unique_ptr<Thread>> threads_;
  std::deque<Task> taskQueue_;

};
}

#endif //RN_THREADPOOL_H
