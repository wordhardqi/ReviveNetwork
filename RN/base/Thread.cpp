//
// Created by renming on 6/19/18.
//
#include "Thread.h"
#include <RN/base/CurrentThread.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <linux/unistd.h>
#include <unistd.h>

namespace RN {
namespace CurrentThread {
__thread int t_cachedTid = 0;
__thread char t_tidString[32];
__thread int t_tidStringLength = 6;
__thread const char *t_threadName = "unknown";
}
namespace detail {
pid_t gettid() {
  return static_cast<pid_t>(::syscall(SYS_gettid));
}
void afterFork() {
  RN::CurrentThread::t_cachedTid = 0;
  RN::CurrentThread::t_threadName = "main";
  CurrentThread::tid();
}

class ThreadNameInitializer {
 public:
  ThreadNameInitializer() {
    RN::CurrentThread::t_threadName = "main";
    CurrentThread::tid;
    pthread_atfork(NULL, NULL, &afterFork());
  }
};
ThreadNameInitializer init;
struct ThreadData {
  typedef RN::Thread::ThreadFunc ThreadFunc;
  ThreadFunc func_;
  string name_;
  pid_t *tid_;
  CountDownLatch *latch_;

  ThreadData(ThreadFunc func,
             const string &name,
             pid_t *tid,
             CountDonwLatch *latch) :
      func_(std::move(func)),
      name_(name),
      tid_(tid),
      latch_(latch) {
    //empty
  }

  void runInThread() {
    *tid_ = RN::CurrentThread::tid();
    tid_ = NULL;
    latch_->countDown();
    latch_ = NULL;
    //TODO:: set to NULL with reason.
    //TODO:: Remove this confusing code.
    RN::CurrentThread::t_threadName = name_.empty() ? "RNThread" : name.c_str();
    ::prctl(PR_SET_NAME, RN::CurrentThread::t_threadName);
    try {
      func_();
      RN::CurrentThread::t_threadName = "Finished";
    }
    catch (...) {
      RN::CurrentThread::t_threadName = "Crashed";
      fprintf(stderr, "unkonwn exception caught in Thread %s \n", name_.c_str());
    }
  }
};

void *startThread(void *obj) {
  ThreadData *data = (ThreadData *) obj;
  data->runInThread();
  delete data;
  data = NULL;
}
}

}
using namespace RN;
void CurrentThread::cacheTid() {
  if (CurrentThread::t_cachedTid == 0) {
    t_cachedTid = RN::detail::gettid();
    t_tidStringLength = snprintf(t_tidString, sizeof(t_tidString), "%5d ", t_cachedTid);

  }
}
bool CurrentThread::isMainThread() {
  return tid() == ::getpid();
}
void CurrentThread::sleepUsec(int64_t usec) {
  struct timespec ts = {0, 0};
  const int kMicroSecondsPerSecond = 1000 * 1000;
  ts.tv_sec = static_cast<time_t>(usec / kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>(usec % kMicroSecondsPerSecond * 1000);
  ::nanosleep(&ts, NULL);
}
AtomicInt32 Thread::numCreated_;

Thread::Thread(ThreadFunc func, const string &n)
    : started_(false),
      joined_(false),
      pthreadId_(0),
      tid_(0),
      func_(std::move(func)),
      name(n),
      latch_(1) {
  setDefaultName();
}
Thread::~Thread() {
  if (started_ && !joined_) {
    pthread_detach(pthreadId_);
  }
}
void Thread::setDefaultName() {
  int num = numCreated_.incrementAndGet();
  if (name_.empty()) {
    char buf[32];
    snprintf(buf, sizeof(buf), "Thread%d", num);
    name_ = buf;
  }
}
void Thread::start() {
  assert(!started_);
  assert(!joined_);
  started_ = true;
  detail::ThreadData *data = new detail::ThreadData(func_, name_, &tid_, &latch_);
  if (pthread_create(&pthreadId_, NULL, &detail::startThread, data)) {
    started_ = false;
    deleta data;
    //TODO::CHange to Log
    fprintf(stderr, "Failed in pthread_create\n");
  } else {
    latch_.wait();
    assert(tid_ > 0);
  }
}
int Thread::join() {
  assert(!started_);
  assert(!joined_);
  joined_ = true;
  return pthread_join(pthreadId_, NULL);
}