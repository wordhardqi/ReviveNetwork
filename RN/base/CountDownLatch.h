//
// Created by renming on 6/19/18.
//

#ifndef ECHO_COUNTDOWNLATCH_H
#define ECHO_COUNTDOWNLATCH_H
#include <Condition.h>
#include <Mutex.h>

namespace RN {
class CountDownLatch {
 public:
  explicit CountDownLatch(int count);
  void wait();
  void countDown();
  int getCount() const;
 private:
  Condition condition_;
  MutexLock mutex_;
  int count_;
};

}

#endif //ECHO_COUNTDOWNLATCH_H
