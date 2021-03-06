//
// Created by renming on 6/19/18.
//

#include <RN/base/CountDownLatch.h>
RN::CountDownLatch::CountDownLatch(int count)
    : mutex_(), condition_(mutex_), count_(count) {

}
void RN::CountDownLatch::countDown() {
  MutexLockGuard lockGuard(mutex_);
  --count_;
  if (count_ == 0) {
    condition_.notifyAll();
  }

}
int RN::CountDownLatch::getCount() const {
  MutexLockGuard lockGuard(mutex_);
  return count_;
}
void RN::CountDownLatch::wait() {
  MutexLockGuard lockGuard(mutex_);
  while (count_ > 0) {
    condition_.wait();
  }
}