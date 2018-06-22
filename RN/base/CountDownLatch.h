//
// Created by renming on 6/19/18.
//

#ifndef ECHO_COUNTDOWNLATCH_H
#define ECHO_COUNTDOWNLATCH_H
#include <RN/base/Condition.h>
#include <RN/base/Mutex.h>

namespace RN {
class CountDownLatch {
 public:
  explicit CountDownLatch(int count);
  void wait();
  void countDown();
  int getCount() const;
 private:

  //Add mutable qualifier fot int `RN::CountDownLatch::getCount() const`
  mutable MutexLock mutex_;
  Condition condition_;
  int count_;
};

}

#endif //ECHO_COUNTDOWNLATCH_H
