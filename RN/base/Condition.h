//
// Created by renming on 6/19/18.
//

#ifndef ECHO_CONDITION_H
#define ECHO_CONDITION_H
#include <RN/base/Mutex.h>
#include <pthread.h>
namespace RN {
class Condition : noncopyable {
 public:
  explicit Condition(MutexLock &mutex)
      : mutex_(mutex) {
    MCHECK(pthread_cond_init(&pcond_, NULL));
  }
  ~Condition() {
    MCHECK(pthread_cond_destroy(&pcond_));
  }
  void wait() {
    MutexLock::UnassignGuard ug(mutex_);
    MCHECK(pthread_cond_wait(&pcond_, mutex_.getPthreadMutex()));
  }
  void waitForSeconds(double seconds);
  void notify() {
    MCHECK(pthread_cond_signal(&pcond_));
  }
  void notifyAll() {
    MCHECK(pthread_cond_broadcast(&pcond_));
  }

 private:
  MutexLock &mutex_;
  pthread_cond_t pcond_;
};
}
#endif //ECHO_CONDITION_H
