//
// Created by renming on 6/22/18.
//

#include <RN/base/ThreadPool.h>
//  Todo :: the handing on maxQueueSize_==0 is ugly. Need better method
using namespace RN;

ThreadPool::ThreadPool(const string &threadPoolName, const int maxQueueSize)
    : mutex_(),
      notFull_(mutex_),
      notEmpty_(mutex_),
      name_(threadPoolName),
      maxQueueSize_(maxQueueSize),
      running_(false),
      threads_(),
      taskQueue_() {
  //empty
  //consider whether to reserve space for threads in this.

}
ThreadPool::~ThreadPool() {
  if (running_) {
    stop();
  }
}
void ThreadPool::start(size_t numThreadInPool) {
  assert(threads_.empty());
  assert(!running_);
  running_ = true;
  threads_.reserve(numThreadInPool);
  for (int i = 0; i < static_cast<int>(numThreadInPool); i++) {
    char id[32];
    snprintf(id, sizeof(id), "%d", i + 1);
    threads_.emplace_back(new Thread(std::bind(&ThreadPool::runInThread, this), name_ + id));
    threads_[i]->start();
  }

  if (numThreadInPool == 0) {
    fprintf(stderr, "Warning The thread pool size is 0 \n");
    if (threadInitCallback_) {
      threadInitCallback_();
    }
  }

}
void ThreadPool::stop() {
    {

        //unlock
        MutexLockGuard lockGuard(mutex_);
        running_ = false;
        notEmpty_.notifyAll();
    }
  for (auto &threadPtr : threads_) {
    threadPtr->join();
  }

}
ThreadPool::Task ThreadPool::take() {
  MutexLockGuard lockGuard(mutex_);
  while (taskQueue_.empty() && running_) {
    notEmpty_.wait();
  }

  Task task = NULL;
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
    while (isFull()) {
      fprintf(stderr, "post wait \n");
      notFull_.wait();
    }
    assert(!isFull());
    taskQueue_.push_back(std::move(task));
    notEmpty_.notify();


  }

}
//size_t ThreadPool::queueSize() const
//{
//  MutexLockGuard lock(mutex_);
//  return taskQueue_.size();
//}


bool ThreadPool::isFull() const {
  mutex_.assertLocked();
  return maxQueueSize_ > 0 && static_cast<int>(taskQueue_.size()) >= (maxQueueSize_);
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
          LOG_INFO << "task finished";
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

