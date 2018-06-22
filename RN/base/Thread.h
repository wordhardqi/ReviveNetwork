//
// Created by renming on 6/19/18.
//

#ifndef ECHO_THREAD_H
#define ECHO_THREAD_H
#include <string>
#include <RN/base/Atomic.h>
#include <RN/base/CountDownLatch.h>
#include <functional>
#include <RN/base/CurrentThread.h>

using std::string;
namespace RN {

class Thread : noncopyable {
 public:
  typedef std::function<void()> ThreadFunc;
  explicit Thread(ThreadFunc threadFunc, const string &name = string());
  ~Thread();
  void start();
  int join();
  bool started() const { return started_; }
  bool joined() const { return joined_; }
  pid_t tid() const { return tid_; }
  const string &name() const { return name_; }
  static int numCreated() { return numCreated_.get(); }

 private:
  void setDefaultName();
  bool started_;
  bool joined_;
  pthread_t pthreadId_;
  pid_t tid_;
  ThreadFunc func_;
  string name_;
  CountDownLatch latch_;
  static AtomicInt32 numCreated_;
};

}

#endif //ECHO_THREAD_H
