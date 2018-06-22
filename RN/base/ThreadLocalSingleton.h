//
// Created by renming on 6/20/18.
//

#ifndef ECHO_THREADLOCALSINGLETON_H
#define ECHO_THREADLOCALSINGLETON_H
#include <RN/base/noncopyable.h>
#include <assert.h>
#include <pthread.h>
namespace RN {
template<typename T>
class ThreadLocalSingleton : noncopyable {
 public:
  static T &instance() {
    if (t_value_ == NULL) {
      T *newObj = new T();
      t_value_ = newObj;
      deleter_.set(t_value_);
    }
    return *t_value_;
  }
  static T *pointer() {
    return t_value_;
  }
 private:
  static void destructor(void *obj) {
    assert(t_value_ == obj);
    typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
    T_must_be_complete_type dummy;
    (void) dummy;
    delete t_value_;
    t_value_ = NULL;
  }
  ThreadLocalSingleton();
  ~ThreadLocalSingleton();
  class Deleter {
   public:
    Deleter() {
      pthread_key_create(pkey_, &ThreadLocalSingleton::destructor);
    }
    ~Deleter() {
      pthread_key_delete(pkey_);
    }
    void set(T *obj) {
      assert(pthread_getspecific(pkey_) == NULL);
      pthread_setspecific(pkey_, obj);
    }
    pthread_key_t pkey_;

  };
  static __thread T *t_value_;
  static Deleter deleter_;

};
template<typename T>
__thread T *ThreadLocalSingleton<T>::t_value_ = NULL;
template<typename T>
typename ThreadLocalSingleton<T>::Deleter deleter_;
}
#endif //ECHO_THREADLOCALSINGLETON_H
