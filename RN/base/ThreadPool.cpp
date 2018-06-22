//
// Created by renming on 6/22/18.
//

#include <RN/base/ThreadPool.h>
//  Todo :: the handing on maxQueueSize_==0 is ugly. Need better method
using namespace RN;

ThreadPool::~ThreadPool() {
  if (running_) {
    stop();
  }
}
void ThreadPool::start() {
  assert(!running_);
  threads_.reserve(maxQueueSize_);
  for (int i = 0; i < static_cast<int>(maxQueueSize_); i++) {
    char id[32];
    snprintf(id, sizeof(id), "%d", i + 1);
    threads_.emplace_back(new Thread(std::bind(&ThreadPool::runInThread, this), name_ + id));
  }

  if (maxQueueSize_ == 0) {
    fprintf(stderr, "Warning The thread pool size is 0");
    if (threadInitCallback_) {
      threadInitCallback_();
    }
  }

}
void ThreadPool::stop() {
  MutexLockGuard lockGuard(mutex_);
  running_ = false;
  notEmpty_.notifyAll();
  for (auto &threadPtr : threads_) {
    threadPtr->join();
  }
}
ThreadPool::Task ThreadPool::take() {
  MutexLockGuard lockGuard(mutex_);
  while (taskQueue_.empty() && running_) {
    notEmpty_.wait();
  }
  Task task;
  if (!taskQueue_.empty()) {
    task = taskQueue_.front();
    taskQueue_.pop_front();
    if (maxQueueSize_ > 0) {
      notFull_.notify();
    }
  }
  return task;
}
void ThreadPool::post(ThreadPool::Task task) {
  if (threads_.empty()) {
    task();
  } else {
    MutexLockGuard lockGuard(mutex_);
    while (taskQueue_.size() >= maxQueueSize_ && running_) {
      notFull_.wait();
    }
    assert(taskQueue_.size() < maxQueueSize_);
    taskQueue_.push_back(std::move(task));

  }

}
void ThreadPool::runInThread() {
  try {
    if (threadInitCallback_) {
      threadInitCallback_();
    }
    while (running_) {
      Task task(take());
      if (task) {
        task();
      }
    }
  }
    //copy from muduo.
    // need my own Exception lib on Thread.h
  catch (const std::exception &ex) {
    fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
    fprintf(stderr, "reason: %s\n", ex.what());
    abort();
  }
  catch (...) {
    fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
    throw; // rethrow
  }
}

