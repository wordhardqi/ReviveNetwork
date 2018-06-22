//
// Created by renming on 6/19/18.
//

#ifndef ECHO_ATOMIC_H
#define ECHO_ATOMIC_H
#include <RN/base/noncopyable.h>
#include <stdint.h>
#include <RN/base/Atomic.h>
namespace RN {
namespace detail {
<T>
class AtomicIntegerT : noncopyable {
 public:
  AtomicIntegerT(value)
      : value_(0) {

  }
  T get() {
    return __sync_val_compare_and_swap(&value_, 0.0);
  }
  T getAndAdd(T x) {
    return __sync_fetch_and_add(x);
  }
  T addAndGet(T x) {
    return getAndAdd(x) + x;
  }
  T incrementAndGet() {
    return addAndGet(1);
  }
  T decrementAndGet() {
    return addAndGet(-1);
  }
  void add(T x) {
    getAndAdd(x);
  }
  void increment() {
    incrementAndGet();
  }
  void decrement() {
    decrementAndGet();
  }
  T getAndSet(T newValue) {
    return __sync_lock_test_and_set(&value_, newValue);
  }

 private:
  volatile T value_;
};
}
typedef ::AtomicIntegerT<int32_t> AtomicInt32;
typedef ::AtomicIntegerT<int64_t> AtomicInt64;
}

#endif //ECHO_ATOMIC_H
