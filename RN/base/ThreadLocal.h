//
// Created by renming on 6/20/18.
//

#ifndef ECHO_THREADLOCAL_H
#define ECHO_THREADLOCAL_H
#include <RN/base/noncopyable.h>
#include <pthread.h>
namespace {
template<typename T>
class ThreadLocal : noncopyable {
 public:
  ThreadLocal() {
    MCHECK(pthread_key_create(pkey_, &ThreadLocal::destructor));
  }
  ~ThreadLocal() {
    MCHECK(pthread_key_delete(pkey_));

  }
  T &value() {
    T *perThreadValue = static_cast<T *>(pthread_getspecific(pkey_));
    if (!perThreadValue) {
      T *newObj = new T();
      MCHECK(pthread_setspecific(pkey_, newObj));
      perThreadValue = newObj;
    }
    return *perThreadValue;
  }
 private:
  static void destructor(void *x) {
    T *obj = static_cast<T *>(x);
    //let the compiler report error
    typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
    T_must_be_complete_type dummpy;
    (void) dummy;
    delete obj;
  }
  pthread_key_t pkey_;
};
}
#endif //ECHO_THREADLOCAL_H