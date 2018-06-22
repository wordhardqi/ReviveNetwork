//
// Created by renming on 6/19/18.
//
#include <RN/base/Condition.h>
#include <errno.h>
#include <time.h>
bool RN::Condition::waitForSeconds(double seconds) {
  {
    struct timespec abstime;
    clock_gettime(CLOCK_REALTIME, &abstime);
    const int64_t kNanoSecondsPerSecond = 1000000000;
    const int64_t nanoseconds = static_cast<int>(seconds) * kNanoSecondsPerSecond;
    abstime.tv_sec += static_cast<time_t >((abstime.tv_nsec + nanoseconds) / kNanoSecondsPerSecond);
    abstime.tv_nsec = static_cast<long >((abstime.tv_nsec + nanoseconds) % kNanoSecondsPerSecond);
    MutexLock::UnassignedGuard ug(mutex_);
    return ETIMEDOUT == pthread_cond_timewait(&pcond_, mutex_.getPthreadMutex(), &abstime);
  }
}
