//
// Created by renming on 6/19/18.
//

#ifndef ECHO_MUTEX_H
#define ECHO_MUTEX_H

#include <RN/base/CurrentThread.h>
#include <RN/base/noncopyable.h>
#include <assert.h>
#include <pthread.h>

#ifdef CHECK_PTHREAD_RETURN_VALUE

#ifdef NDEBUG
__BEGIN_DECLS
extern void __assert_perror_fail (int errnum,
                                  const char *file,
                                  unsigned int line,
                                  const char *function)
    __THROW __attribute__ ((__noreturn__));
__END_DECLS
#endif

#define MCHECK(ret) ({ __typeof__ (ret) errnum = (ret);         \
                       if (__builtin_expect(errnum != 0, 0))    \
                         __assert_perror_fail (errnum, __FILE__, __LINE__, __func__);})

#else  // CHECK_PTHREAD_RETURN_VALUE

#define MCHECK(ret) ({ __typeof__ (ret) errnum = (ret);         \
                       assert(errnum == 0); (void) errnum;})

#endif // CHECK_PTHREAD_RETURN_VALUE

namespace RN {

class MutexLock {
 public:
  MutexLock() : holder_(0) {
    MCHECK(pthread_mutex_init(&mutex_, NULL));
  }
  ~MutexLock() {
    MCHECK(pthread_mutex_destroy(&mutex_));
  }
  void lock() {
    MCHECK(pthread_mutex_lock(&mutex_));
    assignHolder();
  }
  void unlock() {
    unAssignHolder();
    MCHECK(pthread_mutex_unlock(&mutex_));
  }
  pthread_mutex_t *getPthreadMutex() {
    return &mutex_;
  }
  bool isLockedByThisThread() const {
    return CurrentThread::tid() == holder_;
  }
  void assertLocked() const {
    assert(isLockedByThisThread());
  }
 private:
  friend class Condition;

  class UnassignedGuard {
   public:
    UnassignedGuard(MutexLock &owner)
        : owner_(owner) {
      owner_.unAssignHolder();
    }
    ~UnassignedGuard() {
      owner_.assignHolder();
    }

   private:
    MutexLock &owner_;
  };

  void assignHolder() {
    holder_ = CurrentThread::tid();

  }
  void unAssignHolder() {
    holder_ = 0;
  }
  pthread_mutex_t mutex_;
  pid_t holder_;

};
class MutexLockGuard {
 public:
  explicit MutexLockGuard(MutexLock &mutex)
      : mutex_(mutex) {
    mutex_.lock();
  }
  ~MutexLockGuard() {
    mutex_.unlock();
  }
 private:
  MutexLock &mutex_;
};

}
#define MutexLockGuard(x) error "Missing guard object name"

#endif //ECHO_MUTEX_H
